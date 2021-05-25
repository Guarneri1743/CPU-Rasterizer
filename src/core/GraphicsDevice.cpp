#include "GraphicsDevice.hpp"
#include <iostream>
#include "Singleton.hpp"
#include "GlobalShaderParams.hpp"
#include "Plane.hpp"
#include "Frustum.hpp"
#include "Clipper.hpp"
#include "SegmentDrawer.hpp"
#include "Config.h"
#include <execution>
#include <algorithm>

namespace Guarneri
{
	GraphicsDevice::GraphicsDevice()
	{
		width = 0;
		height = 0;
		row_tile_count = 0;
		col_tile_count = 0;
		tile_length = 0;
		tiles = nullptr;
		msaa_subsample_count = 0;
		subsamples_per_axis = 0;
		statistics.culled_backface_triangle_count = 0;
		statistics.culled_triangle_count = 0;
		statistics.earlyz_optimized = 0;
		statistics.triangle_count = 0;
		multi_thread = true;
		tile_based = true;
		thread_pool = std::make_unique<ThreadPool>(std::thread::hardware_concurrency() - 1);
	}

	GraphicsDevice::~GraphicsDevice()
	{}

	void GraphicsDevice::resize(uint32_t w, uint32_t h)
	{
		delete[] tiles;
		initialize(w, h);
	}

	void GraphicsDevice::initialize(uint32_t w, uint32_t h)
	{
		this->width = w;
		this->height = h;

		// prepare tiles
		int row_rest = h % Config::TILE_SIZE;
		int col_rest = w % Config::TILE_SIZE;
		row_tile_count = h / Config::TILE_SIZE + (row_rest > 0 ? 1 : 0);
		col_tile_count = w / Config::TILE_SIZE + (col_rest > 0 ? 1 : 0);
		tile_length = (int)((long)row_tile_count * (long)col_tile_count);
		tiles = new FrameTile[tile_length];
		FrameTile::build_tiles(tiles, Config::TILE_SIZE, row_tile_count, col_tile_count, row_rest, col_rest);

		// prepare buffers
		zbuffer = std::make_unique<RawBuffer<float>>(w, h);
		shadowmap = std::make_unique<RawBuffer<float>>(w, h);
		framebuffer = std::make_unique<RawBuffer<color_rgba>>(w, h);

		stencilbuffer = std::make_unique<RawBuffer<uint8_t>>(w, h);
		zbuffer->clear(FAR_Z);
		shadowmap->clear(FAR_Z);
		stencilbuffer->clear(DEFAULT_STENCIL);
		framebuffer->clear(DEFAULT_COLOR);
	}

	void GraphicsDevice::set_subsample_count(const uint8_t& count)
	{
		MsaaCommand* cmd = new MsaaCommand();
		cmd->enable = count > 0;
		cmd->msaa_subsample_count = count;
		commands.push(cmd);
	}

	// todo: ugly impl, fix it
	RawBuffer<float>* GraphicsDevice::get_shadowmap()
	{
		return shadowmap.get();
	}

	TileInfo GraphicsDevice::get_tile_info()
	{
		return { Config::TILE_SIZE , Config::TILE_TASK_SIZE , row_tile_count, col_tile_count, tile_length };
	}

	void GraphicsDevice::draw(InputAssemblyTask task)
	{
		const Shader& shader = *task.shader;
		auto v1 = task.v1;
		auto v2 = task.v2;
		auto v3 = task.v3;
		auto m = task.m;
		auto v = task.v;
		auto p = task.p;

		auto object_space_frustum = Frustum::create(p * v * m);
		if ((INST(GlobalShaderParams).culling_clipping_flag & CullingAndClippingFlag::APP_FRUSTUM_CULLING) != CullingAndClippingFlag::DISABLE)
		{
			if (Clipper::conservative_frustum_culling(object_space_frustum, v1, v2, v3))
			{
				statistics.culled_triangle_count++;
				return;
			}
		}
		if ((INST(GlobalShaderParams).culling_clipping_flag & CullingAndClippingFlag::NEAR_PLANE_CLIPPING) != CullingAndClippingFlag::DISABLE)
		{
			auto triangles = Clipper::near_plane_clipping(object_space_frustum.near, v1, v2, v3);
			if (triangles.size() == 0)
			{
				statistics.culled_triangle_count++;
				return;
			}
			for (size_t idx = 0; idx < triangles.size(); idx++)
			{
				auto tri = triangles[idx];
				draw_triangle(shader, tri[0], tri[1], tri[2], m, v, p);
			}
		}
		else
		{
			draw_triangle(shader, v1, v2, v3, m, v, p);
		}
	}

	void GraphicsDevice::enqueue(Shader* shader, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p)
	{
		InputAssemblyTask task = { shader, v1, v2, v3, m, v, p };
		input_assembly_tasks.emplace_back(thread_pool->enqueue([this, task]
		{
			this->draw(task);
		}));
	}

	void GraphicsDevice::fence()
	{
		// fence input assembly tasks
		for (auto& task : input_assembly_tasks) { task.get(); }
		input_assembly_tasks.clear();
	}

	void GraphicsDevice::present()
	{
		// fence rasteriztion tasks
		for (uint32_t i = 0; i < tile_length; i++)
		{
			FrameTile* tile = &tiles[i];
			tile_tasks.emplace_back(thread_pool->enqueue([this, tile] {
				this->rasterize_tile(*tile);
			}));
		}

		for (auto& task : tile_tasks) { task.get(); }
		tile_tasks.clear();

		// fence msaa resolve tasks
		if (INST(GlobalShaderParams).enable_msaa)
		{
			for (uint32_t i = 0; i < tile_length; i++)
			{
				FrameTile* tile = &tiles[i];
				tile_tasks.emplace_back(thread_pool->enqueue([this, tile]
				{
					this->resolve_tile(*tile);
				}));
			}

			for (auto& task : tile_tasks) { task.get(); }
			tile_tasks.clear();
		}
	}

	void GraphicsDevice::process_commands()
	{
		while (!commands.empty())
		{
			auto cmd = commands.front();
			commands.pop();
			MsaaCommand* msaa = dynamic_cast<MsaaCommand*>(cmd);
			if (msaa != nullptr)
			{
				INST(GlobalShaderParams).enable_msaa = msaa->enable;
				uint8_t count = msaa->msaa_subsample_count;
				if (count > 0)
				{
					const int w = this->width;
					const int h = this->height;
					msaa_subsample_count = count;
					subsamples_per_axis = (uint8_t)(std::sqrt((double)count));
					msaa_colorbuffer = std::make_unique<RawBuffer<color_rgba>>(w * subsamples_per_axis, h * subsamples_per_axis);
					msaa_zbuffer = std::make_unique<RawBuffer<float>>(w * subsamples_per_axis, h * subsamples_per_axis);
					msaa_coveragebuffer = std::make_unique<RawBuffer<uint8_t>>(w * subsamples_per_axis, h * subsamples_per_axis);
					msaa_stencilbuffer = std::make_unique<RawBuffer<uint8_t>>(w * subsamples_per_axis, h * subsamples_per_axis);
				}
				delete msaa;
			}
		}
	}

	void GraphicsDevice::clear_buffer(const BufferFlag& flag)
	{
		//todo
		process_commands();

		if ((flag & BufferFlag::COLOR) != BufferFlag::NONE)
		{
			if ((INST(GlobalShaderParams).render_flag & RenderFlag::DEPTH) != RenderFlag::DISABLE || (INST(GlobalShaderParams).render_flag & RenderFlag::SHADOWMAP) != RenderFlag::DISABLE)
			{
				framebuffer->clear(DEFAULT_DEPTH_COLOR);
			}
			else
			{
				framebuffer->clear(DEFAULT_COLOR);
			}
		}

		if ((flag & BufferFlag::DEPTH) != BufferFlag::NONE)
		{
			zbuffer->clear(FAR_Z);
		}
		if ((flag & BufferFlag::STENCIL) != BufferFlag::NONE)
		{
			stencilbuffer->clear(DEFAULT_STENCIL);
		}

		shadowmap->clear(FAR_Z);

		if (INST(GlobalShaderParams).enable_msaa)
		{
			msaa_colorbuffer->clear(DEFAULT_COLOR);
			msaa_zbuffer->clear(FAR_Z);
			msaa_coveragebuffer->clear(0);
			msaa_stencilbuffer->clear(DEFAULT_STENCIL);
		}
		statistics.culled_triangle_count = 0;
		statistics.culled_backface_triangle_count = 0;
		statistics.triangle_count = 0;
		statistics.earlyz_optimized = 0;
	}

	void GraphicsDevice::draw_triangle(const Shader& shader, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p)
	{
		UNUSED(m);
		UNUSED(v);
		UNUSED(p);

		statistics.triangle_count++;

		v2f o1 = process_vertex(shader, v1);
		v2f o2 = process_vertex(shader, v2);
		v2f o3 = process_vertex(shader, v3);

		Vertex c1(o1.position, o1.world_pos, o1.shadow_coord, o1.color, o1.normal, o1.uv, o1.tangent, o1.bitangent);
		Vertex c2(o2.position, o2.world_pos, o2.shadow_coord, o2.color, o2.normal, o2.uv, o2.tangent, o2.bitangent);
		Vertex c3(o3.position, o3.world_pos, o3.shadow_coord, o3.color, o3.normal, o3.uv, o3.tangent, o3.bitangent);

		// clip in screen space
		//if (!shader.skybox && Clipper::cvv_clipping(c1.position, c2.position, c3.position)) {
		//	return;
		//}

		// perspective division, position.
		Vertex n1 = clip2ndc(c1);
		Vertex n2 = clip2ndc(c2);
		Vertex n3 = clip2ndc(c3);

		bool double_face = shader.double_face;
		bool enable_backface_culling = (INST(GlobalShaderParams).culling_clipping_flag & CullingAndClippingFlag::BACK_FACE_CULLING) != CullingAndClippingFlag::DISABLE;

		bool culled_back_face = false;

		if (!double_face && enable_backface_culling && !shader.skybox)
		{
			if (Clipper::backface_culling(n1.position, n2.position, n3.position))
			{
				culled_back_face = true;
				if ((INST(GlobalShaderParams).render_flag & RenderFlag::CULLED_BACK_FACE) == RenderFlag::DISABLE)
				{
					statistics.culled_backface_triangle_count++;
					return;
				}
			}
		}

		// perspective division, uv, color, normal, etc. 
		n1.perspective_division();
		n2.perspective_division();
		n3.perspective_division();

		Vertex s1 = ndc2viewport(n1);
		Vertex s2 = ndc2viewport(n2);
		Vertex s3 = ndc2viewport(n3);

		Triangle tri(s1, s2, s3);

		// primitive assembly
		std::vector<Triangle> tris = tri.horizontally_split();

		for (auto iter = tris.begin(); iter != tris.end(); iter++)
		{
			if (culled_back_face && (INST(GlobalShaderParams).render_flag & RenderFlag::CULLED_BACK_FACE) != RenderFlag::DISABLE)
			{
				(*iter).culled = true;
			}
			if (this->tile_based)
			{
				FrameTile::dispatch_render_task(tiles, row_tile_count, col_tile_count, *iter, shader, this->width, this->height, Config::TILE_SIZE);
			}
			else
			{
				rasterize(*iter, shader, RasterizerStrategy::SCANLINE);
			}
		}
	}

	// per vertex processing
	v2f GraphicsDevice::process_vertex(const Shader& shader, const Vertex& vert) const
	{
		a2v input;
		input.position = vert.position;
		input.uv = vert.uv;
		input.color = vert.color;
		input.normal = vert.normal;
		input.tangent = vert.tangent;
		return shader.vertex_shader(input);
	}

	void GraphicsDevice::rasterize_tiles(const size_t& start, const size_t& end)
	{
		assert(end >= start);
		for (auto idx = start; idx < end; idx++)
		{
			rasterize_tile(tiles[idx]);
		}
	}

	void GraphicsDevice::rasterize_tile(FrameTile& tile)
	{
		while (!tile.tasks.empty())
		{
			TileTask task;
			if (tile.pop_task(task))
			{
				auto tri = task.triangle;
				const Shader& shader = *task.shader;

				rasterize(tile, tri, shader);

				// wireframe
				if ((INST(GlobalShaderParams).render_flag & RenderFlag::WIREFRAME) != RenderFlag::DISABLE)
				{
					draw_screen_segment(tri[0].position, tri[1].position, Color(0.5f, 0.5f, 1.0f, 1.0f));
					draw_screen_segment(tri[0].position, tri[2].position, Color(0.5f, 0.5f, 1.0f, 1.0f));
					draw_screen_segment(tri[2].position, tri[1].position, Color(0.5f, 0.5f, 1.0f, 1.0f));
				}

				if (tri.culled && ((INST(GlobalShaderParams).render_flag & RenderFlag::CULLED_BACK_FACE) != RenderFlag::DISABLE))
				{
					statistics.culled_backface_triangle_count++;
					draw_screen_segment(tri[0].position, tri[1].position, Color(0.0f, 1.0f, 0.0f, 1.0f));
					draw_screen_segment(tri[0].position, tri[2].position, Color(0.0f, 1.0f, 0.0f, 1.0f));
					draw_screen_segment(tri[2].position, tri[1].position, Color(0.0f, 1.0f, 0.0f, 1.0f));
				}
			}
		}

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::FRAME_TILE) != RenderFlag::DISABLE)
		{
			for (uint32_t row = (uint32_t)tile.row_start; row < (uint32_t)tile.row_end; row++)
			{
				for (uint32_t col = (uint32_t)tile.col_start; col < (uint32_t)tile.col_end; col++)
				{
					float r = (float)row / tile.row_end;
					float g = (float)col / tile.col_end;
					color_rgba dst;
					if (framebuffer->read(row, col, dst))
					{
						Color dst_color = Color::decode(dst);
						Color src_color = Color(r, g, 0.0f, 0.5f);
						Color blended_color = blend(src_color, dst_color, BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendOp::ADD);
						auto pixel_color = Color::encode_rgba(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
						framebuffer->write(row, col, pixel_color);
					}
				}
			}
		}
	}

	void GraphicsDevice::resolve_tiles(const size_t& start, const size_t& end)
	{
		assert(end >= start);
		for (auto idx = start; idx < end; idx++)
		{
			resolve_tile(tiles[idx]);
		}
	}

	void GraphicsDevice::resolve_tile(FrameTile& tile)
	{
		for (uint32_t row = (uint32_t)tile.row_start; row < (uint32_t)tile.row_end; row++)
		{
			for (uint32_t col = (uint32_t)tile.col_start; col < (uint32_t)tile.col_end; col++)
			{
				uint8_t coverage_count = 0;
				Color pixel_color;
				for (int x_subsample_idx = 0; x_subsample_idx < subsamples_per_axis; x_subsample_idx++)
				{
					for (int y_subsample_idx = 0; y_subsample_idx < subsamples_per_axis; y_subsample_idx++)
					{
						uint32_t sub_row = (uint32_t)(row * subsamples_per_axis + x_subsample_idx);
						uint32_t sub_col = (uint32_t)(col * subsamples_per_axis + y_subsample_idx);
						uint8_t coverage = 0;
						if (msaa_coveragebuffer->read(sub_row, sub_col, coverage) && coverage)
						{
							coverage_count++;
							color_rgba msaa_color;
							msaa_colorbuffer->read(sub_row, sub_col, msaa_color);
							pixel_color += Color::decode(msaa_color);
						}
					}
				}

				if (coverage_count > 0)
				{
					pixel_color *= 1.0f / msaa_subsample_count;
					pixel_color.a = 1.0f;
					framebuffer->write(row, col, Color::encode_rgba(pixel_color));
				}
			}
		}
	}

	void GraphicsDevice::rasterize(const FrameTile& tile, const Triangle& tri, const Shader& shader)
	{
		auto bounds = Rect(tri[0].position.xy(), tri[1].position.xy(), tri[2].position.xy());
		int row_start = (int)(bounds.min().y + 0.5f) - 1;
		int row_end = (int)(bounds.max().y + 0.5f) + 1;
		int col_start = (int)(bounds.min().x + 0.5f) - 1;
		int col_end = (int)(bounds.max().x + 0.5f) + 1;

		row_start = std::clamp(row_start, tile.row_start, tile.row_end);
		row_end = std::clamp(row_end, tile.row_start, tile.row_end);
		col_start = std::clamp(col_start, tile.col_start, tile.col_end);
		col_end = std::clamp(col_end, tile.col_start, tile.col_end);

		bool flip = tri.flip;
		int ccw_idx0 = 0;
		int ccw_idx1 = flip ? 2 : 1;
		int ccw_idx2 = flip ? 1 : 2;

		auto& v0 = tri[ccw_idx0];
		auto& v1 = tri[ccw_idx1];
		auto& v2 = tri[ccw_idx2];

		auto p0 = v0.position.xy();
		auto p1 = v1.position.xy();
		auto p2 = v2.position.xy();

		float area = Triangle::area_double(p0, p1, p2);

		if (INST(GlobalShaderParams).enable_msaa && !shader.shadow)
		{
			for (uint32_t row = (uint32_t)row_start; row < (uint32_t)row_end; row++)
			{
				for (uint32_t col = (uint32_t)col_start; col < (uint32_t)col_end; col++)
				{
					process_subsamples(msaa_colorbuffer.get(), msaa_zbuffer.get(), msaa_stencilbuffer.get(), row, col, shader, v0, v1, v2, area);
				}
			}
		}
		else
		{
			for (uint32_t row = (uint32_t)row_start; row < (uint32_t)row_end; row++)
			{
				for (uint32_t col = (uint32_t)col_start; col < (uint32_t)col_end; col++)
				{
					Vector2 pixel((float)col + 0.5f, (float)row + 0.5f);
					float w0 = Triangle::area_double(p1, p2, pixel);
					float w1 = Triangle::area_double(p2, p0, pixel);
					float w2 = Triangle::area_double(p0, p1, pixel);
					if (w0 >= 0 && w1 >= 0 && w2 >= 0)
					{
						w0 /= area; w1 /= area; w2 /= area;
						Vertex vert = Vertex::barycentric_interpolate(v0, v1, v2, w0, w1, w2);
						RawBuffer<float>* zbuf = shader.shadow ? shadowmap.get() : zbuffer.get();
						process_fragment(framebuffer.get(), zbuf, stencilbuffer.get(), vert, row, col, shader);
					}
				}
			}
		}
	}

	void GraphicsDevice::rasterize(const Triangle& tri, const Shader& shader, const RasterizerStrategy& strategy)
	{
		if (strategy == RasterizerStrategy::SCANBLOCK)
		{
			scanblock(tri, shader);
		}
		else
		{
			scanline(tri, shader);
		}
		// wireframe
		if ((INST(GlobalShaderParams).render_flag & RenderFlag::WIREFRAME) != RenderFlag::DISABLE)
		{
			draw_screen_segment(tri[0].position, tri[1].position, Color(1.0f, 1.0f, 1.0f, 1.0f));
			draw_screen_segment(tri[0].position, tri[2].position, Color(1.0f, 1.0f, 1.0f, 1.0f));
			draw_screen_segment(tri[2].position, tri[1].position, Color(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	void GraphicsDevice::scanblock(const Triangle& tri, const Shader& shader)
	{
		auto bounds = Rect(tri[0].position.xy(), tri[1].position.xy(), tri[2].position.xy());
		int row_start = (int)(bounds.min().y + 0.5f) - 1;
		int row_end = (int)(bounds.max().y + 0.5f) + 1;
		int col_start = (int)(bounds.min().x + 0.5f) - 1;
		int col_end = (int)(bounds.max().x + 0.5f) + 1;

		row_start = std::clamp(row_start, 0, this->height);
		row_end = std::clamp(row_end, 0, this->height);
		col_start = std::clamp(col_start, 0, this->width);
		col_end = std::clamp(col_end, 0, this->width);

		bool flip = tri.flip;
		int ccw_idx0 = 0;
		int ccw_idx1 = flip ? 2 : 1;
		int ccw_idx2 = flip ? 1 : 2;

		auto& v0 = tri[ccw_idx0];
		auto& v1 = tri[ccw_idx1];
		auto& v2 = tri[ccw_idx2];

		auto p0 = v0.position.xy();
		auto p1 = v1.position.xy();
		auto p2 = v2.position.xy();

		float area = Triangle::area_double(p0, p1, p2);
		float inv_area = 1.0f / area;

		for (uint32_t row = row_start; row < (uint32_t)row_end; row++)
		{
			for (uint32_t col = col_start; col < (uint32_t)col_end; col++)
			{
				Vector2 pixel((float)col + 0.5f, (float)row + 0.5f);
				float w0 = Triangle::area_double(p1, p2, pixel);
				float w1 = Triangle::area_double(p2, p0, pixel);
				float w2 = Triangle::area_double(p0, p1, pixel);
				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
					w0 *= inv_area; w1 *= inv_area; w2 *= inv_area;
					Vertex vert = Vertex::barycentric_interpolate(tri[ccw_idx0], tri[ccw_idx1], tri[ccw_idx2], w0, w1, w2);
					RawBuffer<float>* zbuf = shader.shadow ? shadowmap.get() : zbuffer.get();
					process_fragment(framebuffer.get(), zbuf, stencilbuffer.get(), vert, row, col, shader);
				}
			}
		}
	}

	void GraphicsDevice::scanline(const Triangle& tri, const Shader& shader)
	{
		bool flip = tri.flip;
		int top_idx = flip ? 2 : 0;
		int bottom_idx = flip ? 0 : 2;
		int top = (int)(tri[top_idx].position.y + 0.5f);
		int bottom = (int)(tri[bottom_idx].position.y + 0.5f);
		top = std::clamp(top, 0, this->height);
		bottom = std::clamp(bottom, 0, this->height);
		assert(bottom >= top);

		for (uint32_t row = (uint32_t)top; row < (uint32_t)bottom; row++)
		{
			Vertex lhs, rhs;
			tri.interpolate((float)row + 0.5f, lhs, rhs);

			// screen space clipping
			bool enable_screen_clipping = (INST(GlobalShaderParams).culling_clipping_flag & CullingAndClippingFlag::SCREEN_CLIPPING) != CullingAndClippingFlag::DISABLE;
			if (enable_screen_clipping)
			{
				Clipper::screen_clipping(lhs, rhs, this->width);
			}
			int left = (int)(lhs.position.x + 0.5f);
			left = std::clamp(left, 0, this->width);
			int right = (int)(rhs.position.x + 0.5f);
			right = std::clamp(right, 0, this->width);
			assert(right >= left);

			for (uint32_t col = (uint32_t)left; col < (uint32_t)right; col++)
			{
				RawBuffer<float>* zbuf = shader.shadow ? shadowmap.get() : zbuffer.get();
				process_fragment(framebuffer.get(), zbuf, stencilbuffer.get(), lhs, row, col, shader);
				auto dx = Vertex::differential(lhs, rhs);
				lhs = Vertex::intagral(lhs, dx);
			}
		}
	}

	void GraphicsDevice::process_subsamples(RawBuffer<color_rgba>* fbuf, RawBuffer<float>* zbuf, RawBuffer<uint8_t>* stencilbuf, const uint32_t& row, const uint32_t& col, const Shader& shader, const Vertex& v0, const Vertex& v1, const Vertex& v2, const float& area)
	{
		auto p0 = v0.position.xy();
		auto p1 = v1.position.xy();
		auto p2 = v2.position.xy();

		float subsample_step = 1.0f / (float)subsamples_per_axis;
		int sample_idx = 0;
		Vector2 pixel((float)col + 0.5f, (float)row + 0.5f);
		bool coverage = false;
		bool color_calculated = false;

		Color fragment_result;
		color_rgba pixel_color;
		for (int x_subsample_idx = 0; x_subsample_idx < subsamples_per_axis; x_subsample_idx++)
		{
			for (int y_subsample_idx = 0; y_subsample_idx < subsamples_per_axis; y_subsample_idx++)
			{
				Vector2 subsample((float)col + 0.25f + x_subsample_idx * subsample_step, (float)row + 0.25f + y_subsample_idx * subsample_step);
				float w0 = Triangle::area_double(p1, p2, subsample);
				float w1 = Triangle::area_double(p2, p0, subsample);
				float w2 = Triangle::area_double(p0, p1, subsample);
				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
					coverage = true;
					w0 /= area; w1 /= area; w2 /= area;
					Vertex vert = Vertex::barycentric_interpolate(v0, v1, v2, w0, w1, w2);
					uint32_t subsample_row = (uint32_t)(row * subsamples_per_axis + x_subsample_idx);
					uint32_t subsample_col = (uint32_t)(col * subsamples_per_axis + y_subsample_idx);


					bool enable_scissor_test = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::SCISSOR_TEST) != PerSampleOperation::DISABLE;
					bool enable_alpha_test = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::ALPHA_TEST) != PerSampleOperation::DISABLE;
					bool enable_stencil_test = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::STENCIL_TEST) != PerSampleOperation::DISABLE;
					bool enable_depth_test = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::DEPTH_TEST) != PerSampleOperation::DISABLE;
					ColorMask color_mask = shader.color_mask;
					CompareFunc stencil_func = shader.stencil_func;
					StencilOp stencil_pass_op = shader.stencil_pass_op;
					StencilOp stencil_fail_op = shader.stencil_fail_op;
					StencilOp stencil_zfail_op = shader.stencil_zfail_op;
					uint8_t stencil_read_mask = shader.stencil_read_mask;
					uint8_t stencil_write_mask = shader.stencil_write_mask;
					uint8_t stencil_ref_val = shader.stencil_ref_val;
					CompareFunc ztest_func = shader.ztest_func;
					ZWrite zwrite_mode = shader.zwrite_mode;
					BlendFactor src_factor = shader.src_factor;
					BlendFactor dst_factor = shader.dst_factor;
					BlendOp blend_op = shader.blend_op;

					UNUSED(enable_scissor_test);
					UNUSED(enable_alpha_test);
					UNUSED(color_mask);
					UNUSED(stencil_write_mask);

					bool enable_blending = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::BLENDING) != PerSampleOperation::DISABLE && shader.transparent;

					PerSampleOperation op_pass = PerSampleOperation::SCISSOR_TEST | PerSampleOperation::ALPHA_TEST | PerSampleOperation::STENCIL_TEST | PerSampleOperation::DEPTH_TEST;

					msaa_coveragebuffer->write(subsample_row, subsample_col, 1);

					if (!color_calculated)
					{
						// pixel frequent msaa
						w0 = Triangle::area_double(p1, p2, pixel);
						w1 = Triangle::area_double(p2, p0, pixel);
						w2 = Triangle::area_double(p0, p1, pixel);
						w0 /= area; w1 /= area; w2 /= area;
						Vertex v = Vertex::barycentric_interpolate(v0, v1, v2, w0, w1, w2);

						v2f v_out;
						float w = 1.0f / v.rhw;
						v_out.position = v.position;
						v_out.world_pos = v.world_pos * w;
						v_out.shadow_coord = v.shadow_coord * w;
						v_out.color = v.color * w;
						v_out.normal = v.normal * w;
						v_out.uv = v.uv * w;
						v_out.tangent = v.tangent * w;
						v_out.bitangent = v.bitangent * w;
						fragment_result = shader.fragment_shader(v_out);
						pixel_color = Color::encode_rgba(fragment_result);

						color_calculated = true;
					}

					// stencil test
					if (enable_stencil_test)
					{
						if (!perform_stencil_test(stencilbuf, stencil_ref_val, stencil_read_mask, stencil_func, subsample_row, subsample_col))
						{
							op_pass &= ~PerSampleOperation::STENCIL_TEST;
						}
					}

					update_stencil_buffer(stencilbuf, subsample_row, subsample_col, op_pass, stencil_pass_op, stencil_fail_op, stencil_zfail_op, stencil_ref_val);

					// z test
					if (enable_depth_test)
					{
						if (!perform_depth_test(zbuf, ztest_func, subsample_row, subsample_col, vert.position.z))
						{
							op_pass &= ~PerSampleOperation::DEPTH_TEST;
						}
					}

					if (zwrite_mode == ZWrite::ON && (op_pass & PerSampleOperation::DEPTH_TEST) != PerSampleOperation::DISABLE)
					{
						zbuf->write(subsample_row, subsample_col, vert.position.z);
					}

					// blending
					if (enable_blending && shader.transparent)
					{
						color_rgba dst;
						if (fbuf->read(subsample_row, subsample_col, dst))
						{
							Color dst_color = Color::decode(dst);
							Color src_color = fragment_result;
							Color blended_color = blend(src_color, dst_color, src_factor, dst_factor, blend_op);
							pixel_color = Color::encode_rgba(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
						}
					}

					if (validate_fragment(op_pass))
					{
						fbuf->write(subsample_row, subsample_col, pixel_color);
					}
				}

				sample_idx++;
			}
		}
	}

	// per fragment processing
	void GraphicsDevice::process_fragment(RawBuffer<color_rgba>* fbuf, RawBuffer<float>* zbuf, RawBuffer<uint8_t>* stencilbuf, const Vertex& v, const uint32_t& row, const uint32_t& col, const Shader& shader)
	{
		bool enable_scissor_test = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::SCISSOR_TEST) != PerSampleOperation::DISABLE;
		bool enable_alpha_test = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::ALPHA_TEST) != PerSampleOperation::DISABLE;
		bool enable_stencil_test = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::STENCIL_TEST) != PerSampleOperation::DISABLE;
		bool enable_depth_test = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::DEPTH_TEST) != PerSampleOperation::DISABLE;

		PerSampleOperation op_pass = PerSampleOperation::SCISSOR_TEST | PerSampleOperation::ALPHA_TEST | PerSampleOperation::STENCIL_TEST | PerSampleOperation::DEPTH_TEST;

		float z = v.position.z;

		ColorMask color_mask = shader.color_mask;
		CompareFunc stencil_func = shader.stencil_func;
		StencilOp stencil_pass_op = shader.stencil_pass_op;
		StencilOp stencil_fail_op = shader.stencil_fail_op;
		StencilOp stencil_zfail_op = shader.stencil_zfail_op;
		uint8_t stencil_read_mask = shader.stencil_read_mask;
		uint8_t stencil_write_mask = shader.stencil_write_mask;
		uint8_t stencil_ref_val = shader.stencil_ref_val;
		CompareFunc ztest_func = shader.ztest_func;
		ZWrite zwrite_mode = shader.zwrite_mode;
		BlendFactor src_factor = shader.src_factor;
		BlendFactor dst_factor = shader.dst_factor;
		BlendOp blend_op = shader.blend_op;

		UNUSED(stencil_write_mask);

		bool enable_blending = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::BLENDING) != PerSampleOperation::DISABLE && shader.transparent;

		// early-z
		if (enable_depth_test && !enable_alpha_test)
		{
			if (!perform_depth_test(zbuf, ztest_func, row, col, z))
			{
				op_pass &= ~PerSampleOperation::DEPTH_TEST;
				statistics.earlyz_optimized++;
			}
		}

		color_rgba pixel_color;

		// fragment shader
		Color fragment_result;
		v2f v_out;
		float w = 1.0f / v.rhw;
		v_out.position = v.position;
		v_out.world_pos = v.world_pos * w;
		v_out.shadow_coord = v.shadow_coord * w;
		v_out.color = v.color * w;
		v_out.normal = v.normal * w;
		v_out.uv = v.uv * w;
		v_out.tangent = v.tangent * w;
		v_out.bitangent = v.bitangent * w;

		// todo: ddx ddy
		fragment_result = shader.fragment_shader(v_out);
		pixel_color = Color::encode_rgba(fragment_result);

		// todo: scissor test
		if (enable_scissor_test)
		{

		}

		// acctually alpha test is a deprecated feature in newer graphics API, cuz it can be emulated by discard() or clip()
		if (enable_alpha_test)
		{
			if (shader.discarded)
			{
				return;
			}
		}

		// stencil test
		if (enable_stencil_test)
		{
			if (!perform_stencil_test(stencilbuf, stencil_ref_val, stencil_read_mask, stencil_func, row, col))
			{
				op_pass &= ~PerSampleOperation::STENCIL_TEST;
			}
			update_stencil_buffer(stencilbuf, row, col, op_pass, stencil_pass_op, stencil_fail_op, stencil_zfail_op, stencil_ref_val);
		}

		// depth test
		if (enable_depth_test)
		{
			if (!perform_depth_test(zbuf, ztest_func, row, col, z))
			{
				op_pass &= ~PerSampleOperation::DEPTH_TEST;
			}
		}

		// write depth
		if (zwrite_mode == ZWrite::ON && (op_pass & PerSampleOperation::DEPTH_TEST) != PerSampleOperation::DISABLE)
		{
			zbuf->write(row, col, z);
		}

		// blending
		if (enable_blending && shader.transparent)
		{
			color_rgba dst;
			if (fbuf->read(row, col, dst))
			{
				Color dst_color = Color::decode(dst);
				Color src_color = fragment_result;
				Color blended_color = blend(src_color, dst_color, src_factor, dst_factor, blend_op);
				pixel_color = Color::encode_rgba(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
			}
		}

		// write color
		if (validate_fragment(op_pass))
		{
			if (color_mask == (ColorMask::R | ColorMask::G | ColorMask::B | ColorMask::A))
			{
				fbuf->write(row, col, pixel_color);
			}
			else
			{
				color_rgba cur;
				if (fbuf->read(row, col, cur))
				{
					if ((color_mask & ColorMask::R) == ColorMask::ZERO)
					{
						pixel_color.r = cur.r;
					}
					if ((color_mask & ColorMask::G) == ColorMask::ZERO)
					{
						pixel_color.g = cur.g;
					}
					if ((color_mask & ColorMask::B) == ColorMask::ZERO)
					{
						pixel_color.b = cur.b;
					}
					if ((color_mask & ColorMask::A) == ColorMask::ZERO)
					{
						pixel_color.a = cur.a;
					}
					fbuf->write(row, col, pixel_color);
				}
			}
		}

		// stencil visualization
		if ((INST(GlobalShaderParams).render_flag & RenderFlag::STENCIL) != RenderFlag::DISABLE)
		{
			uint8_t stencil;
			if (stencilbuf->read(row, col, stencil))
			{
				color_rgba c = Color::encode_rgba(stencil, stencil, stencil, 255);
				fbuf->write(row, col, c);
			}
		}

		// depth buffer visualization
		if ((INST(GlobalShaderParams).render_flag & RenderFlag::DEPTH) != RenderFlag::DISABLE)
		{
			float cur_depth;
			if (this->zbuffer->read(row, col, cur_depth))
			{
				float linear_depth = linearize_01depth(cur_depth, INST(GlobalShaderParams).cam_near, INST(GlobalShaderParams).cam_far);
				Color depth_color = Color::WHITE * linear_depth;
				color_rgba c = Color::encode_rgba(depth_color);
				fbuf->write(row, col, c);
			}
		}

		// shadowmap visualization
		if ((INST(GlobalShaderParams).render_flag & RenderFlag::SHADOWMAP) != RenderFlag::DISABLE)
		{
			float cur_depth;
			if (this->get_shadowmap()->read(row, col, cur_depth))
			{
				Color depth_color = Color::WHITE * cur_depth;
				color_rgba c = Color::encode_rgba(depth_color);
				fbuf->write(row, col, c);
			}
		}
	}

	bool GraphicsDevice::validate_fragment(const PerSampleOperation& op_pass) const
	{
		if ((op_pass & PerSampleOperation::SCISSOR_TEST) == PerSampleOperation::DISABLE) return false;
		if ((op_pass & PerSampleOperation::ALPHA_TEST) == PerSampleOperation::DISABLE) return false;
		if ((op_pass & PerSampleOperation::STENCIL_TEST) == PerSampleOperation::DISABLE) return false;
		if ((op_pass & PerSampleOperation::DEPTH_TEST) == PerSampleOperation::DISABLE) return false;
		return true;
	}

	bool GraphicsDevice::perform_stencil_test(RawBuffer<uint8_t>* stencilbuf, const uint8_t& ref_val, const uint8_t& read_mask, const CompareFunc& func, const uint32_t& row, const uint32_t& col) const
	{
		bool pass = false;
		uint8_t stencil;
		if (stencilbuf->read(row, col, stencil))
		{
			switch (func)
			{
			case CompareFunc::NEVER:
				pass = false;
				break;
			case CompareFunc::ALWAYS:
				pass = true;
				break;
			case CompareFunc::EQUAL:
				pass = (ref_val & read_mask) == (stencil & read_mask);
				break;
			case CompareFunc::GREATER:
				pass = (ref_val & read_mask) > (stencil & read_mask);
				break;
			case CompareFunc::LEQUAL:
				pass = (ref_val & read_mask) <= (stencil & read_mask);
				break;
			case CompareFunc::NOT_EQUAL:
				pass = (ref_val & read_mask) != (stencil & read_mask);
				break;
			case CompareFunc::GEQUAL:
				pass = (ref_val & read_mask) > (stencil & read_mask);
				break;
			case CompareFunc::LESS:
				pass = (ref_val & read_mask) < (stencil & read_mask);
				break;
			}
		}
		return pass;
	}

	void GraphicsDevice::update_stencil_buffer(RawBuffer<uint8_t>* stencilbuf, const uint32_t& row, const uint32_t& col, const PerSampleOperation& op_pass, const StencilOp& stencil_pass_op, const StencilOp& stencil_fail_op, const StencilOp& stencil_zfail_op, const uint8_t& ref_val) const
	{
		bool stencil_pass = (op_pass & PerSampleOperation::STENCIL_TEST) != PerSampleOperation::DISABLE;
		bool z_pass = (op_pass & PerSampleOperation::DEPTH_TEST) != PerSampleOperation::DISABLE;
		uint8_t stencil;
		stencilbuf->read(row, col, stencil);
		StencilOp stencil_op;
		if (stencil_pass)
		{
			stencil_op = z_pass ? stencil_pass_op : stencil_zfail_op;
		}
		else
		{
			stencil_op = stencil_fail_op;
		}
		switch (stencil_op)
		{
		case StencilOp::KEEP:
			break;
		case StencilOp::ZERO:
			stencilbuf->write(row, col, 0);
			break;
		case StencilOp::REPLACE:
			stencilbuf->write(row, col, ref_val);
			break;
		case StencilOp::INCR:
			stencilbuf->write(row, col, std::clamp((uint8_t)(stencil + 1), (uint8_t)0, (uint8_t)255));
			break;
		case StencilOp::DECR:
			stencilbuf->write(row, col, std::clamp((uint8_t)(stencil - 1), (uint8_t)0, (uint8_t)255));
			break;
		case StencilOp::INCR_WRAP:
			stencilbuf->write(row, col, stencil + 1);
			break;
		case StencilOp::DECR_WRAP:
			stencilbuf->write(row, col, stencil - 1);
			break;
		case StencilOp::INVERT:
			stencilbuf->write(row, col, ~stencil);
			break;
		}
	}

	bool GraphicsDevice::perform_depth_test(RawBuffer<float>* zbuf, const CompareFunc& func, const uint32_t& row, const uint32_t& col, const float& z) const
	{
		float depth;
		bool pass = false;
		if (zbuf->read(row, col, depth))
		{
			pass = z <= depth;
			switch (func)
			{
			case CompareFunc::NEVER:
				pass = false;
				break;
			case CompareFunc::ALWAYS:
				pass = true;
				break;
			case CompareFunc::EQUAL:
				pass = EQUALS(z, depth); // percision concern
				break;
			case CompareFunc::GREATER:
				pass = z > depth;
				break;
			case CompareFunc::LEQUAL:
				pass = z <= depth;
				break;
			case CompareFunc::NOT_EQUAL:
				pass = z != depth;
				break;
			case CompareFunc::GEQUAL:
				pass = z >= depth;
				break;
			case CompareFunc::LESS:
				pass = z < depth;
				break;
			}
		}
		return pass;
	}

	// todo: alpha factor
	Color GraphicsDevice::blend(const Color& src_color, const Color& dst_color, const BlendFactor& src_factor, const BlendFactor& dst_factor, const BlendOp& op)
	{
		Color lhs, rhs;
		switch (src_factor)
		{
		case BlendFactor::ONE:
			lhs = src_color;
			break;
		case BlendFactor::SRC_ALPHA:
			lhs = src_color * src_color.a;
			break;
		case BlendFactor::SRC_COLOR:
			lhs = src_color * src_color;
			break;
		case BlendFactor::ONE_MINUS_SRC_ALPHA:
			lhs = src_color * (1.0f - src_color);
			break;
		case BlendFactor::ONE_MINUS_SRC_COLOR:
			lhs = src_color * (1.0f - dst_color);
			break;
		case BlendFactor::DST_ALPHA:
			lhs = src_color * dst_color.a;
			break;
		case BlendFactor::DST_COLOR:
			lhs = src_color * dst_color;
			break;
		case BlendFactor::ONE_MINUS_DST_ALPHA:
			lhs = src_color * (1.0f - dst_color.a);
			break;
		case BlendFactor::ONE_MINUS_DST_COLOR:
			lhs = src_color * (1.0f - dst_color);
			break;
		}

		switch (dst_factor)
		{
		case BlendFactor::ONE:
			rhs = src_color;
			break;
		case BlendFactor::SRC_ALPHA:
			rhs = dst_color * src_color.a;
			break;
		case BlendFactor::SRC_COLOR:
			rhs = dst_color * src_color;
			break;
		case BlendFactor::ONE_MINUS_SRC_ALPHA:
			rhs = dst_color * (1.0f - src_color);
			break;
		case BlendFactor::ONE_MINUS_SRC_COLOR:
			rhs = dst_color * (1.0f - dst_color);
			break;
		case BlendFactor::DST_ALPHA:
			rhs = dst_color * dst_color.a;
			break;
		case BlendFactor::DST_COLOR:
			rhs = dst_color * dst_color;
			break;
		case BlendFactor::ONE_MINUS_DST_ALPHA:
			rhs = dst_color * (1.0f - dst_color.a);
			break;
		case BlendFactor::ONE_MINUS_DST_COLOR:
			rhs = dst_color * (1.0f - dst_color);
			break;
		}

		switch (op)
		{
		case BlendOp::ADD:
			return lhs + rhs;
		case BlendOp::SUB:
			return lhs - rhs;
		}
		return lhs + rhs;
	}

	Vertex GraphicsDevice::clip2ndc(const Vertex& v) const
	{
		Vertex ret = v;
		ret.position = clip2ndc(v.position);
		return ret;
	}

	Vector4 GraphicsDevice::clip2ndc(const Vector4& v) const
	{
		Vector4 ndc_pos = v;
		float w = v.w;
		if (w == 0.0f)
		{
			w += EPSILON;
		}
		ndc_pos /= w;
		return ndc_pos;
	}

	Vertex GraphicsDevice::ndc2viewport(const Vertex& v) const
	{
		Vertex ret = v;
		ret.position = ndc2viewport(v.position);
		return ret;
	}

	Vector4 GraphicsDevice::ndc2viewport(const Vector4& v) const
	{
		Vector4 viewport_pos;
		viewport_pos.x = (v.x + 1.0f) * this->width * 0.5f;
		viewport_pos.y = (v.y + 1.0f) * this->height * 0.5f;
		viewport_pos.z = v.z * 0.5f + 0.5f;
		viewport_pos.w = v.w;
		return viewport_pos;
	}

	float GraphicsDevice::linearize_01depth(const float& depth, const float& near, const float& far) const
	{
		return (linearize_depth(depth, near, far) - near) / (far - near);
	}

	float GraphicsDevice::linearize_depth(const float& depth, const float& near, const float& far) const
	{
	#ifdef GL_LIKE
		float ndc_z = depth * 2.0f - 1.0f;  // [0, 1] -> [-1, 1] (GL)
	#else
		float ndc_z = depth; // [0, 1] (DX)
	#endif

	#ifdef LEFT_HANDED 
	#ifdef GL_LIKE
		return (2.0f * near * far) / (far + near - ndc_z * (far - near));
	#else
		return (far * near) / (far - (far - near) * ndc_z);
	#endif
	#else
	#ifdef GL_LIKE
		return (2.0f * near * far) / (-(far + near) - ndc_z * (far - near));
	#else
		return (far * near) / (-far - (far - near) * ndc_z);
	#endif
	#endif
	}

	void GraphicsDevice::draw_segment(const Vector3& start, const Vector3& end, const Color& col, const Matrix4x4& v, const Matrix4x4& p, const Vector2& screen_translation)
	{
		Vector4 clip_start = p * v * Vector4(start);
		Vector4 clip_end = p * v * Vector4(end);

		Vector4 n1 = clip2ndc(clip_start);
		Vector4 n2 = clip2ndc(clip_end);

		Vector4 s1 = ndc2viewport(n1);
		Vector4 s2 = ndc2viewport(n2);

		Matrix4x4 translation = Matrix4x4::translation(Vector3(screen_translation));

		s1 = translation * s1;
		s2 = translation * s2;

		SegmentDrawer::bresenham(framebuffer.get(), (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, Color::encode_rgba(col));
	}

	void GraphicsDevice::draw_screen_segment(const Vector4& start, const Vector4& end, const Color& col)
	{
		SegmentDrawer::bresenham(framebuffer.get(), (int)start.x, (int)start.y, (int)end.x, (int)end.y, Color::encode_rgba(col));
	}

	void GraphicsDevice::draw_segment(const Vector3& start, const Vector3& end, const Color& col, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p)
	{
		Vector4 clip_start = p * v * m * Vector4(start);
		Vector4 clip_end = p * v * m * Vector4(end);
		draw_segment(clip_start, clip_end, col);
	}

	void GraphicsDevice::draw_segment(const Vector3& start, const Vector3& end, const Color& col, const Matrix4x4& v, const Matrix4x4& p)
	{
		Vector4 clip_start = p * v * Vector4(start);
		Vector4 clip_end = p * v * Vector4(end);
		draw_segment(clip_start, clip_end, col);
	}

	void GraphicsDevice::draw_segment(const Vector4& clip_start, const Vector4& clip_end, const Color& col)
	{
		Vector4 n1 = clip2ndc(clip_start);
		Vector4 n2 = clip2ndc(clip_end);

		Vector4 s1 = ndc2viewport(n1);
		Vector4 s2 = ndc2viewport(n2);

		SegmentDrawer::bresenham(framebuffer.get(), (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, Color::encode_rgba(col));
	}

	void GraphicsDevice::draw_coordinates(const Vector3& pos, const Vector3& forward, const Vector3& up, const Vector3& right, const Matrix4x4& v, const Matrix4x4& p, const Vector2& offset)
	{
		this->draw_segment(pos, pos + forward, Color::BLUE, v, p, offset);
		this->draw_segment(pos, pos + right, Color::RED, v, p, offset);
		this->draw_segment(pos, pos + up, Color::GREEN, v, p, offset);
	}

	void GraphicsDevice::draw_coordinates(const Vector3& pos, const Vector3& forward, const Vector3& up, const Vector3& right, const Matrix4x4& v, const Matrix4x4& p)
	{
		this->draw_segment(pos, pos + forward, Color::BLUE, v, p);
		this->draw_segment(pos, pos + right, Color::RED, v, p);
		this->draw_segment(pos, pos + up, Color::GREEN, v, p);
	}

	void GraphicsDevice::draw_coordinates(const Vector3& pos, const Vector3& forward, const Vector3& up, const Vector3& right, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p)
	{
		this->draw_segment(pos, pos + forward, Color::BLUE, m, v, p);
		this->draw_segment(pos, pos + right, Color::RED, m, v, p);
		this->draw_segment(pos, pos + up, Color::GREEN, m, v, p);
	}
}
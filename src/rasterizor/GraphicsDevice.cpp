#include "GraphicsDevice.hpp"
#include <iostream>
#include "Singleton.hpp"
#include "GlobalShaderParams.hpp"
#include "tinymath/primitives/Rect.h"
#include "Clipper.hpp"
#include "SegmentDrawer.hpp"
#include <execution>
#include <algorithm>
#include "Logger.hpp"
#include "Sampling.hpp"

namespace Guarneri
{
	constexpr float kFarZ = 1.0f;
	constexpr uint8_t kDefaultStencil = 0x00;
	constexpr uint32_t kTileSize = 16;

	GraphicsDevice::GraphicsDevice()
	{
		width = 0;
		height = 0;
		row_tile_count = 0;
		col_tile_count = 0;
		tile_length = 0;
		msaa_subsample_count = 0;
		subsamples_per_axis = 0;
		statistics.culled_backface_triangle_count = 0;
		statistics.culled_triangle_count = 0;
		statistics.earlyz_optimized = 0;
		statistics.triangle_count = 0;
		tiles = nullptr;
		multi_thread = true;
		tile_based = true;
	}

	GraphicsDevice::~GraphicsDevice()
	{}

	void GraphicsDevice::resize(uint32_t w, uint32_t h)
	{
		initialize(w, h);
	}

	void GraphicsDevice::initialize(uint32_t w, uint32_t h)
	{
		this->width = w;
		this->height = h;

		// prepare tiles
		int row_rest = h % kTileSize;
		int col_rest = w % kTileSize;
		row_tile_count = h / kTileSize + (row_rest > 0 ? 1 : 0);
		col_tile_count = w / kTileSize + (col_rest > 0 ? 1 : 0);
		tile_length = (int)((long)row_tile_count * (long)col_tile_count);
		tiles = new FrameTile[tile_length];
		FrameTile::build_tiles(tiles, kTileSize, row_tile_count, col_tile_count, row_rest, col_rest);

		// prepare buffers
		zbuffer = std::make_unique<RawBuffer<float>>(w, h);
		shadowmap = std::make_unique<RawBuffer<float>>(w, h);
		framebuffer = std::make_unique<RawBuffer<tinymath::color_rgba>>(w, h);

		stencilbuffer = std::make_unique<RawBuffer<uint8_t>>(w, h);
		zbuffer->clear(kFarZ);
		shadowmap->clear(kFarZ);
		stencilbuffer->clear(kDefaultStencil);
		framebuffer->clear(ColorEncoding::encode_rgba(tinymath::kColorBlack));
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
		return { kTileSize, row_tile_count, col_tile_count, tile_length };
	}

	void GraphicsDevice::draw(DrawCommand task)
	{
		const Shader& shader = *task.shader;
		auto v1 = task.v1;
		auto v2 = task.v2;
		auto v3 = task.v3;
		auto m = task.m;
		auto v = task.v;
		auto p = task.p;

		input2raster(shader, v1, v2, v3);
	}

	void GraphicsDevice::submit_draw_command(Shader* shader, const Vertex& v1, const Vertex& v2, const Vertex& v3, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p)
	{
		DrawCommand task = { shader, v1, v2, v3, m, v, p };
		draw_commands.emplace_back(task);
	}

	void GraphicsDevice::fence_draw_commands()
	{
		std::for_each(
			std::execution::par_unseq,
			draw_commands.begin(),
			draw_commands.end(),
			[this](auto&& cmd)
			{
				this->draw(cmd);
			});
		draw_commands.clear();
	}

	void GraphicsDevice::present()
	{
		std::for_each(
			std::execution::par_unseq,
			tiles,
			tiles + tile_length,
			[this](auto&& tile)
			{
				this->rasterize_tile(tile);
			});

		if (INST(GlobalShaderParams).enable_msaa)
		{
			std::for_each(
				std::execution::par_unseq,
				tiles,
				tiles + tile_length,
				[this](auto&& tile)
				{
					this->resolve_tile(tile);
				});
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
					msaa_colorbuffer = std::make_unique<RawBuffer<tinymath::color_rgba>>(w * subsamples_per_axis, h * subsamples_per_axis);
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
		process_commands();

		if ((flag & BufferFlag::COLOR) != BufferFlag::NONE)
		{
			if ((INST(GlobalShaderParams).render_flag & RenderFlag::DEPTH) != RenderFlag::DISABLE || (INST(GlobalShaderParams).render_flag & RenderFlag::SHADOWMAP) != RenderFlag::DISABLE)
			{
				framebuffer->clear(ColorEncoding::encode_rgba(tinymath::kColorWhite));
			}
			else
			{
				framebuffer->clear(ColorEncoding::encode_rgba(tinymath::kColorBlack));
			}
		}

		shadowmap->clear(kFarZ);

		if ((flag & BufferFlag::DEPTH) != BufferFlag::NONE) { zbuffer->clear(kFarZ); }
		if ((flag & BufferFlag::STENCIL) != BufferFlag::NONE) { stencilbuffer->clear(kDefaultStencil); }

		if (INST(GlobalShaderParams).enable_msaa)
		{
			msaa_colorbuffer->clear(ColorEncoding::encode_rgba(tinymath::kColorBlack));
			msaa_zbuffer->clear(kFarZ);
			msaa_coveragebuffer->clear(0);
			msaa_stencilbuffer->clear(kDefaultStencil);
		}

		statistics.culled_triangle_count = 0;
		statistics.culled_backface_triangle_count = 0;
		statistics.triangle_count = 0;
		statistics.earlyz_optimized = 0;
	}

	void GraphicsDevice::input2raster(const Shader& shader, const Vertex& v1, const Vertex& v2, const Vertex& v3)
	{
		// vertex stage
		v2f o1 = process_vertex(shader, v1);
		v2f o2 = process_vertex(shader, v2);
		v2f o3 = process_vertex(shader, v3);
		Vertex c1(o1.position, o1.world_pos, o1.shadow_coord, o1.color, o1.normal, o1.uv, o1.tangent, o1.bitangent);
		Vertex c2(o2.position, o2.world_pos, o2.shadow_coord, o2.color, o2.normal, o2.uv, o2.tangent, o2.bitangent);
		Vertex c3(o3.position, o3.world_pos, o3.shadow_coord, o3.color, o3.normal, o3.uv, o3.tangent, o3.bitangent);

		if (Clipper::inside_cvv(c1.position, c2.position, c3.position))
		{
			// all in cvv, rasterize directly
			clip2raster(shader, c1, c2, c3);
			statistics.triangle_count++;
		}
		else
		{
			// clip in homogenous space
			auto triangles = Clipper::cvv_clipping(INST(GlobalShaderParams).cam_near, tinymath::Frustum::homogenous_volume(), c1, c2, c3);

			if (triangles.size() == 0) { statistics.culled_triangle_count++; return; }

			for (size_t idx = 0; idx < triangles.size(); idx++)
			{
				Vertex clip1 = triangles[idx][0]; Vertex clip2 = triangles[idx][1]; Vertex clip3 = triangles[idx][2];
				clip2raster(shader, clip1, clip2, clip3);
			}

			statistics.triangle_count += (uint32_t)triangles.size();
		}
	}

	void GraphicsDevice::clip2raster(const Shader& shader, const Vertex& c1, const Vertex& c2, const Vertex& c3)
	{
		// clip space to ndc (perspective division)
		Vertex ndc1 = Vertex::clip2ndc(c1);
		Vertex ndc2 = Vertex::clip2ndc(c2);
		Vertex ndc3 = Vertex::clip2ndc(c3);

		// backface culling
		bool double_face = shader.double_face;
		bool enable_backface_culling = (INST(GlobalShaderParams).culling_clipping_flag & CullingAndClippingFlag::BACK_FACE_CULLING) != CullingAndClippingFlag::DISABLE;
		if (!double_face && enable_backface_culling && !shader.skybox)
		{
			if (Clipper::backface_culling_ndc(ndc1.position.xyz, ndc2.position.xyz, ndc3.position.xyz)) { statistics.culled_backface_triangle_count++; return; }
		}

		// ndc to screen space
		Vertex s1 = Vertex::ndc2screen(this->width, this->height, ndc1);
		Vertex s2 = Vertex::ndc2screen(this->width, this->height, ndc2);
		Vertex s3 = Vertex::ndc2screen(this->width, this->height, ndc3);

		// triangle assembly
		std::vector<Triangle> assembled_triangles = Triangle(s1, s2, s3).horizontally_split();
		for (auto triangle = assembled_triangles.begin(); triangle != assembled_triangles.end(); triangle++)
		{
			if (this->tile_based)
			{
				// push tile based draw task
				FrameTile::push_draw_task(tiles, row_tile_count, col_tile_count, *triangle, shader, this->width, this->height, kTileSize);
			}
			else
			{
				// rasterize triangle directly
				rasterize(*triangle, shader, RasterizerStrategy::SCANLINE);
			}
		}
	}

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
				rasterize(tile, task.triangle, *task.shader);

				// wireframe
				if ((INST(GlobalShaderParams).render_flag & RenderFlag::WIREFRAME) != RenderFlag::DISABLE)
				{
					draw_screen_segment(task.triangle[0].position, task.triangle[1].position, tinymath::Color(0.5f, 0.5f, 1.0f, 1.0f));
					draw_screen_segment(task.triangle[0].position, task.triangle[2].position, tinymath::Color(0.5f, 0.5f, 1.0f, 1.0f));
					draw_screen_segment(task.triangle[2].position, task.triangle[1].position, tinymath::Color(0.5f, 0.5f, 1.0f, 1.0f));
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
					tinymath::color_rgba dst;
					if (framebuffer->read(row, col, dst))
					{
						tinymath::Color dst_color = ColorEncoding::decode(dst);
						tinymath::Color src_color = tinymath::Color(r, g, 0.0f, 0.5f);
						tinymath::Color blended_color = blend(src_color, dst_color, BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendOp::ADD);
						auto pixel_color = ColorEncoding::encode_rgba(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
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
				tinymath::Color pixel_color = {0.0f, 0.0f, 0.0f, 1.0f};
				for (int x_subsample_idx = 0; x_subsample_idx < subsamples_per_axis; x_subsample_idx++)
				{
					for (int y_subsample_idx = 0; y_subsample_idx < subsamples_per_axis; y_subsample_idx++)
					{
						uint32_t sub_row = (uint32_t)(row * subsamples_per_axis + x_subsample_idx);
						uint32_t sub_col = (uint32_t)(col * subsamples_per_axis + y_subsample_idx);
						uint8_t coverage = 0;
						if (msaa_coveragebuffer->read(sub_row, sub_col, coverage) && coverage > 0)
						{
							coverage_count++;
							tinymath::color_rgba msaa_color;
							msaa_colorbuffer->read(sub_row, sub_col, msaa_color);
							pixel_color += ColorEncoding::decode(msaa_color);
						}
					}
				}

				if (coverage_count > 0)
				{
					pixel_color *= 1.0f / msaa_subsample_count;
					pixel_color.a = 1.0f;
					framebuffer->write(row, col, ColorEncoding::encode_rgba(pixel_color));
				}
			}
		}
	}

	void GraphicsDevice::rasterize(const FrameTile& tile, const Triangle& tri, const Shader& shader)
	{
		auto bounds = tinymath::Rect(tri[0].position.xy, tri[1].position.xy, tri[2].position.xy);
		int row_start = (int)(bounds.min().y) - 1;
		int row_end = (int)(bounds.max().y) + 1;
		int col_start = (int)(bounds.min().x) - 1;
		int col_end = (int)(bounds.max().x) + 1;

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

		auto p0 = v0.position.xy;
		auto p1 = v1.position.xy;
		auto p2 = v2.position.xy;

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
					tinymath::vec2f pixel((float)col + 0.5f, (float)row + 0.5f);
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
			draw_screen_segment(tri[0].position, tri[1].position, tinymath::Color(1.0f, 1.0f, 1.0f, 1.0f));
			draw_screen_segment(tri[0].position, tri[2].position, tinymath::Color(1.0f, 1.0f, 1.0f, 1.0f));
			draw_screen_segment(tri[2].position, tri[1].position, tinymath::Color(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	void GraphicsDevice::scanblock(const Triangle& tri, const Shader& shader)
	{
		auto bounds = tinymath::Rect(tri[0].position.xy, tri[1].position.xy, tri[2].position.xy);
		int row_start = (int)(bounds.min().y) - 1;
		int row_end = (int)(bounds.max().y) + 1;
		int col_start = (int)(bounds.min().x) - 1;
		int col_end = (int)(bounds.max().x) + 1;

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

		auto p0 = v0.position.xy;
		auto p1 = v1.position.xy;
		auto p2 = v2.position.xy;

		float area = Triangle::area_double(p0, p1, p2);

		for (uint32_t row = row_start; row < (uint32_t)row_end; row++)
		{
			for (uint32_t col = col_start; col < (uint32_t)col_end; col++)
			{
				tinymath::vec2f pixel((float)col + 0.5f, (float)row + 0.5f);
				float w0 = Triangle::area_double(p1, p2, pixel);
				float w1 = Triangle::area_double(p2, p0, pixel);
				float w2 = Triangle::area_double(p0, p1, pixel);
				if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f)
				{
					w0 /= area; w1 /= area; w2 /= area;
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

	void GraphicsDevice::process_subsamples(RawBuffer<tinymath::color_rgba>* fbuf, RawBuffer<float>* zbuf, RawBuffer<uint8_t>* stencilbuf, const uint32_t& row, const uint32_t& col, const Shader& shader, const Vertex& v0, const Vertex& v1, const Vertex& v2, const float& area)
	{
		auto p0 = v0.position.xy;
		auto p1 = v1.position.xy;
		auto p2 = v2.position.xy;

		tinymath::vec2f pixel((float)col + 0.5f, (float)row + 0.5f);
		bool color_calculated = false;

		uint8_t total = msaa_subsample_count;

		for (int x_subsample_idx = 0; x_subsample_idx < subsamples_per_axis; x_subsample_idx++)
		{
			for (int y_subsample_idx = 0; y_subsample_idx < subsamples_per_axis; y_subsample_idx++)
			{
				int i = x_subsample_idx * subsamples_per_axis + y_subsample_idx;
				auto vec = hammersley(i, total);
				tinymath::vec2f subsample((float)col + vec.x, (float)row + vec.y);
				float w0 = Triangle::area_double(p1, p2, subsample);
				float w1 = Triangle::area_double(p2, p0, subsample);
				float w2 = Triangle::area_double(p0, p1, subsample);
				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
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


					tinymath::color_rgba pixel_color; 
					tinymath::Color fragment_result;
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
						v_out.position = v.position * w;
						v_out.world_pos = v.world_pos * w;
						v_out.shadow_coord = v.shadow_coord * w;
						v_out.color = v.color * w;
						v_out.normal = v.normal * w;
						v_out.uv = v.uv * w;
						v_out.tangent = v.tangent * w;
						v_out.bitangent = v.bitangent * w;
						fragment_result = shader.fragment_shader(v_out);
						pixel_color = ColorEncoding::encode_rgba(fragment_result);
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
						tinymath::color_rgba dst;
						if (fbuf->read(subsample_row, subsample_col, dst))
						{
							tinymath::Color dst_color = ColorEncoding::decode(dst);
							tinymath::Color src_color = fragment_result;
							tinymath::Color blended_color = blend(src_color, dst_color, src_factor, dst_factor, blend_op);
							pixel_color = ColorEncoding::encode_rgba(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
						}
					}

					if (validate_fragment(op_pass))
					{
						fbuf->write(subsample_row, subsample_col, pixel_color);
					}
				}
			}
		}
	}

	void GraphicsDevice::process_fragment(RawBuffer<tinymath::color_rgba>* fbuf, RawBuffer<float>* zbuf, RawBuffer<uint8_t>* stencilbuf, const Vertex& v, const uint32_t& row, const uint32_t& col, const Shader& shader)
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

		tinymath::color_rgba pixel_color;

		// fragment shader
		tinymath::Color fragment_result;
		v2f v_out;
		float w = 1.0f / v.rhw;
		v_out.position = v.position * w;
		v_out.world_pos = v.world_pos * w;
		v_out.shadow_coord = v.shadow_coord * w;
		v_out.color = v.color * w;
		v_out.normal = v.normal * w;
		v_out.uv = v.uv * w;
		v_out.tangent = v.tangent * w;
		v_out.bitangent = v.bitangent * w;

		// todo: ddx ddy
		fragment_result = shader.fragment_shader(v_out);
		pixel_color = ColorEncoding::encode_rgba(fragment_result);

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
			tinymath::color_rgba dst;
			if (fbuf->read(row, col, dst))
			{
				tinymath::Color dst_color = ColorEncoding::decode(dst);
				tinymath::Color src_color = fragment_result;
				tinymath::Color blended_color = blend(src_color, dst_color, src_factor, dst_factor, blend_op);
				pixel_color = ColorEncoding::encode_rgba(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
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
				tinymath::color_rgba cur;
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
				tinymath::color_rgba c = ColorEncoding::encode_rgba(stencil, stencil, stencil, 255);
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
				tinymath::Color depth_color = tinymath::kColorWhite * linear_depth;
				tinymath::color_rgba c = ColorEncoding::encode_rgba(depth_color);
				fbuf->write(row, col, c);
			}
		}

		// shadowmap visualization
		if ((INST(GlobalShaderParams).render_flag & RenderFlag::SHADOWMAP) != RenderFlag::DISABLE)
		{
			float cur_depth;
			if (this->get_shadowmap()->read(row, col, cur_depth))
			{
				tinymath::Color depth_color = tinymath::kColorWhite * cur_depth;
				tinymath::color_rgba c = ColorEncoding::encode_rgba(depth_color);
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
				pass = tinymath::approx(z, depth);
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
	tinymath::Color GraphicsDevice::blend(const tinymath::Color& src_color, const tinymath::Color& dst_color, const BlendFactor& src_factor, const BlendFactor& dst_factor, const BlendOp& op)
	{
		tinymath::Color lhs, rhs;
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

	void GraphicsDevice::draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& v, const tinymath::mat4x4& p, const tinymath::vec2f& screen_translation)
	{
		tinymath::vec4f clip_start = p * v * tinymath::vec4f(start);
		tinymath::vec4f clip_end = p * v * tinymath::vec4f(end);

		tinymath::vec4f n1 = Vertex::clip2ndc(clip_start);
		tinymath::vec4f n2 = Vertex::clip2ndc(clip_end);

		tinymath::vec4f s1 = Vertex::ndc2screen(this->width, this->height, n1);
		tinymath::vec4f s2 = Vertex::ndc2screen(this->width, this->height, n2);

		tinymath::mat4x4 translation = tinymath::translation(tinymath::vec3f(screen_translation));

		s1 = translation * s1;
		s2 = translation * s2;

		SegmentDrawer::bresenham(framebuffer.get(), (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, ColorEncoding::encode_rgba(col));
	}

	void GraphicsDevice::draw_screen_segment(const tinymath::vec4f& start, const tinymath::vec4f& end, const tinymath::Color& col)
	{
		SegmentDrawer::bresenham(framebuffer.get(), (int)start.x, (int)start.y, (int)end.x, (int)end.y, ColorEncoding::encode_rgba(col));
	}

	void GraphicsDevice::draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p)
	{
		tinymath::vec4f clip_start = p * v * m * tinymath::vec4f(start);
		tinymath::vec4f clip_end = p * v * m * tinymath::vec4f(end);
		draw_segment(clip_start, clip_end, col);
	}

	void GraphicsDevice::draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& v, const tinymath::mat4x4& p)
	{
		tinymath::vec4f clip_start = p * v * tinymath::vec4f(start);
		tinymath::vec4f clip_end = p * v * tinymath::vec4f(end);
		draw_segment(clip_start, clip_end, col);
	}

	void GraphicsDevice::draw_segment(const tinymath::vec4f& clip_start, const tinymath::vec4f& clip_end, const tinymath::Color& col)
	{
		tinymath::vec4f n1 = Vertex::clip2ndc(clip_start);
		tinymath::vec4f n2 = Vertex::clip2ndc(clip_end);

		tinymath::vec4f s1 = Vertex::ndc2screen(this->width, this->height, n1);
		tinymath::vec4f s2 = Vertex::ndc2screen(this->width, this->height, n2);

		SegmentDrawer::bresenham(framebuffer.get(), (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, ColorEncoding::encode_rgba(col));
	}

	void GraphicsDevice::draw_coordinates(const tinymath::vec3f& pos, const tinymath::vec3f& forward, const tinymath::vec3f& up, const tinymath::vec3f& right, const tinymath::mat4x4& v, const tinymath::mat4x4& p, const tinymath::vec2f& offset)
	{
		this->draw_segment(pos, pos + forward, tinymath::kColorBlue, v, p, offset);
		this->draw_segment(pos, pos + right, tinymath::kColorRed, v, p, offset);
		this->draw_segment(pos, pos + up, tinymath::kColorGreen, v, p, offset);
	}

	void GraphicsDevice::draw_coordinates(const tinymath::vec3f& pos, const tinymath::vec3f& forward, const tinymath::vec3f& up, const tinymath::vec3f& right, const tinymath::mat4x4& v, const tinymath::mat4x4& p)
	{
		this->draw_segment(pos, pos + forward, tinymath::kColorBlue, v, p);
		this->draw_segment(pos, pos + right, tinymath::kColorRed, v, p);
		this->draw_segment(pos, pos + up, tinymath::kColorGreen, v, p);
	}

	void GraphicsDevice::draw_coordinates(const tinymath::vec3f& pos, const tinymath::vec3f& forward, const tinymath::vec3f& up, const tinymath::vec3f& right, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p)
	{
		this->draw_segment(pos, pos + forward, tinymath::kColorBlue, m, v, p);
		this->draw_segment(pos, pos + right, tinymath::kColorRed, m, v, p);
		this->draw_segment(pos, pos + up, tinymath::kColorGreen, m, v, p);
	}
}
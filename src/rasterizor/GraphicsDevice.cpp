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
#include "IdAllocator.hpp"

namespace Guarneri
{
	constexpr size_t kTileSize = 16;
	static IdAllocator buffer_id_allocator(kInvalidID + 1, UINT_MAX);

	GraphicsDevice::GraphicsDevice()
	{
		active_frame_buffer_id = kInvalidID;
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

	void GraphicsDevice::resize(size_t w, size_t h)
	{
		resize_tiles(w, h);

		if (frame_buffer == nullptr)
		{
			frame_buffer = std::make_unique<FrameBuffer>(w, h, FrameContent::Color | FrameContent::Depth | FrameContent::Stencil);
		}
		else
		{
			frame_buffer->resize(w, h);
		}

		if (msaa_buffer != nullptr)
		{
			msaa_buffer->resize(w * subsamples_per_axis, h * subsamples_per_axis);
		}
	}

	void GraphicsDevice::resize_tiles(size_t w, size_t h)
	{
		if (tiles != nullptr)
		{
			delete[] tiles;
		}

		size_t row_rest = h % kTileSize;
		size_t col_rest = w % kTileSize;
		row_tile_count = h / kTileSize + (row_rest > 0 ? 1 : 0);
		col_tile_count = w / kTileSize + (col_rest > 0 ? 1 : 0);
		tile_length = row_tile_count * col_tile_count;
		tiles = new FrameTile[tile_length];
		FrameTile::build_tiles(tiles, kTileSize, row_tile_count, col_tile_count, row_rest, col_rest);
	}

	void GraphicsDevice::initialize(size_t w, size_t h)
	{
		resize(w, h);
	}

	void GraphicsDevice::set_subsample_count(const uint8_t& count)
	{
		MsaaCommand* cmd = new MsaaCommand();
		cmd->enable = count > 0;
		cmd->msaa_subsample_count = count;
		commands.push(cmd);
	}

	FrameBuffer* GraphicsDevice::get_active_framebuffer() const noexcept
	{
		if (kInvalidID == active_frame_buffer_id)
		{
			return frame_buffer.get();
		}

		if (frame_buffer_map.count(active_frame_buffer_id) > 0)
		{
			return frame_buffer_map.at(active_frame_buffer_id).get();
		}

		return frame_buffer.get();
	}

	uint32_t GraphicsDevice::create_buffer(const size_t& w, const size_t& h, const FrameContent& content) noexcept
	{
		auto buffer = std::make_shared<FrameBuffer>(w, h, content);

		uint32_t id;
		if (buffer_id_allocator.alloc(id))
		{
			frame_buffer_map.insert(std::make_pair(id, buffer));
			return id;
		}

		return kInvalidID;
	}

	void GraphicsDevice::set_active_frame_buffer(uint32_t& id)
	{
		if (frame_buffer_map.count(id) > 0)
		{
			auto fb = frame_buffer_map[id];
			size_t current_w, current_h;
			frame_buffer->get_size(current_w, current_h);
			if (current_w != fb->get_width() || current_h != fb->get_height())
			{
				resize_tiles(fb->get_width(), fb->get_height());
			}
			active_frame_buffer_id = id;
		}
	}

	void GraphicsDevice::reset_active_frame_buffer() noexcept
	{
		if (frame_buffer_map.count(active_frame_buffer_id) > 0)
		{
			auto fb = frame_buffer_map[active_frame_buffer_id];
			size_t current_w, current_h;
			fb->get_size(current_w, current_h);
			if (current_w != frame_buffer->get_width() || current_h != frame_buffer->get_height())
			{
				resize_tiles(frame_buffer->get_width(), frame_buffer->get_height());
			}
		}

		active_frame_buffer_id = kInvalidID;
	}

	bool GraphicsDevice::get_buffer(const uint32_t& id, std::shared_ptr<FrameBuffer>& buffer) const noexcept
	{
		buffer = nullptr;
		if (frame_buffer_map.count(id) > 0)
		{
			buffer = frame_buffer_map.at(id);
			return true;
		}

		return false;
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
					size_t w, h;
					get_active_framebuffer()->get_size(w, h);
					msaa_subsample_count = count;
					subsamples_per_axis = (uint8_t)(std::sqrt((double)count));
					msaa_buffer = std::make_unique<FrameBuffer>(w * subsamples_per_axis, h * subsamples_per_axis, FrameContent::Color | FrameContent::Depth | FrameContent::Stencil | FrameContent::Coverage);
				}
				delete msaa;
			}
		}
	}

	void GraphicsDevice::clear_buffer(const FrameContent& flag)
	{
		process_commands();

		get_active_framebuffer()->clear(flag);

		for (auto& kv : frame_buffer_map)
		{
			kv.second->clear(flag);
		}

		if (INST(GlobalShaderParams).enable_msaa)
		{
			msaa_buffer->clear(flag);
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

			statistics.triangle_count += triangles.size();
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

		size_t w, h;
		get_active_framebuffer()->get_size(w, h);

		// ndc to screen space
		Vertex s1 = Vertex::ndc2screen(w, h, ndc1);
		Vertex s2 = Vertex::ndc2screen(w, h, ndc2);
		Vertex s3 = Vertex::ndc2screen(w, h, ndc3);

		// triangle assembly
		std::vector<Triangle> assembled_triangles = Triangle(s1, s2, s3).horizontally_split();
		for (auto triangle = assembled_triangles.begin(); triangle != assembled_triangles.end(); triangle++)
		{
			if (this->tile_based)
			{
				// push tile based draw task
				FrameTile::push_draw_task(tiles, row_tile_count, col_tile_count, *triangle, shader, w, h, kTileSize);
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
				if ((INST(GlobalShaderParams).debug_flag & RenderFlag::WIREFRAME) != RenderFlag::DISABLE)
				{
					draw_screen_segment(task.triangle[0].position, task.triangle[1].position, tinymath::Color(0.5f, 0.5f, 1.0f, 1.0f));
					draw_screen_segment(task.triangle[0].position, task.triangle[2].position, tinymath::Color(0.5f, 0.5f, 1.0f, 1.0f));
					draw_screen_segment(task.triangle[2].position, task.triangle[1].position, tinymath::Color(0.5f, 0.5f, 1.0f, 1.0f));
				}
			}
		}

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::FRAME_TILE) != RenderFlag::DISABLE)
		{
			for (size_t row = (size_t)tile.row_start; row < (size_t)tile.row_end; row++)
			{
				for (size_t col = (size_t)tile.col_start; col < (size_t)tile.col_end; col++)
				{
					float r = (float)row / tile.row_end;
					float g = (float)col / tile.col_end;
					tinymath::color_rgba dst;
					if (get_active_framebuffer()->read_color(row, col, dst))
					{
						tinymath::Color dst_color = ColorEncoding::decode(dst);
						tinymath::Color src_color = tinymath::Color(r, g, 0.0f, 0.5f);
						tinymath::Color blended_color = blend(src_color, dst_color, BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendOp::ADD);
						auto pixel_color = ColorEncoding::encode_rgba(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
						get_active_framebuffer()->write_color(row, col, pixel_color);
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
		for (size_t row = (size_t)tile.row_start; row < (size_t)tile.row_end; row++)
		{
			for (size_t col = (size_t)tile.col_start; col < (size_t)tile.col_end; col++)
			{
				uint8_t coverage_count = 0;
				tinymath::Color pixel_color = {0.0f, 0.0f, 0.0f, 1.0f};
				for (int x_subsample_idx = 0; x_subsample_idx < subsamples_per_axis; x_subsample_idx++)
				{
					for (int y_subsample_idx = 0; y_subsample_idx < subsamples_per_axis; y_subsample_idx++)
					{
						size_t sub_row = (size_t)(row * subsamples_per_axis + x_subsample_idx);
						size_t sub_col = (size_t)(col * subsamples_per_axis + y_subsample_idx);
						uint8_t coverage = 0;
						if (msaa_buffer->read_coverage(sub_row, sub_col, coverage) && coverage > 0)
						{
							coverage_count++;
							tinymath::color_rgba msaa_color;
							msaa_buffer->read_color(sub_row, sub_col, msaa_color);
							pixel_color += ColorEncoding::decode(msaa_color);
						}
					}
				}

				if (coverage_count > 0)
				{
					pixel_color *= 1.0f / msaa_subsample_count;
					pixel_color.a = 1.0f;
					frame_buffer->write_color(row, col, ColorEncoding::encode_rgba(pixel_color));
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

		row_start = std::clamp(row_start, (int)tile.row_start, (int)tile.row_end);
		row_end = std::clamp(row_end, (int)tile.row_start, (int)tile.row_end);
		col_start = std::clamp(col_start, (int)tile.col_start, (int)tile.col_end);
		col_end = std::clamp(col_end, (int)tile.col_start, (int)tile.col_end);

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
			for (size_t row = (size_t)row_start; row < (size_t)row_end; row++)
			{
				for (size_t col = (size_t)col_start; col < (size_t)col_end; col++)
				{
					process_subsamples(*msaa_buffer, row, col, shader, v0, v1, v2, area);
				}
			}
		}
		else
		{
			for (size_t row = (size_t)row_start; row < (size_t)row_end; row++)
			{
				for (size_t col = (size_t)col_start; col < (size_t)col_end; col++)
				{
					tinymath::vec2f pixel((float)col + 0.5f, (float)row + 0.5f);
					float w0 = Triangle::area_double(p1, p2, pixel);
					float w1 = Triangle::area_double(p2, p0, pixel);
					float w2 = Triangle::area_double(p0, p1, pixel);
					if (w0 >= 0 && w1 >= 0 && w2 >= 0)
					{
						w0 /= area; w1 /= area; w2 /= area;
						Vertex vert = Vertex::barycentric_interpolate(v0, v1, v2, w0, w1, w2);
						process_fragment(*get_active_framebuffer(), vert, row, col, shader);
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
		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::WIREFRAME) != RenderFlag::DISABLE)
		{
			draw_screen_segment(tri[0].position, tri[1].position, tinymath::Color(1.0f, 1.0f, 1.0f, 1.0f));
			draw_screen_segment(tri[0].position, tri[2].position, tinymath::Color(1.0f, 1.0f, 1.0f, 1.0f));
			draw_screen_segment(tri[2].position, tri[1].position, tinymath::Color(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	void GraphicsDevice::scanblock(const Triangle& tri, const Shader& shader)
	{
		size_t w, h;
		get_active_framebuffer()->get_size(w, h);

		auto bounds = tinymath::Rect(tri[0].position.xy, tri[1].position.xy, tri[2].position.xy);
		int row_start = (int)(bounds.min().y) - 1;
		int row_end = (int)(bounds.max().y) + 1;
		int col_start = (int)(bounds.min().x) - 1;
		int col_end = (int)(bounds.max().x) + 1;

		row_start = std::clamp(row_start, 0, (int)h);
		row_end = std::clamp(row_end, 0, (int)h);
		col_start = std::clamp(col_start, 0, (int)w);
		col_end = std::clamp(col_end, 0, (int)w);

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

		for (size_t row = row_start; row < (size_t)row_end; row++)
		{
			for (size_t col = col_start; col < (size_t)col_end; col++)
			{
				tinymath::vec2f pixel((float)col + 0.5f, (float)row + 0.5f);
				float w0 = Triangle::area_double(p1, p2, pixel);
				float w1 = Triangle::area_double(p2, p0, pixel);
				float w2 = Triangle::area_double(p0, p1, pixel);
				if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f)
				{
					w0 /= area; w1 /= area; w2 /= area;
					Vertex vert = Vertex::barycentric_interpolate(tri[ccw_idx0], tri[ccw_idx1], tri[ccw_idx2], w0, w1, w2);
					process_fragment(*get_active_framebuffer(), vert, row, col, shader);
				}
			}
		}
	}

	void GraphicsDevice::scanline(const Triangle& tri, const Shader& shader)
	{
		size_t w, h;
		get_active_framebuffer()->get_size(w, h);

		bool flip = tri.flip;
		int top_idx = flip ? 2 : 0;
		int bottom_idx = flip ? 0 : 2;
		int top = (int)(tri[top_idx].position.y + 0.5f);
		int bottom = (int)(tri[bottom_idx].position.y + 0.5f);
		top = std::clamp(top, 0, (int)h);
		bottom = std::clamp(bottom, 0, (int)h);
		assert(bottom >= top);

		for (size_t row = (size_t)top; row < (size_t)bottom; row++)
		{
			Vertex lhs, rhs;
			tri.interpolate((float)row + 0.5f, lhs, rhs);

			// screen space clipping
			bool enable_screen_clipping = (INST(GlobalShaderParams).culling_clipping_flag & CullingAndClippingFlag::SCREEN_CLIPPING) != CullingAndClippingFlag::DISABLE;
			if (enable_screen_clipping)
			{
				Clipper::screen_clipping(lhs, rhs,w);
			}

			int left = (int)(lhs.position.x + 0.5f);
			left = std::clamp(left, 0, (int)w);
			int right = (int)(rhs.position.x + 0.5f);
			right = std::clamp(right, 0, (int)w);
			assert(right >= left);

			for (size_t col = (size_t)left; col < (size_t)right; col++)
			{
				process_fragment(*get_active_framebuffer(), lhs, row, col, shader);
				auto dx = Vertex::differential(lhs, rhs);
				lhs = Vertex::intagral(lhs, dx);
			}
		}
	}

	void GraphicsDevice::process_subsamples(FrameBuffer& buffer, const size_t& row, const size_t& col, const Shader& shader, const Vertex& v0, const Vertex& v1, const Vertex& v2, const float& area)
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
					size_t subsample_row = (size_t)(row * subsamples_per_axis + x_subsample_idx);
					size_t subsample_col = (size_t)(col * subsamples_per_axis + y_subsample_idx);

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

					buffer.write_coverage(subsample_row, subsample_col, 1);


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
						if (!buffer.perform_stencil_test(stencil_ref_val, stencil_read_mask, stencil_func, subsample_row, subsample_col))
						{
							op_pass &= ~PerSampleOperation::STENCIL_TEST;
						}
					}

					buffer.update_stencil_buffer(subsample_row, subsample_col, op_pass, stencil_pass_op, stencil_fail_op, stencil_zfail_op, stencil_ref_val);

					// z test
					if (enable_depth_test)
					{
						if (!buffer.perform_depth_test(ztest_func, subsample_row, subsample_col, vert.position.z))
						{
							op_pass &= ~PerSampleOperation::DEPTH_TEST;
						}
					}

					if (zwrite_mode == ZWrite::ON && (op_pass & PerSampleOperation::DEPTH_TEST) != PerSampleOperation::DISABLE)
					{
						buffer.write_depth(subsample_row, subsample_col, vert.position.z);
					}

					// blending
					if (enable_blending && shader.transparent)
					{
						tinymath::color_rgba dst;
						if (buffer.read_color(subsample_row, subsample_col, dst))
						{
							tinymath::Color dst_color = ColorEncoding::decode(dst);
							tinymath::Color src_color = fragment_result;
							tinymath::Color blended_color = blend(src_color, dst_color, src_factor, dst_factor, blend_op);
							pixel_color = ColorEncoding::encode_rgba(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
						}
					}

					if (validate_fragment(op_pass))
					{
						buffer.write_color(subsample_row, subsample_col, pixel_color);
					}
				}
			}
		}
	}

	void GraphicsDevice::process_fragment(FrameBuffer& buffer, const Vertex& v, const size_t& row, const size_t& col, const Shader& shader)
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
			if (!buffer.perform_depth_test(ztest_func, row, col, z))
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
			if (!buffer.perform_stencil_test(stencil_ref_val, stencil_read_mask, stencil_func, row, col))
			{
				op_pass &= ~PerSampleOperation::STENCIL_TEST;
			}
			buffer.update_stencil_buffer(row, col, op_pass, stencil_pass_op, stencil_fail_op, stencil_zfail_op, stencil_ref_val);
		}

		// depth test
		if (enable_depth_test)
		{
			if (!buffer.perform_depth_test(ztest_func, row, col, z))
			{
				op_pass &= ~PerSampleOperation::DEPTH_TEST;
			}
		}

		// write depth
		if (zwrite_mode == ZWrite::ON && (op_pass & PerSampleOperation::DEPTH_TEST) != PerSampleOperation::DISABLE)
		{
			buffer.write_depth(row, col, z);
		}

		// blending
		if (enable_blending && shader.transparent)
		{
			tinymath::color_rgba dst;
			if (buffer.read_color(row, col, dst))
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
				buffer.write_color(row, col, pixel_color);
			}
			else
			{
				tinymath::color_rgba cur;
				if (buffer.read_color(row, col, cur))
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
					buffer.write_color(row, col, pixel_color);
				}
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

	void GraphicsDevice::draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& v, const tinymath::mat4x4& p, const tinymath::vec2f& screen_translation)
	{
		size_t w, h;
		frame_buffer->get_size(w, h);

		tinymath::vec4f clip_start = p * v * tinymath::vec4f(start);
		tinymath::vec4f clip_end = p * v * tinymath::vec4f(end);

		tinymath::vec4f n1 = Vertex::clip2ndc(clip_start);
		tinymath::vec4f n2 = Vertex::clip2ndc(clip_end);

		tinymath::vec4f s1 = Vertex::ndc2screen(w, h, n1);
		tinymath::vec4f s2 = Vertex::ndc2screen(w, h, n2);

		tinymath::mat4x4 translation = tinymath::translation(tinymath::vec3f(screen_translation));

		s1 = translation * s1;
		s2 = translation * s2;

		SegmentDrawer::bresenham(frame_buffer->get_color_raw_buffer(), (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, ColorEncoding::encode_rgba(col));
	}

	void GraphicsDevice::draw_screen_segment(const tinymath::vec4f& start, const tinymath::vec4f& end, const tinymath::Color& col)
	{
		SegmentDrawer::bresenham(frame_buffer->get_color_raw_buffer(), (int)start.x, (int)start.y, (int)end.x, (int)end.y, ColorEncoding::encode_rgba(col));
	}

	void GraphicsDevice::draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p)
	{
		tinymath::vec4f clip_start = p * v * m * tinymath::vec4f(start.x, start.y, start.z, 1.0f);
		tinymath::vec4f clip_end = p * v * m * tinymath::vec4f(end.x, end.y, end.z, 1.0f);

		draw_segment(clip_start, clip_end, col);
	}

	void GraphicsDevice::draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& v, const tinymath::mat4x4& p)
	{
		tinymath::vec4f clip_start = p * v * tinymath::vec4f(start.x, start.y, start.z, 1.0f);
		tinymath::vec4f clip_end = p * v * tinymath::vec4f(end.x, end.y, end.z, 1.0f);
		draw_segment(clip_start, clip_end, col);
	}

	void GraphicsDevice::draw_segment(const tinymath::vec4f& clip_start, const tinymath::vec4f& clip_end, const tinymath::Color& col)
	{
		size_t w, h;
		frame_buffer->get_size(w, h);

		tinymath::vec4f n1 = Vertex::clip2ndc(clip_start);
		tinymath::vec4f n2 = Vertex::clip2ndc(clip_end);

		tinymath::vec4f s1 = Vertex::ndc2screen(w, h, n1);
		tinymath::vec4f s2 = Vertex::ndc2screen(w, h, n2);

		SegmentDrawer::bresenham(frame_buffer->get_color_raw_buffer(), (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, ColorEncoding::encode_rgba(col));
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
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
	static IdAllocator buffer_id_allocator(kInvalidID + 1, UINT_MAX);

	GraphicsDevice::GraphicsDevice()
	{
		active_frame_buffer_id = kInvalidID;
		statistics.culled_backface_triangle_count = 0;
		statistics.culled_triangle_count = 0;
		statistics.earlyz_optimized = 0;
		statistics.triangle_count = 0;
		multi_thread = true;
		tile_based = true;
	}

	GraphicsDevice::~GraphicsDevice()
	{}

	void GraphicsDevice::resize(size_t w, size_t h)
	{
		INST(GlobalShaderParams).width = w;
		INST(GlobalShaderParams).height = h;

		if (target_rendertexture == nullptr)
		{
			target_rendertexture = std::make_unique<RenderTexture>(w, h, FrameContent::Color | FrameContent::Depth | FrameContent::Stencil, INST(GlobalShaderParams).enable_msaa, INST(GlobalShaderParams).msaa_subsample_count);
		}
		else
		{
			target_rendertexture->resize(w, h);
		}

		if (tile_based_manager == nullptr)
		{
			tile_based_manager = std::make_unique<TileBasedManager>(w, h);
		}
		else
		{
			tile_based_manager->resize(w, h);
		}
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

	RenderTexture* GraphicsDevice::get_active_rendertexture() const noexcept
	{
		if (kInvalidID == active_frame_buffer_id)
		{
			return target_rendertexture.get();
		}

		if (frame_buffer_map.count(active_frame_buffer_id) > 0)
		{
			return frame_buffer_map.at(active_frame_buffer_id).get();
		}

		return target_rendertexture.get();
	}

	uint32_t GraphicsDevice::create_buffer(const size_t& w, const size_t& h, const FrameContent& content) noexcept
	{
		auto buffer = std::make_shared<RenderTexture>(w, h, content);

		uint32_t id;
		if (buffer_id_allocator.alloc(id))
		{
			frame_buffer_map.insert(std::make_pair(id, buffer));
			return id;
		}

		return kInvalidID;
	}

	void GraphicsDevice::set_active_rendertexture(uint32_t& id)
	{
		if (frame_buffer_map.count(id) > 0)
		{
			active_frame_buffer_id = id;
		}
	}

	void GraphicsDevice::reset_active_rendertexture() noexcept
	{
		active_frame_buffer_id = kInvalidID;
	}

	bool GraphicsDevice::get_buffer(const uint32_t& id, std::shared_ptr<RenderTexture>& buffer) const noexcept
	{
		buffer = nullptr;
		if (frame_buffer_map.count(id) > 0)
		{
			buffer = frame_buffer_map.at(id);
			return true;
		}

		return false;
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
		tile_based_manager->foreach_tile(
			[this](auto&& rect, auto&& task_queue)
		{
			this->rasterize_tile(rect, task_queue);
			if (INST(GlobalShaderParams).enable_msaa)
			{
				this->resolve_tile(rect, task_queue);
			}
		});
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
				target_rendertexture->set_msaa_param(msaa->enable, msaa->msaa_subsample_count);
				delete msaa;
			}
		}
	}

	void GraphicsDevice::clear_buffer(const FrameContent& flag)
	{
		process_commands();

		get_active_rendertexture()->clear(flag);

		for (auto& kv : frame_buffer_map)
		{
			kv.second->clear(flag);
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
		get_active_rendertexture()->get_size(w, h);

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
				triangle->update_area();
				tile_based_manager->push_draw_task(*triangle, shader);
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

	void GraphicsDevice::rasterize_tile(const tinymath::Rect& rect, SafeQueue<TileTask>& task_queue)
	{
		while (!task_queue.empty())
		{
			TileTask task;
			if (task_queue.try_consume(task))
			{
				rasterize(rect, task.triangle, *task.shader);

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
			get_active_rendertexture()->foreach_pixel(
				rect,
				[this, rect](auto&& buffer, auto&& pixel)
			{
				float r = (float)pixel.row / rect.max().y;
				float g = (float)pixel.col / rect.max().x;
				tinymath::color_rgba dst;
				if (buffer.get_framebuffer()->read_color(pixel.row, pixel.col, dst))
				{
					tinymath::Color dst_color = ColorEncoding::decode(dst);
					tinymath::Color src_color = tinymath::Color(r, g, 0.0f, 0.5f);
					tinymath::Color blended_color = FrameBuffer::blend(src_color, dst_color, BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendOp::ADD);
					auto pixel_color = ColorEncoding::encode_rgba(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
					buffer.get_framebuffer()->write_color(pixel.row, pixel.col, pixel_color);
				}
			});
		}
	}

	void GraphicsDevice::resolve_tile(const tinymath::Rect& rect, SafeQueue<TileTask>& task_queue)
	{
		UNUSED(task_queue);
		get_active_rendertexture()->foreach_pixel(
			rect,
			[this](auto&& buffer, auto&& pixel)
		{
			tinymath::Color pixel_color = { 0.0f, 0.0f, 0.0f, 1.0f };

			for (uint8_t x_subsample_idx = 0; x_subsample_idx < get_active_rendertexture()->get_subsamples_per_axis(); ++x_subsample_idx)
			{
				for (uint8_t y_subsample_idx = 0; y_subsample_idx < get_active_rendertexture()->get_subsamples_per_axis(); ++y_subsample_idx)
				{
					auto subpixel = buffer.get_subpixel(pixel.row, pixel.col, x_subsample_idx, y_subsample_idx);
					uint8_t coverage = 0;
					if (buffer.get_msaa_framebuffer()->read_coverage(subpixel.row, subpixel.col, coverage) && coverage > 0)
					{
						tinymath::color_rgba msaa_color;
						buffer.get_msaa_framebuffer()->read_color(subpixel.row, subpixel.col, msaa_color);
						pixel_color += ColorEncoding::decode(msaa_color);
					}
				}
			}

			float weight = 1.0f / (float)buffer.get_subsample_count();
			pixel_color *= weight;
			pixel_color.a = 1.0f;
			buffer.get_framebuffer()->write_color(pixel.row, pixel.col, ColorEncoding::encode_rgba(pixel_color));
		}
		);
	}

	void GraphicsDevice::rasterize(const tinymath::Rect& rect, const Triangle& tri, const Shader& shader)
	{
		auto bounds = tri.get_bounds();
		int row_start = bounds.min().y - 1;
		int row_end = bounds.max().y + 1;
		int col_start = bounds.min().x - 1;
		int col_end = bounds.max().x + 1;

		row_start = tinymath::clamp(row_start, rect.y(), rect.y() + rect.size().y);
		row_end = tinymath::clamp(row_end, rect.y(), rect.y() + rect.size().y);
		col_start = tinymath::clamp(col_start, rect.x(), rect.x() + rect.size().x);
		col_end = tinymath::clamp(col_end, rect.y(), rect.x() + rect.size().x);

		if (INST(GlobalShaderParams).enable_msaa && !shader.shadow)
		{
			// msaa on
			get_active_rendertexture()->foreach_pixel_block(
				rect,
				[this, tri, &shader](auto&& buffer, auto&& block)
			{
				SubsampleParam sp1 = { {0, 0, 0, 1}, false };
				SubsampleParam sp2 = { {0, 0, 0, 1}, false };
				SubsampleParam sp3 = { {0, 0, 0, 1}, false };
				SubsampleParam sp4 = { {0, 0, 0, 1}, false };

				for (uint8_t x_subsample_idx = 0; x_subsample_idx < get_active_rendertexture()->get_subsamples_per_axis(); ++x_subsample_idx)
				{
					for (uint8_t y_subsample_idx = 0; y_subsample_idx < get_active_rendertexture()->get_subsamples_per_axis(); ++y_subsample_idx)
					{
						auto px1 = buffer.get_subpixel(block.top_left.row, block.top_left.col, x_subsample_idx, y_subsample_idx);
						auto px2 = buffer.get_subpixel(block.top_right.row, block.top_right.col, x_subsample_idx, y_subsample_idx);
						auto px3 = buffer.get_subpixel(block.bottom_left.row, block.bottom_left.col, x_subsample_idx, y_subsample_idx);
						auto px4 = buffer.get_subpixel(block.bottom_right.row, block.bottom_right.col, x_subsample_idx, y_subsample_idx);
						rasterize_pixel_block(tri, shader, buffer, px1, px2, px3, px4, sp1, sp2, sp3, sp4);
					}
				}
			});
		}
		else
		{
			// msaa off
			get_active_rendertexture()->foreach_pixel_block(
				rect,
				[this, &tri, &shader](auto&& buffer, auto&& block)
			{
				SubsampleParam sp;
				Pixel p1, p2, p3, p4;
				p1 = block.top_left;
				p2 = block.top_right;
				p3 = block.bottom_left;
				p4 = block.bottom_right;
				rasterize_pixel_block(tri, shader, buffer, p1, p2, p3, p4, sp, sp, sp, sp);
			});
		}
	}

	void GraphicsDevice::rasterize_pixel_block(const Triangle& tri,
											   const Shader& shader, 
											   const RenderTexture& rt, 
											   const Pixel& px1,
											   const Pixel& px2,
											   const Pixel& px3,
											   const Pixel& px4,
											   SubsampleParam& p1,
											   SubsampleParam& p2,
											   SubsampleParam& p3,
											   SubsampleParam& p4)
	{
		Vertex top_left_vert, top_right_vert, bottom_left_vert, bottom_right_vert;

		bool top_left_valid = tri.barycentric_interpolate(px1.pos, top_left_vert);
		bool top_right_valid = tri.barycentric_interpolate(px2.pos, top_right_vert);
		bool bottom_left_valid = tri.barycentric_interpolate(px3.pos, bottom_left_vert);
		bool bottom_right_valid = tri.barycentric_interpolate(px4.pos, bottom_right_vert);

		top_left_vert = Vertex::reverse_perspective_division(top_left_vert);
		top_right_vert = Vertex::reverse_perspective_division(top_right_vert);
		bottom_left_vert = Vertex::reverse_perspective_division(bottom_left_vert);
		bottom_right_vert = Vertex::reverse_perspective_division(bottom_right_vert);

		Vertex ddx = Vertex::substract(top_right_vert, top_left_vert);
		Vertex ddy = Vertex::substract(top_right_vert, bottom_left_vert);

		FrameBuffer& fb = INST(GlobalShaderParams).enable_msaa ? *rt.get_msaa_framebuffer() : *rt.get_framebuffer();

		if (top_left_valid)
		{
			process_fragment(fb, top_left_vert, ddx, ddy, px1.row, px1.col, shader, p1);
		}

		if (top_right_valid)
		{
			process_fragment(fb, top_right_vert, ddx, ddy, px2.row, px2.col, shader, p2);
		}

		if (bottom_left_valid)
		{
			process_fragment(fb, bottom_left_vert, ddx, ddy, px3.row, px3.col, shader, p3);
		}

		if (bottom_right_valid)
		{
			process_fragment(fb, bottom_right_vert, ddx, ddy, px4.row, px4.col, shader, p4);
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
		get_active_rendertexture()->get_size(w, h);

		auto bounds = tri.get_bounds();
		int row_start = bounds.min().y - 1;
		int row_end = bounds.max().y + 1;
		int col_start = bounds.min().x - 1;
		int col_end = bounds.max().x + 1;

		row_start = tinymath::clamp(row_start, 0, (int)h);
		row_end = tinymath::clamp(row_end, 0, (int)h);
		col_start = tinymath::clamp(col_start, 0, (int)w);
		col_end = tinymath::clamp(col_end, 0, (int)w);

		auto rect = tinymath::Rect(col_start, row_start, col_end - col_start, row_end - row_start);

		get_active_rendertexture()->foreach_pixel(
			rect,
			[this, &tri, &shader](auto&& buffer, auto&& pixel)
		{
			Vertex vert;
			if (tri.barycentric_interpolate(pixel.pos, vert))
			{
				process_fragment(*buffer.get_framebuffer(), vert, Vertex(), Vertex(), pixel.row, pixel.col, shader);
			}
		});
	}

	void GraphicsDevice::scanline(const Triangle& tri, const Shader& shader)
	{
		size_t w, h;
		get_active_rendertexture()->get_size(w, h);

		bool flip = tri.flip;
		int top_idx = flip ? 2 : 0;
		int bottom_idx = flip ? 0 : 2;
		int top = (int)(tri[top_idx].position.y + 0.5f);
		int bottom = (int)(tri[bottom_idx].position.y + 0.5f);
		top = tinymath::clamp(top, 0, (int)h);
		bottom = tinymath::clamp(bottom, 0, (int)h);
		assert(bottom >= top);

		for (size_t row = (size_t)top; row < (size_t)bottom; row++)
		{
			Vertex lhs, rhs;
			tri.interpolate((float)row + 0.5f, lhs, rhs);

			// screen space clipping
			bool enable_screen_clipping = (INST(GlobalShaderParams).culling_clipping_flag & CullingAndClippingFlag::SCREEN_CLIPPING) != CullingAndClippingFlag::DISABLE;
			if (enable_screen_clipping)
			{
				Clipper::screen_clipping(lhs, rhs, w);
			}

			int left = (int)(lhs.position.x + 0.5f);
			left = tinymath::clamp(left, 0, (int)w);
			int right = (int)(rhs.position.x + 0.5f);
			right = tinymath::clamp(right, 0, (int)w);

			assert(right >= left);
			for (size_t col = (size_t)left; col < (size_t)right; col++)
			{
				process_fragment(*get_active_rendertexture()->get_framebuffer(), lhs, Vertex(), Vertex(), row, col, shader);
				auto dx = Vertex::differential(lhs, rhs);
				lhs = Vertex::intagral(lhs, dx);
			}
		}
	}

	bool GraphicsDevice::process_fragment(FrameBuffer& rt, const Vertex& v, const Vertex& ddx, const Vertex& ddy, const size_t& row, const size_t& col, const Shader& shader)
	{
		SubsampleParam subsample_param;
		return process_fragment(rt, v, ddx, ddy, row, col, shader, subsample_param);
	}

	bool GraphicsDevice::process_fragment(FrameBuffer& buffer, const Vertex& v, const Vertex& ddx, const Vertex& ddy, const size_t& row, const size_t& col, const Shader& shader, SubsampleParam& subsample_param)
	{
		tinymath::color_rgba pixel_color;

		bool enable_scissor_test = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::SCISSOR_TEST) != PerSampleOperation::DISABLE;
		bool enable_alpha_test = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::ALPHA_TEST) != PerSampleOperation::DISABLE;
		bool enable_stencil_test = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::STENCIL_TEST) != PerSampleOperation::DISABLE;
		bool enable_depth_test = (INST(GlobalShaderParams).persample_op_flag & PerSampleOperation::DEPTH_TEST) != PerSampleOperation::DISABLE;

		PerSampleOperation op_pass = PerSampleOperation::SCISSOR_TEST | PerSampleOperation::ALPHA_TEST | PerSampleOperation::STENCIL_TEST | PerSampleOperation::DEPTH_TEST;

		float z = v.position.z / v.position.w;

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
				return false; // here we can assume fragment shader will not modify depth (cuz modifying depth in fragment shader is not implemented yet)
			}
		}

		// fragment shader
		tinymath::Color fragment_result = tinymath::kColorBlack;

		if (!INST(GlobalShaderParams).enable_msaa ||
			!subsample_param.pixel_color_calculated ||
			INST(GlobalShaderParams).multi_sample_frequency == MultiSampleFrequency::kSubsampleFrequency)
		{
			v2f v_out;
			float w = 1.0f;// / v.rhw;
			v_out.position = v.position * w;
			v_out.world_pos = v.world_pos * w;
			v_out.shadow_coord = v.shadow_coord * w;
			v_out.color = v.color * w;
			v_out.normal = v.normal * w;
			v_out.uv = v.uv * w;
			v_out.tangent = v.tangent * w;
			v_out.bitangent = v.bitangent * w;

			fragment_result = shader.fragment_shader(v_out, ddx, ddy);
			pixel_color = ColorEncoding::encode_rgba(fragment_result);
			subsample_param.pixel_color = pixel_color;
			subsample_param.pixel_color_calculated = true;
		}
		else
		{
			pixel_color = subsample_param.pixel_color;
		}

		// todo: scissor test
		if (enable_scissor_test)
		{

		}

		// acctually alpha test is a deprecated feature in newer graphics API, cuz it can be emulated by discard() or clip()
		if (enable_alpha_test)
		{
			if (shader.discarded)
			{
				return false;
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
				tinymath::Color blended_color = FrameBuffer::blend(src_color, dst_color, src_factor, dst_factor, blend_op);
				pixel_color = ColorEncoding::encode_rgba(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
			}
		}

		bool fragment_passed = validate_fragment(op_pass);

		if (fragment_passed)
		{
			buffer.write_coverage(row, col, (uint8_t)1); // pixel is guarantee to be covered here
		}

		// write color
		if (fragment_passed)
		{
			if (color_mask != (ColorMask::R | ColorMask::G | ColorMask::B | ColorMask::A))
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
				}
			}

			buffer.write_color(row, col, pixel_color);
			return true;
		}

		return false;
	}

	bool GraphicsDevice::validate_fragment(const PerSampleOperation& op_pass) const
	{
		if ((op_pass & PerSampleOperation::SCISSOR_TEST) == PerSampleOperation::DISABLE) return false;
		if ((op_pass & PerSampleOperation::ALPHA_TEST) == PerSampleOperation::DISABLE) return false;
		if ((op_pass & PerSampleOperation::STENCIL_TEST) == PerSampleOperation::DISABLE) return false;
		if ((op_pass & PerSampleOperation::DEPTH_TEST) == PerSampleOperation::DISABLE) return false;
		return true;
	}

	void GraphicsDevice::draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& v, const tinymath::mat4x4& p, const tinymath::vec2f& screen_translation)
	{
		size_t w, h;
		target_rendertexture->get_size(w, h);

		tinymath::vec4f clip_start = p * v * tinymath::vec4f(start);
		tinymath::vec4f clip_end = p * v * tinymath::vec4f(end);

		tinymath::vec4f n1 = Vertex::clip2ndc(clip_start);
		tinymath::vec4f n2 = Vertex::clip2ndc(clip_end);

		tinymath::vec4f s1 = Vertex::ndc2screen(w, h, n1);
		tinymath::vec4f s2 = Vertex::ndc2screen(w, h, n2);

		tinymath::mat4x4 translation = tinymath::translation(tinymath::vec3f(screen_translation));

		s1 = translation * s1;
		s2 = translation * s2;

		SegmentDrawer::bresenham(target_rendertexture->get_color_raw_buffer(), (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, ColorEncoding::encode_rgba(col));
	}

	void GraphicsDevice::draw_screen_segment(const tinymath::vec4f& start, const tinymath::vec4f& end, const tinymath::Color& col)
	{
		SegmentDrawer::bresenham(target_rendertexture->get_color_raw_buffer(), (int)start.x, (int)start.y, (int)end.x, (int)end.y, ColorEncoding::encode_rgba(col));
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
		target_rendertexture->get_size(w, h);

		tinymath::vec4f n1 = Vertex::clip2ndc(clip_start);
		tinymath::vec4f n2 = Vertex::clip2ndc(clip_end);

		tinymath::vec4f s1 = Vertex::ndc2screen(w, h, n1);
		tinymath::vec4f s2 = Vertex::ndc2screen(w, h, n2);

		SegmentDrawer::bresenham(target_rendertexture->get_color_raw_buffer(), (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, ColorEncoding::encode_rgba(col));
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
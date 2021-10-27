#include "GraphicsDevice.hpp"
#include <iostream>
#include <execution>
#include <algorithm>
#include "tinymath/color/ColorEncoding.h"
#include "Clipper.hpp"
#include "Pipeline.hpp"
#include "Singleton.hpp"
#include "Logger.hpp"
#include "IdAllocator.hpp"
#include "RawBuffer.hpp"
#include "tinymath.h"
#include "Triangle.hpp"
#include "RenderTexture.hpp"
#include "TileBasedManager.hpp"
#include "GlobalShaderParams.hpp"
#include "tinymath/primitives/Rect.h"
#include "SegmentDrawer.hpp"
#include "Sampling.hpp"
#include "ShaderProgram.hpp"

namespace CpuRasterizer
{
	constexpr resource_id kDefaultRenderTextureID = 0;

	struct SubsampleParam
	{
		tinymath::color_rgba pixel_color;
		bool pixel_color_calculated;
	};

	GraphicsDevice::GraphicsDevice()
	{
		active_frame_buffer_id = kDefaultRenderTextureID;
		statistics.culled_backface_triangle_count = 0;
		statistics.culled_triangle_count = 0;
		statistics.earlyz_optimized = 0;
		statistics.triangle_count = 0;
		multi_thread = true;
		tile_based = true;
		msaa_dirty = false;
		context = GraphicsContext();
		context.msaa_subsample_count = 4;
		context.multi_sample_frequency = MultiSampleFrequency::kPixelFrequency;
		context.pipeline_feature_flag =
			PipelineFeature::kDepthTest |
			PipelineFeature::kFaceCulling |
			PipelineFeature::kZWrite;

		context.face_culling = FaceCulling::Back;
		context.vertex_order = VertexOrder::CW;
		context.color_mask = (ColorMask::kRed | ColorMask::kGreen | ColorMask::kBlue | ColorMask::kAlpha);
		context.stencil_func = CompareFunc::kAlways;
		context.stencil_pass_op = StencilOp::kKeep;
		context.stencil_fail_op = StencilOp::kKeep;
		context.stencil_zfail_op = StencilOp::kKeep;
		context.stencil_read_mask = 0xFF;
		context.stencil_write_mask = 0xFF;
		context.stencil_ref_val = 0;
		context.ztest_func = CompareFunc::kLEqual;
		context.src_factor = BlendFactor::kSrcAlpha;
		context.dst_factor = BlendFactor::kOneMinusSrcAlpha;
		context.blend_op = BlendFunc::kAdd;
		vertex_buffer_table.push_back(std::vector<Vertex>()); // dummy buffer
		index_buffer_table.push_back(std::vector<size_t>()); // dummy buffer
		shader_programs.push_back(nullptr); // dummy shader
		rendertextures.push_back(nullptr); // dummy rt
		context.current_index_buffer_id = 0;
		context.current_vertex_buffer_id = 0;
	}

	GraphicsDevice::~GraphicsDevice()
	{}

	void GraphicsDevice::resize(size_t w, size_t h)
	{
		CpuRasterSharedData.width = w;
		CpuRasterSharedData.height = h;

		if (target_rendertexture == nullptr)
		{
			target_rendertexture = std::make_unique<RenderTexture>(w, h, FrameContent::kColor | FrameContent::kDepth | FrameContent::kStencil, 
																   is_flag_enabled(context, PipelineFeature::kMSAA),
																   context.msaa_subsample_count);
		}
		else
		{
			target_rendertexture->resize(w, h);
		}
	}

	void GraphicsDevice::set_viewport(size_t x, size_t y, size_t w, size_t h)
	{
		UNUSED(x);
		UNUSED(y);
		resize(w, h);
	}

	RenderTexture* GraphicsDevice::get_active_rendertexture() const 
	{
		if (kDefaultRenderTextureID == active_frame_buffer_id)
		{
			return target_rendertexture.get();
		}

		size_t index = static_cast<size_t>(active_frame_buffer_id);
		if (index > kDefaultRenderTextureID && index < rendertextures.size())
		{
			return rendertextures[index].get();
		}

		return target_rendertexture.get();
	}

	resource_id GraphicsDevice::create_buffer(size_t w, size_t h, FrameContent content)
	{
		auto buffer = std::make_shared<RenderTexture>(w, h, content);
		resource_id id = static_cast<resource_id>(rendertextures.size());
		rendertextures.emplace_back(buffer);
		return id;
	}

	void GraphicsDevice::set_active_rendertexture(resource_id id)
	{
		size_t index = static_cast<size_t>(id);
		if (index >= kDefaultRenderTextureID && index < rendertextures.size())
		{
			active_frame_buffer_id = id;
		}
	}

	void GraphicsDevice::reset_active_rendertexture() 
	{
		active_frame_buffer_id = kDefaultRenderTextureID;
	}

	bool GraphicsDevice::get_buffer(resource_id id, std::shared_ptr<RenderTexture>& buffer) const
	{
		buffer = nullptr;
		size_t index = static_cast<size_t>(id);
		if (index >= 0 && index < rendertextures.size())
		{
			buffer = rendertextures[index];
			return true;
		}

		return false;
	}

	resource_id GraphicsDevice::bind_vertex_buffer(const std::vector<Vertex>& buffer)
	{
		resource_id id = static_cast<resource_id>(vertex_buffer_table.size());
		vertex_buffer_table.emplace_back(buffer);
		return id;
	}

	resource_id GraphicsDevice::bind_index_buffer(const std::vector<size_t>& buffer)
	{
		resource_id id = static_cast<resource_id>(index_buffer_table.size());
		index_buffer_table.emplace_back(buffer);
		return id;
	}

	void GraphicsDevice::delete_vertex_buffer(resource_id id)
	{
		size_t index = static_cast<size_t>(id);
		vertex_buffer_table[index] = vertex_buffer_table[vertex_buffer_table.size() - 1];
		vertex_buffer_table.erase(vertex_buffer_table.end()-1);
	}

	void GraphicsDevice::delete_index_buffer(resource_id id)
	{
		size_t index = static_cast<size_t>(id);
		index_buffer_table[index] = index_buffer_table[index_buffer_table.size() - 1];
		index_buffer_table.erase(index_buffer_table.end()-1);
	}

	void GraphicsDevice::use_vertex_buffer(resource_id id)
	{
		context.current_vertex_buffer_id = id;
	}

	void GraphicsDevice::use_index_buffer(resource_id id)
	{
		context.current_index_buffer_id = id;
	}

	void GraphicsDevice::enable_flag(PipelineFeature flag)
	{
		context.pipeline_feature_flag |= flag;
	}

	void GraphicsDevice::disable_flag(PipelineFeature flag)
	{	
		context.pipeline_feature_flag &= ~flag;
	}

	bool GraphicsDevice::is_flag_enabled(PipelineFeature flag)
	{
		return is_flag_enabled(context, flag);
	}

	bool GraphicsDevice::is_flag_enabled(const GraphicsContext& ctx, PipelineFeature flag)
	{
		return (ctx.pipeline_feature_flag & flag) != PipelineFeature::kNone;
	}

	void GraphicsDevice::set_subsample_count(uint8_t count)
	{
		if (count > 0)
		{
			enable_flag(PipelineFeature::kMSAA);
		}
		else
		{
			disable_flag(PipelineFeature::kMSAA);
		}

		context.msaa_subsample_count = count;
		msaa_dirty = true;
	}

	void GraphicsDevice::set_multisample_frequency(MultiSampleFrequency frequency)
	{
		context.multi_sample_frequency = frequency;
	}

	void GraphicsDevice::set_stencil_func(CompareFunc func)
	{
		context.stencil_func = func;
	}

	void GraphicsDevice::set_stencil_op(StencilOp pass_op, StencilOp fail_op, StencilOp zfail_op)
	{
		context.stencil_pass_op = pass_op;
		context.stencil_fail_op = fail_op;
		context.stencil_zfail_op = zfail_op;
	}

	void GraphicsDevice::set_stencil_mask(stencil_t ref_val, stencil_t write_mask, stencil_t read_mask)
	{
		context.stencil_ref_val = ref_val;
		context.stencil_write_mask = write_mask;
		context.stencil_read_mask = read_mask;
	}

	void GraphicsDevice::set_depth_func(CompareFunc func)
	{
		context.ztest_func = func;
	}

	void GraphicsDevice::set_blend_func(BlendFunc func)
	{
		context.blend_op = func;
	}

	void GraphicsDevice::set_blend_factor(BlendFactor src_factor, BlendFactor dst_factor)
	{
		context.src_factor = src_factor;
		context.dst_factor = dst_factor;
	}

	void GraphicsDevice::set_front_face(VertexOrder order)
	{
		context.vertex_order = order;
	}

	void GraphicsDevice::set_cull_face(FaceCulling face_culling)
	{
		context.face_culling = face_culling;
	}

	void GraphicsDevice::set_color_mask(ColorMask mask)
	{
		context.color_mask = mask;
	}

	resource_id GraphicsDevice::create_shader_program(ShaderProgram* shader)
	{
		resource_id id = static_cast<resource_id>(shader_programs.size());
		shader_programs.emplace_back(shader);
		return id;
	}

	void GraphicsDevice::delete_shader_program(resource_id id)
	{
		shader_programs[static_cast<size_t>(id)] = shader_programs[shader_programs.size() - 1];
		shader_programs.erase(shader_programs.end()-1);
	}

	void GraphicsDevice::use_program(resource_id id)
	{
		size_t index = static_cast<size_t>(id);
		if (index >= 0 && index < shader_programs.size())
		{
			context.shader = shader_programs[index];
		}
	}

	void GraphicsDevice::set_uniform_int(resource_id id, property_name prop_id, int v)
	{
		size_t index = static_cast<size_t>(id);
		if (index >= 0 && index < shader_programs.size())
		{
			shader_programs[index]->local_properties.set_int(prop_id, v);
		}
	}

	void GraphicsDevice::set_uniform_float(resource_id id, property_name prop_id, float v)
	{
		size_t index = static_cast<size_t>(id);
		if (index >= 0 && index < shader_programs.size())
		{
			shader_programs[index]->local_properties.set_float(prop_id, v);
		}
	}

	void GraphicsDevice::set_uniform_float4(resource_id id, property_name prop_id, tinymath::vec4f v)
	{
		size_t index = static_cast<size_t>(id);
		if (index >= 0 && index < shader_programs.size())
		{
			shader_programs[index]->local_properties.set_float4(prop_id, v);
		}
	}

	void GraphicsDevice::set_uniform_mat4x4(resource_id id, property_name prop_id, tinymath::mat4x4 mat)
	{
		size_t index = static_cast<size_t>(id);
		if (index >= 0 && index < shader_programs.size())
		{
			shader_programs[index]->local_properties.set_mat4x4(prop_id, mat);
		}
	}

	void GraphicsDevice::draw_primitive()
	{
		if (context.current_index_buffer_id == 0) return;

		for (size_t index = 0; index < index_buffer_table[context.current_index_buffer_id].size(); index += 3)
		{
			context.indices[0] = index;
			context.indices[1] = index + 1;
			context.indices[2] = index + 2;
			contexts.emplace_back(context);
		}
	}

	void GraphicsDevice::fence_primitives()
	{
		std::for_each(
			std::execution::par_unseq,
			contexts.begin(),
			contexts.end(),
			[this](auto&& ctx)
		{
			auto& v1 = vertex_buffer_table[ctx.current_vertex_buffer_id][ctx.indices[0]];
			auto& v2 = vertex_buffer_table[ctx.current_vertex_buffer_id][ctx.indices[1]];
			auto& v3 = vertex_buffer_table[ctx.current_vertex_buffer_id][ctx.indices[2]];
			input2raster(ctx, v1, v2, v3);
		});

		contexts.clear();
	}

	void GraphicsDevice::fence_pixels()
	{
		get_active_rendertexture()->get_tile_based_manager()->foreach_tile(
			[this](auto&& rect, auto&& task_queue)
		{
			this->rasterize_tile(rect, task_queue);
			if (is_flag_enabled(context, PipelineFeature::kMSAA))
			{
				this->resolve_tile(rect, task_queue);
			}
		});
	}

	void GraphicsDevice::clear_buffer(FrameContent flag)
	{
		if (msaa_dirty)
		{
			target_rendertexture->set_msaa_param(is_flag_enabled(context, PipelineFeature::kMSAA), context.msaa_subsample_count);
			msaa_dirty = false;
		}

		get_active_rendertexture()->clear(flag);

		for(size_t index = kDefaultRenderTextureID + 1; index < rendertextures.size(); ++index)
		{
			rendertextures[index]->clear(flag);
		}

		statistics.culled_triangle_count = 0;
		statistics.culled_backface_triangle_count = 0;
		statistics.triangle_count = 0;
		statistics.earlyz_optimized = 0;
	}

	void GraphicsDevice::set_clear_color(const tinymath::Color color)
	{
		target_rendertexture->set_clear_color(ColorEncoding::encode_rgba(color));
	}

	void GraphicsDevice::input2raster(const GraphicsContext& ctx, const Vertex& v1, const Vertex& v2, const Vertex& v3)
	{
		auto& shader = *ctx.shader;

		// vertex stage
		v2f o1 = shader.vertex_shader(vertex_to_a2v(v1)); 
		v2f o2 = shader.vertex_shader(vertex_to_a2v(v2));
		v2f o3 = shader.vertex_shader(vertex_to_a2v(v3));

		Vertex c1 = v2f_to_vertex(o1);
		Vertex c2 = v2f_to_vertex(o2);
		Vertex c3 = v2f_to_vertex(o3);

		if (Clipper::inside_cvv(c1.position, c2.position, c3.position))
		{
			// all in cvv, rasterize directly
			clip2raster(ctx, c1, c2, c3);
			statistics.triangle_count++;
		}
		else
		{
			// clip in homogenous space
			auto triangles = Clipper::clip_triangle(CpuRasterSharedData.cam_near, tinymath::Frustum::homogenous_volume(), c1, c2, c3);

			if (triangles.size() == 0) { statistics.culled_triangle_count++; return; }

			for (size_t idx = 0; idx < triangles.size(); idx++)
			{
				Vertex clip1 = triangles[idx][0]; Vertex clip2 = triangles[idx][1]; Vertex clip3 = triangles[idx][2];
				clip2raster(ctx, clip1, clip2, clip3);
			}

			statistics.triangle_count += triangles.size();
		}
	}

	void GraphicsDevice::clip2raster(const GraphicsContext& ctx, const Vertex& c1, const Vertex& c2, const Vertex& c3)
	{
		// clip space to ndc (perspective division)
		Vertex ndc1 = Pipeline::clip2ndc(c1);
		Vertex ndc2 = Pipeline::clip2ndc(c2);
		Vertex ndc3 = Pipeline::clip2ndc(c3);

		// face culling
		bool double_face = ctx.face_culling == FaceCulling::None;
		bool enable_face_culling = is_flag_enabled(ctx, PipelineFeature::kFaceCulling);
		if (!double_face && enable_face_culling)
		{
			if ((ndc1.mask & ndc2.mask & ndc3.mask & kNormalMask) != 0)
			{
				if (Clipper::backface_culling_ndc(ndc1.normal)) { statistics.culled_backface_triangle_count++; return; }
			}
			else {
				if (ctx.vertex_order == VertexOrder::CCW)
				{
					if (Clipper::backface_culling_ndc(ndc3.position.xyz, ndc2.position.xyz, ndc1.position.xyz)) { statistics.culled_backface_triangle_count++; return; }
				}
				else
				{
					if (Clipper::backface_culling_ndc(ndc1.position.xyz, ndc2.position.xyz, ndc3.position.xyz)) { statistics.culled_backface_triangle_count++; return; }
				}
			}
		}

		size_t w, h;
		get_active_rendertexture()->get_size(w, h);

		// ndc to screen space
		Vertex s1 = Pipeline::ndc2screen(w, h, ndc1);
		Vertex s2 = Pipeline::ndc2screen(w, h, ndc2);
		Vertex s3 = Pipeline::ndc2screen(w, h, ndc3);

		// triangle assembly
		std::vector<Triangle> assembled_triangles = Triangle(s1, s2, s3).horizontally_split();
		for (auto triangle = assembled_triangles.begin(); triangle != assembled_triangles.end(); triangle++)
		{
			if (this->tile_based)
			{
				// push rasterization task
				get_active_rendertexture()->get_tile_based_manager()->push_draw_task(*triangle, ctx);
			}
			else
			{
				// rasterize triangle directly
				rasterize(*triangle, ctx, RasterizerStrategy::kScanline);
			}
		}
	}

	void GraphicsDevice::rasterize_tile(const tinymath::Rect& rect, SafeQueue<TileTask>& task_queue)
	{
		while (!task_queue.empty())
		{
			TileTask task;
			if (task_queue.try_consume(task))
			{
				rasterize(rect, task.context, task.triangle);

				// wireframe
				if ((CpuRasterSharedData.debug_flag & RenderFlag::kWireFrame) != RenderFlag::kNone)
				{
					draw_screen_segment(task.triangle[0].position, task.triangle[1].position, tinymath::Color(0.5f, 0.5f, 1.0f, 1.0f));
					draw_screen_segment(task.triangle[0].position, task.triangle[2].position, tinymath::Color(0.5f, 0.5f, 1.0f, 1.0f));
					draw_screen_segment(task.triangle[2].position, task.triangle[1].position, tinymath::Color(0.5f, 0.5f, 1.0f, 1.0f));
				}
			}
		}

		if ((CpuRasterSharedData.debug_flag & RenderFlag::kFrameTile) != RenderFlag::kNone)
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
					tinymath::Color blended_color = FrameBuffer::blend(src_color, dst_color, BlendFactor::kSrcAlpha, BlendFactor::kOneMinusSrcAlpha, BlendFunc::kAdd);
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

			for (uint8_t x_subsample_idx = 0; x_subsample_idx < buffer.get_subsamples_per_axis(); ++x_subsample_idx)
			{
				for (uint8_t y_subsample_idx = 0; y_subsample_idx < buffer.get_subsamples_per_axis(); ++y_subsample_idx)
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

	void GraphicsDevice::rasterize(const tinymath::Rect& rect, const GraphicsContext& ctx, const Triangle& tri)
	{
		auto bounds = tri.get_bounds();
		int padding = kBoundsPadding;
		int row_start = bounds.min().y - padding;
		int row_end = bounds.max().y + padding;
		int col_start = bounds.min().x - padding;
		int col_end = bounds.max().x + padding;

		row_start = tinymath::clamp(row_start, rect.y(), rect.y() + rect.size().y);
		row_end = tinymath::clamp(row_end, rect.y(), rect.y() + rect.size().y);
		col_start = tinymath::clamp(col_start, rect.x(), rect.x() + rect.size().x);
		col_end = tinymath::clamp(col_end, rect.y(), rect.x() + rect.size().x);

		if (is_flag_enabled(ctx, PipelineFeature::kMSAA))
		{
			// msaa on
			get_active_rendertexture()->foreach_pixel_block(
				rect,
				[this, tri, &ctx](auto&& buffer, auto&& block)
			{
				SubsampleParam sp1 = { {0, 0, 0, 1}, false };
				SubsampleParam sp2 = { {0, 0, 0, 1}, false };
				SubsampleParam sp3 = { {0, 0, 0, 1}, false };
				SubsampleParam sp4 = { {0, 0, 0, 1}, false };

				for (uint8_t x_subsample_idx = 0; x_subsample_idx < buffer.get_subsamples_per_axis(); ++x_subsample_idx)
				{
					for (uint8_t y_subsample_idx = 0; y_subsample_idx < buffer.get_subsamples_per_axis(); ++y_subsample_idx)
					{
						auto px1 = buffer.get_subpixel(block.top_left.row, block.top_left.col, x_subsample_idx, y_subsample_idx);
						auto px2 = buffer.get_subpixel(block.top_right.row, block.top_right.col, x_subsample_idx, y_subsample_idx);
						auto px3 = buffer.get_subpixel(block.bottom_left.row, block.bottom_left.col, x_subsample_idx, y_subsample_idx);
						auto px4 = buffer.get_subpixel(block.bottom_right.row, block.bottom_right.col, x_subsample_idx, y_subsample_idx);
						rasterize_pixel_block(tri, ctx, buffer, px1, px2, px3, px4, sp1, sp2, sp3, sp4);
					}
				}
			});
		}
		else
		{
			// msaa off
			get_active_rendertexture()->foreach_pixel_block(
				rect,
				[this, &tri, &ctx](auto&& buffer, auto&& block)
			{
				SubsampleParam sp;
				Pixel p1, p2, p3, p4;
				p1 = block.top_left;
				p2 = block.top_right;
				p3 = block.bottom_left;
				p4 = block.bottom_right;
				rasterize_pixel_block(tri, ctx, buffer, p1, p2, p3, p4, sp, sp, sp, sp);
			});
		}
	}

	void GraphicsDevice::rasterize_pixel_block(const Triangle& tri,
											   const GraphicsContext& ctx,
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
		Fragment frag1, frag2, frag3, frag4;

		bool px1_inside = tri.barycentric_interpolate(px1.pos, frag1);
		bool px2_inside = tri.barycentric_interpolate(px2.pos, frag2);
		bool px3_inside = tri.barycentric_interpolate(px3.pos, frag3);
		bool px4_inside = tri.barycentric_interpolate(px4.pos, frag4);

		frag1 = Pipeline::reverse_perspective_division(frag1);
		frag2 = Pipeline::reverse_perspective_division(frag2);
		frag3 = Pipeline::reverse_perspective_division(frag3);
		frag4 = Pipeline::reverse_perspective_division(frag4);

		Fragment ddx = Pipeline::substract(frag1, frag2);
		Fragment ddy = Pipeline::substract(frag1, frag3);

		FrameBuffer& fb = is_flag_enabled(ctx, PipelineFeature::kMSAA) ? *rt.get_msaa_framebuffer() : *rt.get_framebuffer();

		if (px1_inside)
		{
			multisample_fragment_stage(fb, frag1, ddx, ddy, px1.row, px1.col, ctx, p1);
		}

		if (px2_inside)
		{
			multisample_fragment_stage(fb, frag2, ddx, ddy, px2.row, px2.col, ctx, p2);
		}

		if (px3_inside)
		{
			multisample_fragment_stage(fb, frag3, ddx, ddy, px3.row, px3.col, ctx, p3);
		}

		if (px4_inside)
		{
			multisample_fragment_stage(fb, frag4, ddx, ddy, px4.row, px4.col, ctx, p4);
		}
	}

	void GraphicsDevice::rasterize(const Triangle& tri, const GraphicsContext& ctx, RasterizerStrategy strategy)
	{
		if (strategy == RasterizerStrategy::kScanblock)
		{
			scanblock(tri, ctx);
		}
		else
		{
			scanline(tri, ctx);
		}

		// wireframe
		if ((CpuRasterSharedData.debug_flag & RenderFlag::kWireFrame) != RenderFlag::kNone)
		{
			draw_screen_segment(tri[0].position, tri[1].position, tinymath::Color(1.0f, 1.0f, 1.0f, 1.0f));
			draw_screen_segment(tri[0].position, tri[2].position, tinymath::Color(1.0f, 1.0f, 1.0f, 1.0f));
			draw_screen_segment(tri[2].position, tri[1].position, tinymath::Color(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	void GraphicsDevice::scanblock(const Triangle& tri, const GraphicsContext& ctx)
	{
		size_t w, h;
		get_active_rendertexture()->get_size(w, h);
		int padding = kBoundsPadding;
		auto bounds = tri.get_bounds();
		int row_start = bounds.min().y - padding;
		int row_end = bounds.max().y + padding;
		int col_start = bounds.min().x - padding;
		int col_end = bounds.max().x + padding;

		row_start = tinymath::clamp(row_start, 0, (int)h);
		row_end = tinymath::clamp(row_end, 0, (int)h);
		col_start = tinymath::clamp(col_start, 0, (int)w);
		col_end = tinymath::clamp(col_end, 0, (int)w);

		auto rect = tinymath::Rect(col_start, row_start, col_end - col_start, row_end - row_start);

		get_active_rendertexture()->foreach_pixel(
			rect,
			[this, &tri, &ctx](auto&& buffer, auto&& pixel)
		{
			Fragment frag;
			if (tri.barycentric_interpolate(pixel.pos, frag))
			{
				fragment_stage(*buffer.get_framebuffer(), frag, Fragment(), Fragment(), pixel.row, pixel.col, ctx);
			}
		});
	}

	void GraphicsDevice::scanline(const Triangle& tri, const GraphicsContext& ctx)
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
			Fragment lhs, rhs;
			tri.interpolate((float)row + 0.5f, lhs, rhs);

			// screen space clipping
			Clipper::clip_horizontally(lhs, rhs, w);
			
			int left = (int)(lhs.position.x + 0.5f);
			left = tinymath::clamp(left, 0, (int)w);
			int right = (int)(rhs.position.x + 0.5f);
			right = tinymath::clamp(right, 0, (int)w);

			assert(right >= left);
			for (size_t col = (size_t)left; col < (size_t)right; col++)
			{
				fragment_stage(*get_active_rendertexture()->get_framebuffer(), lhs, Fragment(), Fragment(), row, col, ctx);
				auto dx = Pipeline::differential(lhs, rhs);
				lhs = Pipeline::intagral(lhs, dx);
			}
		}
	}

	bool GraphicsDevice::fragment_stage(FrameBuffer& rt, const Fragment& frag, const Fragment& ddx, const Fragment& ddy, size_t row, size_t col, const GraphicsContext& ctx)
	{
		SubsampleParam subsample_param;
		return multisample_fragment_stage(rt, frag, ddx, ddy, row, col, ctx, subsample_param);
	}

	bool GraphicsDevice::multisample_fragment_stage(FrameBuffer& buffer, const Fragment& frag, const Fragment& ddx, const Fragment& ddy, size_t row, size_t col, const GraphicsContext& ctx, SubsampleParam& subsample_param)
	{
		auto& shader = *ctx.shader;

		tinymath::color_rgba pixel_color;

		bool enable_scissor_test = is_flag_enabled(ctx, PipelineFeature::kScissorTest);
		bool enable_alpha_test = is_flag_enabled(ctx, PipelineFeature::kAlphaTest); 
		bool enable_stencil_test = is_flag_enabled(ctx, PipelineFeature::kStencilTest); 
		bool enable_depth_test = is_flag_enabled(ctx, PipelineFeature::kDepthTest);

		PipelineFeature op_pass = PipelineFeature::kScissorTest | PipelineFeature::kAlphaTest | PipelineFeature::kStencilTest | PipelineFeature::kDepthTest;

		float z = frag.position.z / frag.position.w;

		ColorMask color_mask = ctx.color_mask;
		CompareFunc stencil_func = ctx.stencil_func;
		StencilOp stencil_pass_op = ctx.stencil_pass_op;
		StencilOp stencil_fail_op = ctx.stencil_fail_op;
		StencilOp stencil_zfail_op = ctx.stencil_zfail_op;
		uint8_t stencil_read_mask = ctx.stencil_read_mask;
		uint8_t stencil_write_mask = ctx.stencil_write_mask;
		uint8_t stencil_ref_val = ctx.stencil_ref_val;
		CompareFunc ztest_func = ctx.ztest_func;
		bool zwrite_on = is_flag_enabled(ctx, PipelineFeature::kZWrite);
		BlendFactor src_factor = ctx.src_factor;
		BlendFactor dst_factor = ctx.dst_factor;
		BlendFunc blend_op = ctx.blend_op;
		bool enable_blending = is_flag_enabled(ctx, PipelineFeature::kBlending);

		UNUSED(stencil_write_mask);

		// early-z
		if (enable_depth_test && !enable_alpha_test && !enable_stencil_test)
		{
			if (!buffer.perform_depth_test(ztest_func, row, col, z))
			{
				op_pass &= ~PipelineFeature::kDepthTest;
				statistics.earlyz_optimized++;
				return false; // here we can assume fragment shader will not modify depth (cuz modifying depth in fragment shader is not implemented yet)
			}
		}

		// fragment shader
		tinymath::Color fragment_result = tinymath::kColorBlack;

		if (!is_flag_enabled(ctx, PipelineFeature::kMSAA) ||
			!subsample_param.pixel_color_calculated ||
			ctx.multi_sample_frequency == MultiSampleFrequency::kSubsampleFrequency)
		{
			v2f v_out = frag_to_v2f(frag);

			v_out.ddx = ddx;
			v_out.ddy = ddy;

			fragment_result = shader.fragment_shader(v_out);
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
				op_pass &= ~PipelineFeature::kStencilTest;
			}
			buffer.update_stencil_buffer(row, col, op_pass, stencil_pass_op, stencil_fail_op, stencil_zfail_op, stencil_ref_val);
		}

		// depth test
		if (enable_depth_test)
		{
			if (!buffer.perform_depth_test(ztest_func, row, col, z))
			{
				op_pass &= ~PipelineFeature::kDepthTest;
			}
		}

		// write depth
		if (zwrite_on && (op_pass & PipelineFeature::kDepthTest) != PipelineFeature::kNone)
		{
			buffer.write_depth(row, col, z);
		}

		// blending
		if (enable_blending)
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
			if (color_mask != (ColorMask::kRed | ColorMask::kGreen | ColorMask::kBlue | ColorMask::kAlpha))
			{
				tinymath::color_rgba cur;
				if (buffer.read_color(row, col, cur))
				{
					if ((color_mask & ColorMask::kRed) == ColorMask::kZero)
					{
						pixel_color.r = cur.r;
					}
					if ((color_mask & ColorMask::kGreen) == ColorMask::kZero)
					{
						pixel_color.g = cur.g;
					}
					if ((color_mask & ColorMask::kBlue) == ColorMask::kZero)
					{
						pixel_color.b = cur.b;
					}
					if ((color_mask & ColorMask::kAlpha) == ColorMask::kZero)
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

	bool GraphicsDevice::validate_fragment(PipelineFeature op_pass) const
	{
		if ((op_pass & PipelineFeature::kScissorTest) == PipelineFeature::kNone) return false;
		if ((op_pass & PipelineFeature::kAlphaTest) == PipelineFeature::kNone) return false;
		if ((op_pass & PipelineFeature::kStencilTest) == PipelineFeature::kNone) return false;
		if ((op_pass & PipelineFeature::kDepthTest) == PipelineFeature::kNone) return false;
		return true;
	}

	void GraphicsDevice::draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& v, const tinymath::mat4x4& p, const tinymath::vec2f& screen_translation)
	{
		size_t w, h;
		target_rendertexture->get_size(w, h);

		tinymath::vec4f clip_start = p * v * tinymath::vec4f(start);
		tinymath::vec4f clip_end = p * v * tinymath::vec4f(end);

		bool clip = true;
		auto start_clipped = clip_start;
		auto end_clipped = clip_end;

		if ((Clipper::inside_cvv(clip_start) && Clipper::inside_cvv(clip_end)) || Clipper::clip_segment(CpuRasterSharedData.cam_near, clip_start, clip_end, start_clipped, end_clipped))
		{
			clip = false;
		}

		if (!clip)
		{
			tinymath::vec4f n1 = Pipeline::clip2ndc(start_clipped);
			tinymath::vec4f n2 = Pipeline::clip2ndc(end_clipped);

			tinymath::vec4f s1 = Pipeline::ndc2screen(w, h, n1);
			tinymath::vec4f s2 = Pipeline::ndc2screen(w, h, n2);

			tinymath::mat4x4 translation = tinymath::translation(tinymath::vec3f(screen_translation));

			s1 = translation * s1;
			s2 = translation * s2;

			SegmentDrawer::bresenham(target_rendertexture->get_color_raw_buffer(), (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, ColorEncoding::encode_rgba(col));
		}
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

	void GraphicsDevice::draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& mvp)
	{
		tinymath::vec4f clip_start = mvp * tinymath::vec4f(start.x, start.y, start.z, 1.0f);
		tinymath::vec4f clip_end = mvp * tinymath::vec4f(end.x, end.y, end.z, 1.0f);
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

		bool clip = true;
		auto start_clipped = clip_start;
		auto end_clipped = clip_end;

		if ((Clipper::inside_cvv(clip_start) && Clipper::inside_cvv(clip_end)) || Clipper::clip_segment(CpuRasterSharedData.cam_near, clip_start, clip_end, start_clipped, end_clipped))
		{
			clip = false;
		}

		if (!clip)
		{
			tinymath::vec4f n1 = Pipeline::clip2ndc(start_clipped);
			tinymath::vec4f n2 = Pipeline::clip2ndc(end_clipped);

			tinymath::vec4f s1 = Pipeline::ndc2screen(w, h, n1);
			tinymath::vec4f s2 = Pipeline::ndc2screen(w, h, n2);

			SegmentDrawer::bresenham(target_rendertexture->get_color_raw_buffer(), (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, ColorEncoding::encode_rgba(col));
		}
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
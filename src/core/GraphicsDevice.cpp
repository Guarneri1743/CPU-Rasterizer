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
#include "Shader.hpp"

namespace CpuRasterizor
{
	static IdAllocator buffer_id_allocator(kInvalidID + 1, UINT_MAX);

	struct SubsampleParam
	{
		tinymath::color_rgba pixel_color;
		bool pixel_color_calculated;
	};

	struct RenderCommand
	{
		Shader* shader;
		Vertex v1;
		Vertex v2;
		Vertex v3;
	};

	class GraphicsCommand
	{
		virtual void execute() = 0;
	};

	class MsaaCommand : public GraphicsCommand
	{
	public:
		bool enable;
		uint8_t msaa_subsample_count;

	public:
		void execute() {}
	};

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
		CpuRasterSharedData.width = w;
		CpuRasterSharedData.height = h;

		if (target_rendertexture == nullptr)
		{
			target_rendertexture = std::make_unique<RenderTexture>(w, h, FrameContent::kColor | FrameContent::kDepth | FrameContent::kStencil, CpuRasterSharedData.enable_msaa, CpuRasterSharedData.msaa_subsample_count);
		}
		else
		{
			target_rendertexture->resize(w, h);
		}
	}

	void GraphicsDevice::set_viewport(size_t w, size_t h)
	{
		resize(w, h);
	}

	void GraphicsDevice::set_subsample_count(uint8_t count)
	{
		CpuRasterSharedData.enable_msaa = count > 0;
		CpuRasterSharedData.msaa_subsample_count = count;
		CpuRasterSharedData.msaa_dirty = true;
	}

	RenderTexture* GraphicsDevice::get_active_rendertexture() const 
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

	uint32_t GraphicsDevice::create_buffer(size_t w, size_t h, FrameContent content) 
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

	bool GraphicsDevice::try_alloc_id(uint32_t& id)
	{
		return buffer_id_allocator.alloc(id);
	}

	void GraphicsDevice::set_active_rendertexture(uint32_t& id)
	{
		if (frame_buffer_map.count(id) > 0)
		{
			active_frame_buffer_id = id;
		}
	}

	void GraphicsDevice::reset_active_rendertexture() 
	{
		active_frame_buffer_id = kInvalidID;
	}

	bool GraphicsDevice::get_buffer(uint32_t id, std::shared_ptr<RenderTexture>& buffer) const 
	{
		buffer = nullptr;
		if (frame_buffer_map.count(id) > 0)
		{
			buffer = frame_buffer_map.at(id);
			return true;
		}

		return false;
	}

	void GraphicsDevice::draw(const RenderCommand& task)
	{
		input2raster(*task.shader, task.v1, task.v2, task.v3);
	}

	void GraphicsDevice::submit_primitive(Shader* shader, const Vertex& v1, const Vertex& v2, const Vertex& v3)
	{
		RenderCommand cmd = { shader, v1, v2, v3 };
		primitive_commands.emplace_back(cmd);
	}

	void GraphicsDevice::fence_primitives()
	{
		std::for_each(
			std::execution::par_unseq,
			primitive_commands.begin(),
			primitive_commands.end(),
			[this](auto&& cmd)
		{
			this->draw(cmd);
		});
		primitive_commands.clear();
	}

	void GraphicsDevice::fence_pixels()
	{
		get_active_rendertexture()->get_tile_based_manager()->foreach_tile(
			[this](auto&& rect, auto&& task_queue)
		{
			this->rasterize_tile(rect, task_queue);
			if (CpuRasterSharedData.enable_msaa)
			{
				this->resolve_tile(rect, task_queue);
			}
		});
	}

	void GraphicsDevice::clear_buffer(FrameContent flag)
	{
		if (CpuRasterSharedData.msaa_dirty)
		{
			target_rendertexture->set_msaa_param(CpuRasterSharedData.enable_msaa, CpuRasterSharedData.msaa_subsample_count);
			CpuRasterSharedData.msaa_dirty = false;
		}

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

	void GraphicsDevice::set_clear_color(const tinymath::Color color)
	{
		target_rendertexture->set_clear_color(ColorEncoding::encode_rgba(color));
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
		c1.texcoord0 = o1.texcoord0;
		c1.texcoord1 = o1.texcoord1;
		c1.texcoord2 = o1.texcoord2;
		c1.texcoord3 = o1.texcoord3;
		c1.texcoord4 = o1.texcoord4;
		c1.texcoord5 = o1.texcoord5;
		c1.texcoord6 = o1.texcoord6;
		c1.texcoord7 = o1.texcoord7;
		c1.texcoord8 = o1.texcoord8;

		if (Clipper::inside_cvv(c1.position, c2.position, c3.position))
		{
			// all in cvv, rasterize directly
			clip2raster(shader, c1, c2, c3);
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
				clip2raster(shader, clip1, clip2, clip3);
			}

			statistics.triangle_count += triangles.size();
		}
	}

	void GraphicsDevice::clip2raster(const Shader& shader, const Vertex& c1, const Vertex& c2, const Vertex& c3)
	{
		// clip space to ndc (perspective division)
		Vertex ndc1 = Pipeline::clip2ndc(c1);
		Vertex ndc2 = Pipeline::clip2ndc(c2);
		Vertex ndc3 = Pipeline::clip2ndc(c3);

		// backface culling
		bool double_face = shader.double_face;
		bool enable_backface_culling = (CpuRasterSharedData.culling_clipping_flag & CullingAndClippingFlag::kBackFaceCulling) != CullingAndClippingFlag::kNone;
		if (!double_face && enable_backface_culling && !shader.skybox)
		{
			if (Clipper::backface_culling_ndc(ndc1.position.xyz, ndc2.position.xyz, ndc3.position.xyz)) { statistics.culled_backface_triangle_count++; return; }
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
				// push tile based draw task
				get_active_rendertexture()->get_tile_based_manager()->push_draw_task(*triangle, shader);
			}
			else
			{
				// rasterize triangle directly
				rasterize(*triangle, shader, RasterizerStrategy::kScanline);
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
		input.texcoord0 = vert.texcoord0;
		input.texcoord1 = vert.texcoord1;
		input.texcoord2 = vert.texcoord2;
		input.texcoord3 = vert.texcoord3;
		input.texcoord4 = vert.texcoord4;
		input.texcoord5 = vert.texcoord5;
		input.texcoord6 = vert.texcoord6;
		input.texcoord7 = vert.texcoord7;
		input.texcoord8 = vert.texcoord8;
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
					tinymath::Color blended_color = FrameBuffer::blend(src_color, dst_color, BlendFactor::kSrcAlpha, BlendFactor::kOneMinusSrcAlpha, BlendOp::kAdd);
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

	void GraphicsDevice::rasterize(const tinymath::Rect& rect, const Triangle& tri, const Shader& shader)
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

		if (CpuRasterSharedData.enable_msaa)
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

				for (uint8_t x_subsample_idx = 0; x_subsample_idx < buffer.get_subsamples_per_axis(); ++x_subsample_idx)
				{
					for (uint8_t y_subsample_idx = 0; y_subsample_idx < buffer.get_subsamples_per_axis(); ++y_subsample_idx)
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

		FrameBuffer& fb = (CpuRasterSharedData.enable_msaa) ? *rt.get_msaa_framebuffer() : *rt.get_framebuffer();

		if (px1_inside)
		{
			process_fragment(fb, frag1, ddx, ddy, px1.row, px1.col, shader, p1);
		}

		if (px2_inside)
		{
			process_fragment(fb, frag2, ddx, ddy, px2.row, px2.col, shader, p2);
		}

		if (px3_inside)
		{
			process_fragment(fb, frag3, ddx, ddy, px3.row, px3.col, shader, p3);
		}

		if (px4_inside)
		{
			process_fragment(fb, frag4, ddx, ddy, px4.row, px4.col, shader, p4);
		}
	}

	void GraphicsDevice::rasterize(const Triangle& tri, const Shader& shader, RasterizerStrategy strategy)
	{
		if (strategy == RasterizerStrategy::kScanblock)
		{
			scanblock(tri, shader);
		}
		else
		{
			scanline(tri, shader);
		}

		// wireframe
		if ((CpuRasterSharedData.debug_flag & RenderFlag::kWireFrame) != RenderFlag::kNone)
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
			[this, &tri, &shader](auto&& buffer, auto&& pixel)
		{
			Fragment frag;
			if (tri.barycentric_interpolate(pixel.pos, frag))
			{
				process_fragment(*buffer.get_framebuffer(), frag, Fragment(), Fragment(), pixel.row, pixel.col, shader);
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
			Fragment lhs, rhs;
			tri.interpolate((float)row + 0.5f, lhs, rhs);

			// screen space clipping
			bool enable_screen_clipping = (CpuRasterSharedData.culling_clipping_flag & CullingAndClippingFlag::kScreenClipping) != CullingAndClippingFlag::kNone;
			if (enable_screen_clipping)
			{
				Clipper::clip_horizontally(lhs, rhs, w);
			}

			int left = (int)(lhs.position.x + 0.5f);
			left = tinymath::clamp(left, 0, (int)w);
			int right = (int)(rhs.position.x + 0.5f);
			right = tinymath::clamp(right, 0, (int)w);

			assert(right >= left);
			for (size_t col = (size_t)left; col < (size_t)right; col++)
			{
				process_fragment(*get_active_rendertexture()->get_framebuffer(), lhs, Fragment(), Fragment(), row, col, shader);
				auto dx = Pipeline::differential(lhs, rhs);
				lhs = Pipeline::intagral(lhs, dx);
			}
		}
	}

	bool GraphicsDevice::process_fragment(FrameBuffer& rt, const Fragment& frag, const Fragment& ddx, const Fragment& ddy, size_t row, size_t col, const Shader& shader)
	{
		SubsampleParam subsample_param;
		return process_fragment(rt, frag, ddx, ddy, row, col, shader, subsample_param);
	}

	bool GraphicsDevice::process_fragment(FrameBuffer& buffer, const Fragment& frag, const Fragment& ddx, const Fragment& ddy, size_t row, size_t col, const Shader& shader, SubsampleParam& subsample_param)
	{
		tinymath::color_rgba pixel_color;

		bool enable_scissor_test = (CpuRasterSharedData.persample_op_flag & PerSampleOperation::kScissorTest) != PerSampleOperation::kNone;
		bool enable_alpha_test = (CpuRasterSharedData.persample_op_flag & PerSampleOperation::kAlphaTest) != PerSampleOperation::kNone;
		bool enable_stencil_test = (CpuRasterSharedData.persample_op_flag & PerSampleOperation::kStencilTest) != PerSampleOperation::kNone;
		bool enable_depth_test = (CpuRasterSharedData.persample_op_flag & PerSampleOperation::kDepthTest) != PerSampleOperation::kNone;

		PerSampleOperation op_pass = PerSampleOperation::kScissorTest | PerSampleOperation::kAlphaTest | PerSampleOperation::kStencilTest | PerSampleOperation::kDepthTest;

		float z = frag.position.z / frag.position.w;

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

		bool enable_blending = (CpuRasterSharedData.persample_op_flag & PerSampleOperation::kBlending) != PerSampleOperation::kNone && shader.transparent;

		// early-z
		if (enable_depth_test && !enable_alpha_test)
		{
			if (!buffer.perform_depth_test(ztest_func, row, col, z))
			{
				op_pass &= ~PerSampleOperation::kDepthTest;
				statistics.earlyz_optimized++;
				return false; // here we can assume fragment shader will not modify depth (cuz modifying depth in fragment shader is not implemented yet)
			}
		}

		// fragment shader
		tinymath::Color fragment_result = tinymath::kColorBlack;

		if (!CpuRasterSharedData.enable_msaa ||
			!subsample_param.pixel_color_calculated ||
			CpuRasterSharedData.multi_sample_frequency == MultiSampleFrequency::kSubsampleFrequency)
		{
			v2f v_out;
			v_out.position = frag.position;
			v_out.world_pos = frag.world_pos;
			v_out.shadow_coord = frag.shadow_coord;
			v_out.color = frag.color;
			v_out.normal = frag.normal;
			v_out.uv = frag.uv;
			v_out.tangent = frag.tangent;
			v_out.bitangent = frag.bitangent;

			v_out.texcoord0 = frag.texcoord0;
			v_out.texcoord1 = frag.texcoord1;
			v_out.texcoord2 = frag.texcoord2;
			v_out.texcoord3 = frag.texcoord3;
			v_out.texcoord4 = frag.texcoord4;
			v_out.texcoord5 = frag.texcoord5;
			v_out.texcoord6 = frag.texcoord6;
			v_out.texcoord7 = frag.texcoord7;
			v_out.texcoord8 = frag.texcoord8;

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
				op_pass &= ~PerSampleOperation::kStencilTest;
			}
			buffer.update_stencil_buffer(row, col, op_pass, stencil_pass_op, stencil_fail_op, stencil_zfail_op, stencil_ref_val);
		}

		// depth test
		if (enable_depth_test)
		{
			if (!buffer.perform_depth_test(ztest_func, row, col, z))
			{
				op_pass &= ~PerSampleOperation::kDepthTest;
			}
		}

		// write depth
		if (zwrite_mode == ZWrite::kOn && (op_pass & PerSampleOperation::kDepthTest) != PerSampleOperation::kNone)
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

	bool GraphicsDevice::validate_fragment(PerSampleOperation op_pass) const
	{
		if ((op_pass & PerSampleOperation::kScissorTest) == PerSampleOperation::kNone) return false;
		if ((op_pass & PerSampleOperation::kAlphaTest) == PerSampleOperation::kNone) return false;
		if ((op_pass & PerSampleOperation::kStencilTest) == PerSampleOperation::kNone) return false;
		if ((op_pass & PerSampleOperation::kDepthTest) == PerSampleOperation::kNone) return false;
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
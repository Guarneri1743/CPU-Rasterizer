#pragma once
#include <stdint.h>
#include <queue>
#include <memory>
#include <vector>
#include <future>
#include "Define.hpp"
#include "RasterAttributes.hpp"
#include "RenderTexture.hpp"
#include "TileBasedManager.hpp"
#include "RasterAttributes.hpp"
#include "ShaderProgram.hpp"
#include "Triangle.hpp"
#include "GraphicsContext.hpp"

#define CpuRasterDevice Singleton<CpuRasterizer::GraphicsDevice>::get()

namespace CpuRasterizer
{
	struct SubsampleParam;

	class GraphicsDevice
	{
	public:
		GraphicsDevice();
		~GraphicsDevice();

		// misc
		void set_viewport(size_t x, size_t y, size_t w, size_t h);
		void draw_primitive();
		void fence_primitives();
		void fence_pixels();
		void clear_buffer(FrameContent flag);
		void set_clear_color(const tinymath::Color color);
		resource_id create_buffer(size_t width, size_t height, FrameContent content);
		bool get_buffer(resource_id id, std::shared_ptr<RenderTexture>& buffer) const;

		// shader
		size_t create_shader_program(ShaderProgram* shader);
		void delete_shader_program(resource_id id);
		void use_program(resource_id id);
		void set_uniform_int(resource_id id, property_name prop_id, int v);
		void set_uniform_float(resource_id id, property_name prop_id, float v);
		void set_uniform_float4(resource_id id, property_name prop_id, tinymath::vec4f v);
		void set_uniform_mat4x4(resource_id id, property_name prop_id, tinymath::mat4x4 mat);

		// VB/IB
		resource_id bind_vertex_buffer(const std::vector<Vertex>& buffer);
		resource_id bind_index_buffer(const std::vector<size_t>& buffer);
		void delete_vertex_buffer(resource_id id);
		void delete_index_buffer(resource_id id);
		void use_vertex_buffer(resource_id id);
		void use_index_buffer(resource_id id);

		// rt
		RenderTexture* get_active_rendertexture() const ;
		tinymath::color_rgba* get_active_color_buffer() const { return get_active_rendertexture()->get_color_buffer_ptr(); }
		tinymath::color_rgba* get_target_color_buffer() const  { return target_rendertexture->get_color_buffer_ptr(); }
		void set_active_rendertexture(resource_id id);
		void reset_active_rendertexture() ;
		
		// msaa
		void set_subsample_count(uint8_t multiplier);
		void set_multisample_frequency(MultiSampleFrequency frequency);
		uint8_t get_subsample_count() { return target_rendertexture->get_subsample_count(); }

		// state flag
		void enable_flag(PipelineFeature flag);
		void disable_flag(PipelineFeature flag);
		bool is_flag_enabled(PipelineFeature flag);
		static bool is_flag_enabled(const GraphicsContext& ctx, PipelineFeature flag);

		// stencil
		void set_stencil_func(CompareFunc func);
		void set_stencil_op(StencilOp pass_op, StencilOp fail_op, StencilOp zfail_op);
		void set_stencil_mask(stencil_t ref_val, stencil_t write_mask, stencil_t read_mask);

		// depth test
		void set_depth_func(CompareFunc func);

		// blend
		void set_blend_func(BlendFunc func);
		void set_blend_factor(BlendFactor src_factor, BlendFactor dst_factor);

		// culling
		void set_front_face(VertexOrder order);
		void set_cull_face(FaceCulling face_culling);

		// colormask
		void set_color_mask(ColorMask mask);

		// others
		size_t get_width() { return target_rendertexture->get_width(); }
		size_t get_height() { return target_rendertexture->get_height(); }
		
		// segment drawer
		void draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& v, const tinymath::mat4x4& p, const tinymath::vec2f& screen_translation);
		void draw_screen_segment(const tinymath::vec4f& start, const tinymath::vec4f& end, const tinymath::Color& col);
		void draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& mvp);
		void draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void draw_segment(const tinymath::vec4f& clip_start, const tinymath::vec4f& clip_end, const tinymath::Color& col);
		void draw_coordinates(const tinymath::vec3f& pos, const tinymath::vec3f& forward, const tinymath::vec3f& up, const tinymath::vec3f& right, const tinymath::mat4x4& v, const tinymath::mat4x4& p, const tinymath::vec2f& offset);
		void draw_coordinates(const tinymath::vec3f& pos, const tinymath::vec3f& forward, const tinymath::vec3f& up, const tinymath::vec3f& right, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void draw_coordinates(const tinymath::vec3f& pos, const tinymath::vec3f& forward, const tinymath::vec3f& up, const tinymath::vec3f& right, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p);

	private:
		void input2vertex(const GraphicsContext& context, const Vertex& v1, const Vertex& v2, const Vertex& v3);
		void clip2raster(const GraphicsContext& context, const Vertex& c1, const Vertex& c2, const Vertex& c3);
		void rasterize_tile(const tinymath::Rect& rect, SafeQueue<TileTask>& task_queue);
		void resolve_tile(const tinymath::Rect& rect, SafeQueue<TileTask>& task_queue);
		void rasterize(const tinymath::Rect& rect, const GraphicsContext& context, const Triangle& tri);
		void rasterize_pixel_block(const Triangle& tri, 
								   const GraphicsContext& context,
								   const RenderTexture& rt, 
								   const Pixel& px1,
								   const Pixel& px2,
								   const Pixel& px3,
								   const Pixel& px4,
								   SubsampleParam& p1,
								   SubsampleParam& p2,
								   SubsampleParam& p3,
								   SubsampleParam& p4);
		void rasterize(const Triangle& tri, const GraphicsContext& context, RasterizerStrategy strategy);
		void scanblock(const Triangle& tri, const GraphicsContext& context);
		void scanline(const Triangle& tri, const GraphicsContext& context);
		bool validate_fragment(PipelineFeature op_pass) const;
		void resize(size_t w, size_t h);

		bool fragment_stage(FrameBuffer& rt, const Vertex& v, const Vertex& ddx, const Vertex& ddy, size_t row, size_t col, const GraphicsContext& context);
		bool multisample_fragment_stage(FrameBuffer& rt, const Vertex& v, const Vertex& ddx, const Vertex& ddy, size_t row, size_t col, const GraphicsContext& context, SubsampleParam& subsample_param);

	public:
		GraphicsStatistic statistics;
		bool tile_based;
		bool multi_thread;

	private:
		std::unique_ptr<RenderTexture> target_rendertexture; // glfw use double buffering by default, so only one frame buffer is needed
		std::vector<GraphicsContext> contexts;
		
		GraphicsContext context;
		resource_id active_frame_buffer_id;

		// VB/IB
		std::vector<std::shared_ptr<RenderTexture>> rendertextures;
		std::vector<std::vector<Vertex>> vertex_buffer_table;
		std::vector<std::vector<size_t>> index_buffer_table;

		std::vector<ShaderProgram*> shader_programs;

		bool msaa_dirty;
	};
}
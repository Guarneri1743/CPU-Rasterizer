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
	struct GraphicsCommand;

	class GraphicsDevice
	{
	public:
		GraphicsDevice();
		~GraphicsDevice();

		// misc
		void set_viewport(size_t x, size_t y, size_t w, size_t h);
		void submit_primitive(ShaderProgram* shader, const Vertex& v1, const Vertex& v2, const Vertex& v3);
		void fence_primitives();
		void fence_pixels();
		void clear_buffer(FrameContent flag);
		void set_clear_color(const tinymath::Color color);
		bool try_alloc_id(uint32_t& id);
		uint32_t create_buffer(size_t width, size_t height, FrameContent content);
		bool get_buffer(uint32_t id, std::shared_ptr<RenderTexture>& buffer) const;

		// rt
		RenderTexture* get_active_rendertexture() const ;
		tinymath::color_rgba* get_target_color_buffer() const  { return target_rendertexture->get_color_buffer_ptr(); }
		void set_active_rendertexture(uint32_t id);
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
		void draw(const GraphicsCommand& task);
		void input2raster(const ShaderProgram& shader, const GraphicsContext& context, const Vertex& v1, const Vertex& v2, const Vertex& v3);
		void clip2raster(const ShaderProgram& shader, const GraphicsContext& context, const Vertex& c1, const Vertex& c2, const Vertex& c3);
		void rasterize_tile(const tinymath::Rect& rect, SafeQueue<TileTask>& task_queue);
		void resolve_tile(const tinymath::Rect& rect, SafeQueue<TileTask>& task_queue);
		void rasterize(const tinymath::Rect& rect, const Triangle& tri, const ShaderProgram& shader, const GraphicsContext& context);
		void rasterize_pixel_block(const Triangle& tri, 
								   const ShaderProgram& shader, 
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
		void rasterize(const Triangle& tri, const ShaderProgram& shader, const GraphicsContext& context, RasterizerStrategy strategy);
		void scanblock(const Triangle& tri, const ShaderProgram& shader, const GraphicsContext& context);
		void scanline(const Triangle& tri, const ShaderProgram& shader, const GraphicsContext& context);
		bool fragment_stage(FrameBuffer& rt, const Vertex& v, const Vertex& ddx, const Vertex& ddy, size_t row, size_t col, 
							  const ShaderProgram& shader, const GraphicsContext& context);
		bool multisample_fragment_stage(FrameBuffer& rt, const Vertex& v, const Vertex& ddx, const Vertex& ddy, size_t row, size_t col,
							  const ShaderProgram& shader, const GraphicsContext& context, SubsampleParam& subsample_param);
		bool validate_fragment(PipelineFeature op_pass) const;
		void resize(size_t w, size_t h);

	public:
		GraphicsStatistic statistics;
		bool tile_based;
		bool multi_thread;

	private:
		std::unique_ptr<RenderTexture> target_rendertexture; // glfw use double buffering by default, so only one frame buffer is needed
		std::unordered_map<uint32_t, std::shared_ptr<RenderTexture>> frame_buffer_map;
		std::vector<GraphicsCommand> primitive_commands;
		GraphicsContext context;
		uint32_t active_frame_buffer_id;

		bool msaa_dirty;
	};
}
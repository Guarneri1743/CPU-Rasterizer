#ifndef _GRAPHICS_DEVICE_
#define _GRAPHICS_DEVICE_
#include <stdint.h>
#include <queue>
#include <memory>
#include "Marcos.h"
#include "Define.hpp"
#include "RawBuffer.hpp"
#include "GraphicsCommand.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Matrix4x4.hpp"
#include "Rect.hpp"
#include "Color.hpp"
#include "Triangle.hpp"
#include "FrameTile.hpp"

namespace Guarneri
{
	class GraphicsDevice
	{
	public:
		GraphicsStatistic statistics;
		bool tile_based;
		bool multi_thread;

	private:
		// use 32 bits zbuffer here, for convenience 
		std::unique_ptr<RawBuffer<float>> zbuffer;
		// 32 bits bgra framebuffer, 8-bit per channel
		std::unique_ptr<RawBuffer<color_bgra>> framebuffer;
		// 8 bits stencil buffer
		std::unique_ptr<RawBuffer<uint8_t>> stencilbuffer;
		// msaa buffer
		std::unique_ptr<RawBuffer<uint8_t>> msaa_stencilbuffer;
		std::unique_ptr<RawBuffer<uint8_t>> msaa_coveragebuffer;
		std::unique_ptr<RawBuffer<float>> msaa_zbuffer;
		std::unique_ptr<RawBuffer<color_bgra>> msaa_colorbuffer;
		// shadowmap
		std::unique_ptr<RawBuffer<float>> shadowmap;
		// commands
		std::queue<GraphicsCommand*> commands;
		// framebuffer tiles
		FrameTile* tiles;
		int width;
		int height;
		uint32_t row_tile_count;
		uint32_t col_tile_count;
		uint32_t tile_length;
		uint8_t msaa_subsample_count;
		uint8_t subsamples_per_axis;

	public:
		GraphicsDevice();
		~GraphicsDevice();
		void initialize(void* bitmap_handle, uint32_t w, uint32_t h);
		void draw(Shader* shader, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p);
		void present();
		void clear_buffer(const BufferFlag& flag);
		void set_subsample_count(const uint8_t& multiplier);
		uint8_t* get_framebuffer() { int size;  return reinterpret_cast<uint8_t*>(framebuffer->get_ptr(size)); };

	public:
		void draw_segment(const Vector3& start, const Vector3& end, const Color& col, const Matrix4x4& v, const Matrix4x4& p, const Vector2& screen_translation);
		void draw_screen_segment(const Vector4& start, const Vector4& end, const Color& col);
		void draw_segment(const Vector3& start, const Vector3& end, const Color& col, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p);
		void draw_segment(const Vector3& start, const Vector3& end, const Color& col, const Matrix4x4& v, const Matrix4x4& p);
		void draw_segment(const Vector4& clip_start, const Vector4& clip_end, const Color& col);
		void draw_coordinates(const Vector3& pos, const Vector3& forward, const Vector3& up, const Vector3& right, const Matrix4x4& v, const Matrix4x4& p, const Vector2& offset);
		void draw_coordinates(const Vector3& pos, const Vector3& forward, const Vector3& up, const Vector3& right, const Matrix4x4& v, const Matrix4x4& p);
		void draw_coordinates(const Vector3& pos, const Vector3& forward, const Vector3& up, const Vector3& right, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p);

	public:
		TileInfo get_tile_info();
		uint8_t get_subsample_count() { return msaa_subsample_count; }
		RawBuffer<float>* get_shadowmap();

	private:
		void draw_triangle(Shader* shader, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p);
		void render_tiles();
		void msaa_resolve();
		void process_commands();
		void rasterize_tiles(const size_t& start, const size_t& end);
		void rasterize_tile(FrameTile& tile);
		void resolve_tiles(const size_t& start, const size_t& end);
		void resolve_tile(FrameTile& tile);
		void execute_task(const FrameTile& tile, const Triangle& tri, Shader* shader);
		void rasterize(const Triangle& tri, Shader* shader, const RasterizerStrategy& strategy);
		void scanblock(const Triangle& tri, Shader* shader);
		void scanline(const Triangle& tri, Shader* shader);
		v2f process_vertex(Shader* shader, const Vertex& vert) const;
		void process_subsamples(RawBuffer<color_bgra>* fbuf, RawBuffer<float>* zbuf, RawBuffer<uint8_t>* stencilbuf, const uint32_t& row, const uint32_t& col, Shader* shader, const Vertex& v0, const Vertex& v1, const Vertex& v2, const float& area);
		void process_fragment(RawBuffer<color_bgra>* fbuf, RawBuffer<float>* zbuf, RawBuffer<uint8_t>* stencilbuf, const Vertex& v, const uint32_t& row, const uint32_t& col, Shader* shader);
		bool validate_fragment(const PerSampleOperation& op_pass) const;
		bool perform_stencil_test(RawBuffer<uint8_t>* stencilbuf, const uint8_t& ref_val, const uint8_t& read_mask, const CompareFunc& func, const uint32_t& row, const uint32_t& col) const;
		void update_stencil_buffer(RawBuffer<uint8_t>* stencilbuf, const uint32_t& row, const uint32_t& col, const PerSampleOperation& op_pass, const StencilOp& stencil_pass_op, const StencilOp& stencil_fail_op, const StencilOp& stencil_zfail_op, const uint8_t& ref_val) const;
		bool perform_depth_test(RawBuffer<float>* zbuf, const CompareFunc& func, const uint32_t& row, const uint32_t& col, const float& z) const;
		Color blend(const Color& src_color, const Color& dst_color, const BlendFactor& src_factor, const BlendFactor& dst_factor, const BlendOp& op);
		Vertex clip2ndc(const Vertex& v) const;
		Vector4 clip2ndc(const Vector4& v) const;
		Vertex ndc2viewport(const Vertex& v) const;
		Vector4 ndc2viewport(const Vector4& v) const;
		float linearize_depth(const float& depth, const float& near, const float& far) const;
		float linearize_01depth(const float& depth, const float& near, const float& far) const;
	};
}
#endif
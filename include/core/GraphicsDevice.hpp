#pragma once
#include <stdint.h>
#include <queue>
#include <memory>
#include "Marcos.h"
#include "Define.hpp"
#include "RawBuffer.hpp"
#include "GraphicsCommand.hpp"
#include "tinymath.h"
#include "Rect.hpp"
#include "Color.hpp"
#include "Triangle.hpp"
#include "FrameTile.hpp"
#include <vector>
#include <future>

namespace Guarneri
{
	struct DrawCommand
	{
		Shader* shader;
		Vertex v1;
		Vertex v2;
		Vertex v3;
		tinymath::mat4x4 m;
		tinymath::mat4x4 v;
		tinymath::mat4x4 p;
	};

	class GraphicsDevice
	{
	public:
		GraphicsStatistic statistics;
		bool tile_based;
		bool multi_thread;

	private:
		// use 32 bits zbuffer here, for convenience 
		std::unique_ptr<RawBuffer<float>> zbuffer;
		// 32 bits rgba framebuffer, 8-bit per channel
		std::unique_ptr<RawBuffer<color_rgba>> framebuffer;
		// 8 bits stencil buffer
		std::unique_ptr<RawBuffer<uint8_t>> stencilbuffer;
		// msaa buffer
		std::unique_ptr<RawBuffer<uint8_t>> msaa_stencilbuffer;
		std::unique_ptr<RawBuffer<uint8_t>> msaa_coveragebuffer;
		std::unique_ptr<RawBuffer<float>> msaa_zbuffer;
		std::unique_ptr<RawBuffer<color_rgba>> msaa_colorbuffer;
		// shadowmap
		std::unique_ptr<RawBuffer<float>> shadowmap;
		// draw commands
		std::vector<DrawCommand> draw_commands;
		// general commands
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
		void resize(uint32_t w, uint32_t h);
		void initialize(uint32_t w, uint32_t h);
		void submit_draw_command(Shader* shader, const Vertex& v1, const Vertex& v2, const Vertex& v3, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void fence_draw_commands();
		void present();
		void clear_buffer(const BufferFlag& flag);
		void set_subsample_count(const uint8_t& multiplier);
		color_rgba* get_framebuffer() { size_t size;  return framebuffer->get_ptr(size); };
		int get_width() { return width; }
		int get_height() { return height; }

	public:
		void draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const Color& col, const tinymath::mat4x4& v, const tinymath::mat4x4& p, const tinymath::vec2f& screen_translation);
		void draw_screen_segment(const tinymath::vec4f& start, const tinymath::vec4f& end, const Color& col);
		void draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const Color& col, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const Color& col, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void draw_segment(const tinymath::vec4f& clip_start, const tinymath::vec4f& clip_end, const Color& col);
		void draw_coordinates(const tinymath::vec3f& pos, const tinymath::vec3f& forward, const tinymath::vec3f& up, const tinymath::vec3f& right, const tinymath::mat4x4& v, const tinymath::mat4x4& p, const tinymath::vec2f& offset);
		void draw_coordinates(const tinymath::vec3f& pos, const tinymath::vec3f& forward, const tinymath::vec3f& up, const tinymath::vec3f& right, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void draw_coordinates(const tinymath::vec3f& pos, const tinymath::vec3f& forward, const tinymath::vec3f& up, const tinymath::vec3f& right, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p);

	public:
		TileInfo get_tile_info();
		uint8_t get_subsample_count() { return msaa_subsample_count; }
		RawBuffer<float>* get_shadowmap();

	private:
		void draw(DrawCommand task);
		void input2raster(const Shader& shader, const Vertex& v1, const Vertex& v2, const Vertex& v3);
		void clip2raster(const Shader& shader, const Vertex& c1, const Vertex& c2, const Vertex& c3);
		void rasterize_tiles(const size_t& start, const size_t& end);
		void rasterize_tile(FrameTile& tile);
		void resolve_tiles(const size_t& start, const size_t& end);
		void resolve_tile(FrameTile& tile);
		void rasterize(const FrameTile& tile, const Triangle& tri, const Shader& shader);
		void rasterize(const Triangle& tri, const Shader& shader, const RasterizerStrategy& strategy);
		void scanblock(const Triangle& tri, const Shader& shader);
		void scanline(const Triangle& tri, const Shader& shader);
		v2f process_vertex(const Shader& shader, const Vertex& vert) const;
		void process_subsamples(RawBuffer<color_rgba>* fbuf, RawBuffer<float>* zbuf, RawBuffer<uint8_t>* stencilbuf, const uint32_t& row, const uint32_t& col, const Shader& shader, const Vertex& v0, const Vertex& v1, const Vertex& v2, const float& area);
		void process_fragment(RawBuffer<color_rgba>* fbuf, RawBuffer<float>* zbuf, RawBuffer<uint8_t>* stencilbuf, const Vertex& v, const uint32_t& row, const uint32_t& col, const Shader& shader);
		bool validate_fragment(const PerSampleOperation& op_pass) const;
		bool perform_stencil_test(RawBuffer<uint8_t>* stencilbuf, const uint8_t& ref_val, const uint8_t& read_mask, const CompareFunc& func, const uint32_t& row, const uint32_t& col) const;
		void update_stencil_buffer(RawBuffer<uint8_t>* stencilbuf, const uint32_t& row, const uint32_t& col, const PerSampleOperation& op_pass, const StencilOp& stencil_pass_op, const StencilOp& stencil_fail_op, const StencilOp& stencil_zfail_op, const uint8_t& ref_val) const;
		bool perform_depth_test(RawBuffer<float>* zbuf, const CompareFunc& func, const uint32_t& row, const uint32_t& col, const float& z) const;
		Color blend(const Color& src_color, const Color& dst_color, const BlendFactor& src_factor, const BlendFactor& dst_factor, const BlendOp& op);
		float linearize_depth(const float& depth, const float& near, const float& far) const;
		float linearize_01depth(const float& depth, const float& near, const float& far) const;
		void process_commands();
	};
}
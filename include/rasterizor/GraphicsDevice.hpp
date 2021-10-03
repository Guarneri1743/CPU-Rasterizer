#pragma once
#include <stdint.h>
#include <queue>
#include <memory>
#include "Marcos.h"
#include "Define.hpp"
#include "RawBuffer.hpp"
#include "GraphicsCommand.hpp"
#include "tinymath.h"
#include "Triangle.hpp"
#include "FrameTile.hpp"
#include "FrameBuffer.hpp"
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
		std::unique_ptr<FrameBuffer> frame_buffer; // glfw use double buffering by default, so only one frame buffer is needed
		std::unique_ptr<FrameBuffer> msaa_buffer;
		std::unordered_map<uint32_t, std::shared_ptr<FrameBuffer>> frame_buffer_map;
		// draw commands
		std::vector<DrawCommand> draw_commands;
		// general commands
		std::queue<GraphicsCommand*> commands;
		// framebuffer tiles
		FrameTile* tiles;
		size_t row_tile_count;
		size_t col_tile_count;
		size_t tile_length;
		uint8_t msaa_subsample_count;
		uint8_t subsamples_per_axis;
		uint32_t active_frame_buffer_id;

	public:
		GraphicsDevice();
		~GraphicsDevice();
		void resize(size_t w, size_t h);
		void resize_tiles(size_t w, size_t h);
		void initialize(size_t w, size_t h);
		void submit_draw_command(Shader* shader, const Vertex& v1, const Vertex& v2, const Vertex& v3, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void fence_draw_commands();
		void present();
		void clear_buffer(const FrameContent& flag);
		void set_subsample_count(const uint8_t& multiplier);
		FrameBuffer* get_active_framebuffer() const noexcept;
		tinymath::color_rgba* get_framebuffer() const noexcept { return frame_buffer->get_color_buffer_ptr(); }
		void set_active_frame_buffer(uint32_t& id);
		void reset_active_frame_buffer() noexcept;
		uint32_t create_buffer(const size_t& width, const size_t& height, const FrameContent& content) noexcept;
		bool get_buffer(const uint32_t& id, std::shared_ptr<FrameBuffer>& buffer) const noexcept;

		size_t get_width() { return frame_buffer->get_width(); }
		size_t get_height() { return frame_buffer->get_height(); }

	public:
		void draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& v, const tinymath::mat4x4& p, const tinymath::vec2f& screen_translation);
		void draw_screen_segment(const tinymath::vec4f& start, const tinymath::vec4f& end, const tinymath::Color& col);
		void draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void draw_segment(const tinymath::vec3f& start, const tinymath::vec3f& end, const tinymath::Color& col, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void draw_segment(const tinymath::vec4f& clip_start, const tinymath::vec4f& clip_end, const tinymath::Color& col);
		void draw_coordinates(const tinymath::vec3f& pos, const tinymath::vec3f& forward, const tinymath::vec3f& up, const tinymath::vec3f& right, const tinymath::mat4x4& v, const tinymath::mat4x4& p, const tinymath::vec2f& offset);
		void draw_coordinates(const tinymath::vec3f& pos, const tinymath::vec3f& forward, const tinymath::vec3f& up, const tinymath::vec3f& right, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void draw_coordinates(const tinymath::vec3f& pos, const tinymath::vec3f& forward, const tinymath::vec3f& up, const tinymath::vec3f& right, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p);

	public:
		TileInfo get_tile_info();
		uint8_t get_subsample_count() { return msaa_subsample_count; }

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
		void process_subsamples(FrameBuffer& frame_buffer, const size_t& row, const size_t& col, const Shader& shader, const Vertex& v0, const Vertex& v1, const Vertex& v2, const float& area);
		void process_fragment(FrameBuffer& frame_buffer, const Vertex& v, const size_t& row, const size_t& col, const Shader& shader);
		bool validate_fragment(const PerSampleOperation& op_pass) const;
		tinymath::Color blend(const tinymath::Color& src_color, const tinymath::Color& dst_color, const BlendFactor& src_factor, const BlendFactor& dst_factor, const BlendOp& op);
		void process_commands();
	};
}
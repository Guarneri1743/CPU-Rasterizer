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
#include "RenderTexture.hpp"
#include "TileBasedManager.hpp"
#include <vector>
#include <future>

namespace Guarneri
{
	struct SubsampleParam
	{
		tinymath::color_rgba pixel_color;
		bool pixel_color_calculated;
	};

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
		std::unique_ptr<TileBasedManager> tile_based_manager;
		std::unique_ptr<RenderTexture> target_rendertexture; // glfw use double buffering by default, so only one frame buffer is needed
		std::unordered_map<uint32_t, std::shared_ptr<RenderTexture>> frame_buffer_map;
		std::vector<DrawCommand> draw_commands;
		std::queue<GraphicsCommand*> commands;
		uint32_t active_frame_buffer_id;

	public:
		GraphicsDevice();
		~GraphicsDevice();
		void resize(size_t w, size_t h);
		void initialize(size_t w, size_t h);
		void submit_draw_command(Shader* shader, const Vertex& v1, const Vertex& v2, const Vertex& v3, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void fence_draw_commands();
		void present();
		void clear_buffer(const FrameContent& flag);
		void set_subsample_count(const uint8_t& multiplier);
		RenderTexture* get_active_rendertexture() const noexcept;
		tinymath::color_rgba* get_target_color_buffer() const noexcept { return target_rendertexture->get_color_buffer_ptr(); }
		void set_active_rendertexture(uint32_t& id);
		void reset_active_rendertexture() noexcept;
		uint32_t create_buffer(const size_t& width, const size_t& height, const FrameContent& content) noexcept;
		bool get_buffer(const uint32_t& id, std::shared_ptr<RenderTexture>& buffer) const noexcept;

		size_t get_width() { return target_rendertexture->get_width(); }
		size_t get_height() { return target_rendertexture->get_height(); }

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
		uint8_t get_subsample_count() { return target_rendertexture->get_subsample_count(); }

	private:
		void draw(DrawCommand task);
		void input2raster(const Shader& shader, const Vertex& v1, const Vertex& v2, const Vertex& v3);
		void clip2raster(const Shader& shader, const Vertex& c1, const Vertex& c2, const Vertex& c3);
		void rasterize_tile(const tinymath::Rect& rect, SafeQueue<TileTask>& task_queue);
		void resolve_tile(const tinymath::Rect& rect, SafeQueue<TileTask>& task_queue);
		void rasterize(const tinymath::Rect& rect, const Triangle& tri, const Shader& shader);
		void rasterize_pixel_block(const Triangle& tri, 
								   const Shader& shader, 
								   const RenderTexture& rt, 
								   const Pixel& px1,
								   const Pixel& px2,
								   const Pixel& px3,
								   const Pixel& px4,
								   SubsampleParam& p1,
								   SubsampleParam& p2,
								   SubsampleParam& p3,
								   SubsampleParam& p4);
		void rasterize(const Triangle& tri, const Shader& shader, const RasterizerStrategy& strategy);
		void scanblock(const Triangle& tri, const Shader& shader);
		void scanline(const Triangle& tri, const Shader& shader);
		v2f process_vertex(const Shader& shader, const Vertex& vert) const;
		bool process_fragment(FrameBuffer& rt, const Vertex& v, const Vertex& ddx, const Vertex& ddy, const size_t& row, const size_t& col, const Shader& shader);
		bool process_fragment(FrameBuffer& rt, const Vertex& v, const Vertex& ddx, const Vertex& ddy, const size_t& row, const size_t& col, const Shader& shader, SubsampleParam& subsample_param);
		bool validate_fragment(const PerSampleOperation& op_pass) const;
		void process_commands();
	};
}
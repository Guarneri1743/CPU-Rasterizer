#pragma once
#include <memory>
#include <functional>
#include "tinymath.h"
#include "Define.hpp"
#include "FrameBuffer.hpp"

namespace CpuRasterizor
{
	struct Pixel
	{
		size_t row;
		size_t col;
		tinymath::vec2f pos;
	};

	struct PixelBlock
	{
		Pixel top_left;
		Pixel top_right;
		Pixel bottom_left;
		Pixel bottom_right; 
	};

	class RenderTexture
	{
	public:
		RenderTexture(const size_t& w, const size_t& h, const FrameContent& flag);
		RenderTexture(const size_t& w, const size_t& h, const FrameContent& flag, bool msaa_on, uint8_t subsample_count);

		void foreach_pixel(const tinymath::Rect& rect, std::function<void(RenderTexture& buffer, const Pixel& pixel)> pixel_func);
		void foreach_pixel(std::function<void(RenderTexture& buffer, const Pixel& pixel)> pixel_func);

		void foreach_pixel_block (const tinymath::Rect& rect, std::function<void(RenderTexture& buffer, const PixelBlock& pixel)> pixel_block_func);

		Pixel get_subpixel(const size_t& row, const size_t& col, const uint8_t& x_subsample_idx, const uint8_t& y_subsample_idx);

		void clear(const FrameContent& flag);
		void set_clear_color(const tinymath::color_rgba color);

		void resize(const size_t& w, const size_t& h);
		size_t get_width() const noexcept { return framebuffer->get_width(); }
		size_t get_height() const noexcept { return framebuffer->get_height(); }
		void get_size(size_t& w, size_t& h) const noexcept { w = framebuffer->get_width(); h = framebuffer->get_height(); }

		RawBuffer<tinymath::color_rgba>* get_color_raw_buffer() const noexcept { return framebuffer->get_color_raw_buffer(); }
		tinymath::color_rgba* get_color_buffer_ptr() const noexcept { return framebuffer->get_color_buffer_ptr(); };
		
		RawBuffer<tinymath::color_rgba>* get_msaa_color_raw_buffer() const noexcept { return msaa_framebuffer->get_color_raw_buffer(); }
		tinymath::color_rgba* get_msaa_color_buffer_ptr() const noexcept { return msaa_framebuffer->get_color_buffer_ptr(); };

		FrameBuffer* get_framebuffer() const noexcept { return framebuffer.get(); };
		FrameBuffer* get_msaa_framebuffer() const noexcept { return msaa_framebuffer.get(); }

		void set_msaa_param(bool msaa_on, uint8_t subsample_count);
		uint8_t get_subsample_count() { return msaa_subsample_count; }
		uint8_t get_subsamples_per_axis() { return subsamples_per_axis; }

	private:
		void reset_msaa_buffer();

	public:
		MultiSampleFrequency multi_sample_frequency; // here I use pixel frequency by default

	private:
		std::unique_ptr<FrameBuffer> framebuffer;
		std::unique_ptr<FrameBuffer> msaa_framebuffer;

		bool has_msaa_buffer;
		uint8_t msaa_subsample_count;
		uint8_t subsamples_per_axis;
	};
}
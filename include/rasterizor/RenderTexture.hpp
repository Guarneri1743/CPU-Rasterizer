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
		RenderTexture(size_t w, size_t h, FrameContent flag);
		RenderTexture(size_t w, size_t h, FrameContent flag, bool msaa_on, uint8_t subsample_count);

		void foreach_pixel(const tinymath::Rect& rect, std::function<void(RenderTexture& buffer, const Pixel& pixel)> pixel_func);
		void foreach_pixel(std::function<void(RenderTexture& buffer, const Pixel& pixel)> pixel_func);

		void foreach_pixel_block (const tinymath::Rect& rect, std::function<void(RenderTexture& buffer, const PixelBlock& pixel)> pixel_block_func);

		Pixel get_subpixel(size_t row, size_t col, uint8_t x_subsample_idx, uint8_t y_subsample_idx);

		void clear(FrameContent flag);
		void set_clear_color(const tinymath::color_rgba color);

		void resize(size_t w, size_t h);
		size_t get_width() const  { return framebuffer->get_width(); }
		size_t get_height() const  { return framebuffer->get_height(); }
		void get_size(size_t& w, size_t& h) const  { w = framebuffer->get_width(); h = framebuffer->get_height(); }

		RawBuffer<tinymath::color_rgba>* get_color_raw_buffer() const  { return framebuffer->get_color_raw_buffer(); }
		tinymath::color_rgba* get_color_buffer_ptr() const  { return framebuffer->get_color_buffer_ptr(); };
		
		RawBuffer<tinymath::color_rgba>* get_msaa_color_raw_buffer() const  { return msaa_framebuffer->get_color_raw_buffer(); }
		tinymath::color_rgba* get_msaa_color_buffer_ptr() const  { return msaa_framebuffer->get_color_buffer_ptr(); };

		FrameBuffer* get_framebuffer() const  { return framebuffer.get(); };
		FrameBuffer* get_msaa_framebuffer() const  { return msaa_framebuffer.get(); }

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
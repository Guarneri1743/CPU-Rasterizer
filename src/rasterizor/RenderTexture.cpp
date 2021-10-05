#include "RenderTexture.hpp"
#include "Sampling.hpp"

namespace Guarneri
{
	RenderTexture::RenderTexture(const size_t& w, const size_t& h, const FrameContent& flag, bool msaa_on, uint8_t subsample_count) :
		has_msaa_buffer(msaa_on),
		msaa_subsample_count(tinymath::round_up_to_power_of_two(subsample_count)),
		multi_sample_frequency(MultiSampleFrequency::kPixelFrequency)
	{
		framebuffer = std::make_unique<FrameBuffer>(w, h, flag & ~FrameContent::kCoverage);
		reset_msaa_buffer();
	}

	RenderTexture::RenderTexture(const size_t& w, const size_t& h, const FrameContent& flag) : RenderTexture(w, h, flag, false, 0) {}

	void RenderTexture::foreach_pixel(const tinymath::Rect& rect, std::function<void(RenderTexture& buffer, const Pixel& pixel)> pixel_func)
	{
		for (size_t row = rect.min().y; row < rect.max().y; row++)
		{
			for (size_t col = rect.min().x; col < rect.max().x; col++)
			{
				tinymath::vec2f pos((float)col + 0.5f, (float)row + 0.5f);
				Pixel pixel = { row, col, pos };
				pixel_func(*this, pixel);
			}
		}
	}

	void RenderTexture::foreach_pixel(std::function<void(RenderTexture& buffer, const Pixel& pixel)> pixel_func)
	{
		for (size_t row = 0ull; row < (size_t)framebuffer->get_height(); row++)
		{
			for (size_t col = 0ull; col < (size_t)framebuffer->get_width(); col++)
			{
				tinymath::vec2f pos((float)col + 0.5f, (float)row + 0.5f);
				Pixel pixel = { row, col, pos };
				pixel_func(*this, pixel);
			}
		}
	}

	void RenderTexture::foreach_pixel_block(const tinymath::Rect& rect, std::function<void(RenderTexture& buffer, const PixelBlock& pixel)> pixel_block_func)
	{
		assert(rect.size().x % 2 == 0 && rect.size().y % 2 == 0);

		for (size_t row = rect.min().y; row < rect.max().y; row += 2)
		{
			for (size_t col = rect.min().x; col < rect.max().x; col += 2)
			{
				tinymath::vec2f p1((float)col + 0.5f, (float)row + 0.5f);
				Pixel top_left = { row, col, p1 };

				tinymath::vec2f p2((float)col + 1.5f, (float)row + 0.5f);
				Pixel top_right = { row, col + 1, p2 };

				tinymath::vec2f p3((float)col + 0.5f, (float)row + 1.5f);
				Pixel bottom_left = { row + 1, col, p3 };

				tinymath::vec2f p4((float)col + 1.5f, (float)row + 1.5f);
				Pixel bottom_right = { row + 1, col + 1, p4 };

				PixelBlock block = { top_left, top_right, bottom_left, bottom_right };

				pixel_block_func(*this, block);
			}
		}
	}

	Pixel RenderTexture::get_subpixel(const size_t& row, const size_t& col, const uint8_t& x_subsample_idx, const uint8_t& y_subsample_idx)
	{
		uint8_t index = x_subsample_idx * subsamples_per_axis + y_subsample_idx;
		size_t subsample_row = (size_t)(row * subsamples_per_axis + x_subsample_idx);
		size_t subsample_col = (size_t)(col * subsamples_per_axis + y_subsample_idx);
		auto local_pos = hammersley((uint32_t)index, (uint32_t)msaa_subsample_count);
		tinymath::vec2f screen_pos((float)col + local_pos.x, (float)row + local_pos.y);
		Pixel subpixel = { subsample_row, subsample_col, screen_pos };
		return subpixel;
	}

	void RenderTexture::clear(const FrameContent& flag)
	{
		framebuffer->clear(flag & ~FrameContent::kCoverage);
		if (has_msaa_buffer)
		{
			msaa_framebuffer->clear(flag | FrameContent::kCoverage);
		}
	}

	void RenderTexture::resize(const size_t& w, const size_t& h)
	{
		framebuffer->resize(w, h);
		reset_msaa_buffer();
	}

	void RenderTexture::set_msaa_param(bool msaa_on, uint8_t subsample_count)
	{
		uint8_t target_subsample_count = tinymath::round_up_to_power_of_two(subsample_count);
		if (msaa_on != has_msaa_buffer || (msaa_on && target_subsample_count != msaa_subsample_count))
		{
			msaa_subsample_count = target_subsample_count;
			subsamples_per_axis = tinymath::sqrt(msaa_subsample_count);
			has_msaa_buffer = msaa_on;
			reset_msaa_buffer();
		}
	}

	void RenderTexture::reset_msaa_buffer()
	{
		if (has_msaa_buffer)
		{
			if (msaa_framebuffer == nullptr)
			{
				msaa_framebuffer = std::make_unique<FrameBuffer>(framebuffer->get_width() * subsamples_per_axis, framebuffer->get_height() * subsamples_per_axis, framebuffer->get_flag() | FrameContent::kCoverage);
			}
			else if(msaa_framebuffer->get_width() != (framebuffer->get_width() * subsamples_per_axis) || msaa_framebuffer->get_height() != (framebuffer->get_height() * subsamples_per_axis))
			{
				msaa_framebuffer->resize(framebuffer->get_width() * subsamples_per_axis, framebuffer->get_height() * subsamples_per_axis);
			}
		}
		else
		{
			msaa_framebuffer.reset();
		}
	}
}
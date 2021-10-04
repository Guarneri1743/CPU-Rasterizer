#pragma once
#include <memory>
#include <functional>
#include "Define.hpp"
#include "tinymath/tinymath.h"
#include "RawBuffer.hpp"

namespace Guarneri
{
	constexpr float kFarZ = 1.0f;
	constexpr uint8_t kDefaultStencil = 0x00;

	class FrameBuffer
	{
	public:
		FrameBuffer(const size_t& w, const size_t& h, const FrameContent& flag);

		void write_color(const size_t& row, const size_t& col, const tinymath::color_rgba& color);
		void write_depth(const size_t& row, const size_t& col, const float& depth);
		void write_stencil(const size_t& row, const size_t& col, const uint8_t& stencil);
		void write_coverage(const size_t& row, const size_t& col, const uint8_t& coverage);

		bool read_color(const size_t& row, const size_t& col, tinymath::color_rgba& color);
		bool read_depth(const size_t& row, const size_t& col, float& depth);
		bool read_stencil(const size_t& row, const size_t& col, uint8_t& stencil);
		bool read_coverage(const size_t& row, const size_t& col, uint8_t& coverage);

		void write_color(const float& u, const float& v, const tinymath::color_rgba& color);
		void write_depth(const float& u, const float& v, const float& depth);
		void write_stencil(const float& u, const float& v, const uint8_t& stencil);
		void write_coverage(const float& u, const float& v, const uint8_t& coverage);

		bool read_color(const float& u, const float& v, tinymath::color_rgba& color);
		bool read_depth(const float& u, const float& v, float& depth);
		bool read_stencil(const float& u, const float& v, uint8_t& stencil);
		bool read_coverage(const float& u, const float& v, uint8_t& coverage);

		void clear(const FrameContent& flag);

		void resize(const size_t& w, const size_t& h);
		size_t get_width() const noexcept { return width; }
		size_t get_height() const noexcept { return height; }
		void get_size(size_t& w, size_t& h) const noexcept { w = width; h = height; }
		FrameContent get_flag() const { return content_flag; }

		RawBuffer<tinymath::color_rgba>* get_color_raw_buffer() const noexcept { return color_buffer.get(); }
		tinymath::color_rgba* get_color_buffer_ptr() const noexcept { size_t size;  return color_buffer->get_ptr(size); };


		// Pper pixel operations
	
		// stencil test
		bool perform_stencil_test(const uint8_t& ref_val,
								  const uint8_t& read_mask,
								  const CompareFunc& func,
								  const size_t& row,
								  const size_t& col) const;

		void update_stencil_buffer(const size_t& row,
								   const size_t& col,
								   const PerSampleOperation& op_pass,
								   const StencilOp& stencil_pass_op,
								   const StencilOp& stencil_fail_op,
								   const StencilOp& stencil_zfail_op,
								   const uint8_t& ref_val) const;

		// depth test
		bool perform_depth_test(const CompareFunc& func, 
								const size_t& row,
								const size_t& col, 
								const float& z) const;

		// blending
		static tinymath::Color blend(const tinymath::Color& src_color, 
							  const tinymath::Color& dst_color, 
							  const BlendFactor& src_factor, 
							  const BlendFactor& dst_factor, 
							  const BlendOp& op);

	private:
		FrameContent content_flag;
		size_t width;
		size_t height;

		// use 32 bits zbuffer here, for convenience 
		std::unique_ptr<RawBuffer<float>> depth_buffer;
		// 32 bits rgba framebuffer, 8-bit per channel
		std::unique_ptr<RawBuffer<tinymath::color_rgba>> color_buffer;
		// 8 bits stencil buffer
		std::unique_ptr<RawBuffer<uint8_t>> stencil_buffer;
		// coverage buffer
		std::unique_ptr<RawBuffer<uint8_t>> coverage_buffer;
	};
}
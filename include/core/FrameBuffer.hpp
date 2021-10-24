#pragma once
#include <memory>
#include <functional>
#include "Define.hpp"
#include "tinymath.h"
#include "RawBuffer.hpp"

namespace CpuRasterizer
{
	constexpr depth_t kFarZ = 1.0f;
	constexpr stencil_t kDefaultStencil = 0x00;

	template<typename T>
	class RawBuffer;

	class FrameBuffer
	{
	public:
		FrameBuffer(size_t w, size_t h, FrameContent flag);

		void write_color(size_t row, size_t col, const tinymath::color_rgba& color);
		void write_depth(size_t row, size_t col, depth_t depth);
		void write_stencil(size_t row, size_t col, stencil_t stencil);
		void write_coverage(size_t row, size_t col, coverage_t coverage);

		bool read_color(size_t row, size_t col, tinymath::color_rgba& color);
		bool read_depth(size_t row, size_t col, depth_t& depth);
		bool read_stencil(size_t row, size_t col, stencil_t& stencil);
		bool read_coverage(size_t row, size_t col, coverage_t& coverage);

		void write_color(float u, float v, const tinymath::color_rgba& color);
		void write_depth(float u, float v, depth_t depth);
		void write_stencil(float u, float v, stencil_t stencil);
		void write_coverage(float u, float v, coverage_t coverage);

		bool read_color(float u, float v, tinymath::color_rgba& color);
		bool read_depth(float u, float v, depth_t& depth);
		bool read_stencil(float u, float v, stencil_t& stencil);
		bool read_coverage(float u, float v, coverage_t& coverage);

		void clear(FrameContent flag);
		void set_clear_color(const tinymath::color_rgba& color);

		void resize(size_t w, size_t h);
		size_t get_width() const  { return width; }
		size_t get_height() const  { return height; }
		void get_size(size_t& w, size_t& h) const  { w = width; h = height; }
		FrameContent get_flag() const { return content_flag; }

		RawBuffer<tinymath::color_rgba>* get_color_raw_buffer() const ;
		tinymath::color_rgba* get_color_buffer_ptr() const ;


		// Pper pixel operations
	
		// stencil test
		bool perform_stencil_test(stencil_t ref_val,
								  stencil_t read_mask,
								  CompareFunc func,
								  size_t row,
								  size_t col) const;

		void update_stencil_buffer(size_t row,
								   size_t col,
								   PerSampleOperation op_pass,
								   StencilOp stencil_pass_op,
								   StencilOp stencil_fail_op,
								   StencilOp stencil_zfail_op,
								   stencil_t ref_val) const;

		// depth test
		bool perform_depth_test(CompareFunc func, 
								size_t row,
								size_t col, 
								depth_t z) const;

		// blending
		static tinymath::Color blend(const tinymath::Color& src_color, 
							  const tinymath::Color& dst_color, 
							  BlendFactor src_factor, 
							  BlendFactor dst_factor, 
							  const BlendOp& op);

	private:
		FrameContent content_flag;
		size_t width;
		size_t height;
		Filtering filtering;

		tinymath::color_rgba clear_color;

		// use 32-bit zbuffer here, for convenience 
		std::unique_ptr<RawBuffer<depth_t>> depth_buffer;
		// 32-bit rgba framebuffer, 8-bit per channel
		std::unique_ptr<RawBuffer<tinymath::color_rgba>> color_buffer;
		// 8-bit stencil buffer
		std::unique_ptr<RawBuffer<stencil_t>> stencil_buffer;
		// 8-bit coverage buffer
		std::unique_ptr<RawBuffer<coverage_t>> coverage_buffer;
	};
}
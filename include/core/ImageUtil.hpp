#pragma once
#include "tinymath.h"
#include "Define.hpp"
#include "RawBuffer.hpp"

namespace CpuRasterizer
{
	class ImageUtil
	{
	public:
		template<typename T>
		static bool point(const RawBuffer<T>& image, float u, float v, T& out);
		template<typename T>
		static bool point(const RawBuffer<T>& image, float u, float v, float w, T& out);
		template<typename T>
		static bool linear(const RawBuffer<T>& image, float u, float v, T& out);
		template<typename T>
		static bool linear(const RawBuffer<T>& image, float u, float v, float w, T& out);

		template<typename T>
		static void resize(RawBuffer<T>& image, size_t width, size_t height, Filtering filtering);
		template<typename T>
		static void resize(RawBuffer<T>& image, size_t width, size_t height, size_t layer_count, Filtering filtering);

		static void resize(RawBuffer<stencil_t>& image, size_t width, size_t height, Filtering filtering);
	};

	inline void uv2pixel(size_t width, size_t height, size_t layer_count, float u, float v, float w, size_t& row, size_t& col, size_t& layer, float& row_frac, float& col_frac, float& layer_frac)
	{
		float rowf = v * (float)height;
		float colf = u * (float)width;
		float layerf = w * (float)layer_count;
		float row_integer = tinymath::floor(rowf - 0.5f);
		float col_integer = tinymath::floor(colf - 0.5f);
		float layer_integer = tinymath::floor(layerf - 0.5f);
		row = (size_t)(row_integer);
		col = (size_t)(col_integer);
		layer = (size_t)(layer_integer);
		row_frac = rowf - row_integer;
		col_frac = colf - col_integer;
		layer_frac = layerf - layer_integer;
	}

	inline void uv2pixel(size_t width, size_t height, size_t layer_count, float u, float v, float w, size_t& row, size_t& col, size_t& layer)
	{
		row = (size_t)(tinymath::floor(v * (float)height));
		col = (size_t)(tinymath::floor(u * (float)width));
		layer = (size_t)(tinymath::floor(w * (float)layer_count));
	}

	inline void pixel2uv(size_t width, size_t height, size_t layer_count, size_t row, size_t col, size_t layer, float& u, float& v, float& w)
	{
		u = ((float)col + 0.5f) / (float)width;
		v = ((float)row + 0.5f) / (float)height;
		w = ((float)layer + 0.5f) / (float)layer_count;
	}

	inline void uv2pixel(size_t _width, size_t _height, float u, float v, size_t& row, size_t& col, float& row_frac, float& col_frac)
	{
		float rowf = v * (float)_height - 0.5f;
		float colf = u * (float)_width - 0.5f;
		rowf = tinymath::clamp(rowf, 0.0f, (float)_height - 1.0f);
		colf = tinymath::clamp(colf, 0.0f, (float)_width - 1.0f);
		float row_integer = tinymath::floor(rowf);
		float col_integer = tinymath::floor(colf);
		row = (size_t)(row_integer);
		col = (size_t)(col_integer);
		row_frac = rowf - row_integer;
		col_frac = colf - col_integer;
	}

	inline void uv2pixel(size_t w, size_t h, float u, float v, size_t& row, size_t& col)
	{
		float rowf = v * (float)h - 0.5f;
		float colf = u * (float)w - 0.5f;
		rowf = tinymath::clamp(rowf, 0.0f, (float)h - 1.0f);
		colf = tinymath::clamp(colf, 0.0f, (float)w - 1.0f);
		row = (size_t)(tinymath::floor(rowf));
		col = (size_t)(tinymath::floor(colf));
	}

	inline void pixel2uv(size_t w, size_t h, size_t row, size_t col, float& u, float& v)
	{
		u = ((float)col + 0.5f) / (float)w;
		v = ((float)row + 0.5f) / (float)h;
	}

	template<typename T>
	inline bool ImageUtil::point(const RawBuffer<T>& image, float u, float v, T& out)
	{
		return image.read(u, v, out);
	}

	template<typename T>
	inline bool ImageUtil::point(const RawBuffer<T>& image, float u, float v, float w, T& out)
	{
		return image.read(u, v, w, out);
	}

	template<typename T>
	inline bool ImageUtil::linear(const RawBuffer<T>& image, float u, float v, T& out)
	{
		size_t row, col;
		float frac_row, frac_col;
		uv2pixel(image.get_width(), image.get_height(), u, v, row, col, frac_row, frac_col);

		T c00, c01, c11, c10;
		bool flag1 = image.read(row, col, c00);
		bool flag2 = image.read(row + 1ull, col, c01);
		bool flag3 = image.read(row + 1ull, col + 1ull, c11);
		bool flag4 = image.read(row, col + 1ull, c10);

		T a = c00 * (flag1 ? (1.0f - frac_row) : 0.0f) + c10 * (flag4 ? frac_row : 0.0f);
		T b = c01 * (flag2 ? (1.0f - frac_row) : 0.0f) + c11 * (flag3 ? frac_row : 0.0f);
		out = a * (1.0f - frac_col) + b * frac_col;

		return flag1 && flag2 && flag3 && flag4;
	}

	template<typename T>
	inline bool ImageUtil::linear(const RawBuffer<T>& image, float u, float v, float w, T& out)
	{
		UNUSED(image);
		UNUSED(u);
		UNUSED(v);
		UNUSED(w);
		UNUSED(out);
		return true;
	}

	template<typename T>
	inline void ImageUtil::resize(RawBuffer<T>& image, size_t width, size_t height, Filtering filtering)
	{
		RawBuffer<T> prev_image = image;

		image.reallocate(width, height);

		for (size_t row = 0; row < height; ++row)
		{
			for (size_t col = 0; col < width; ++col)
			{
				float u, v;
				pixel2uv(width, height, row, col, u, v);
				T val;
				if (filtering == Filtering::kPoint)
				{
					point(prev_image, u, v, val);
				}
				else if (filtering == Filtering::kBilinear)
				{
					linear(prev_image, u, v, val);
				}
				else
				{
					point(prev_image, u, v, val);
				}

				image.write(row, col, val);
			}
		} 
	}

	inline void ImageUtil::resize(RawBuffer<stencil_t>& image, size_t width, size_t height, Filtering filtering)
	{
		UNUSED(filtering);
		RawBuffer<stencil_t> prev_image = image;

		image.reallocate(width, height);

		for (size_t row = 0; row < height; ++row)
		{
			for (size_t col = 0; col < width; ++col)
			{
				float u, v;
				pixel2uv(width, height, row, col, u, v);
				stencil_t val;
				point(prev_image, u, v, val);
				image.write(row, col, val);
			}
		}
	}

	template<typename T>
	inline void ImageUtil::resize(RawBuffer<T>& image, size_t width, size_t height, size_t layer_count, Filtering filtering)
	{
		RawBuffer<T> prev_image = image;

		image.reallocate(width, height, layer_count);

		for (size_t row = 0; row < height; ++row)
		{
			for (size_t col = 0; col < width; ++col)
			{
				for (size_t layer = 0; layer < layer_count; ++layer)
				{
					float u, v, w;
					pixel2uv(width, height, layer_count, row, col, layer, u, v, w);
					T val;
					if (filtering == Filtering::kPoint)
					{
						point(prev_image, u, v, w, val);
					}
					else if (filtering == Filtering::kBilinear)
					{
						linear(prev_image, u, v, w, val);
					}
					else
					{
						point(prev_image, u, v, w, val);
					}
					image.write(row, col, layer, val);
				}
			}
		}
	}
}
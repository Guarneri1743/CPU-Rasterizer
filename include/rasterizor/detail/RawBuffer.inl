#include "tinymath.h"
#include "RawBuffer.hpp"
#include "tinymath/color/Color.h"
#include "tinymath/color/ColorEncoding.h"
#include <type_traits>

namespace CpuRasterizor
{
	template<typename T>
	RawBuffer<T>::RawBuffer(size_t w, size_t h)
	{
		this->width = w;
		this->height = h;
		this->deletor = [](T* ptr)
		{
			delete[] ptr;
		};
		int buf_size = static_cast<int>(static_cast<long>(width) * static_cast<long>(height));
		this->buffer = new T[buf_size];
	}

	template<typename T>
	RawBuffer<T>::RawBuffer(void* _buffer, size_t w, size_t h, void (*deletor)(T* ptr))
	{
		this->width = w;
		this->height = h;
		this->deletor = deletor;
		this->buffer = (T*)_buffer;
	}

	template<typename T>
	RawBuffer<T>::RawBuffer(const RawBuffer<T>& other)
	{
		copy(other);
	}

	template<typename T>
	RawBuffer<T>::~RawBuffer()
	{
		deletor(buffer);
	}

	template<typename T>
	std::shared_ptr<RawBuffer<T>> RawBuffer<T>::create(size_t w, size_t h)
	{
		return std::make_shared<RawBuffer>(w, h);
	}

	template<typename T>
	std::shared_ptr<RawBuffer<T>> RawBuffer<T>::create(void* _buffer, size_t w, size_t h, void (*deletor)(T* ptr))
	{
		return std::make_shared<RawBuffer>(_buffer, w, h, deletor);
	}

	template<typename T>
	std::shared_ptr<RawBuffer<T>> RawBuffer<T>::create(const RawBuffer<T>& other)
	{
		return std::make_shared<RawBuffer>(other);
	}

	template<typename T>
	bool RawBuffer<T>::read(const float& u, const float& v, T& out) const
	{
		return read(buffer, u, v, width, height, out);
	}

	template<typename T>
	bool RawBuffer<T>::read(const size_t& row, const size_t& col, T& out) const
	{
		return read(buffer, row, col, width, height, out);
	}

	template<typename T>
	bool RawBuffer<T>::write(const float& u, const float& v, const T& data)
	{
		return write(buffer, u, v, width, height, data);
	}

	template<typename T>
	bool RawBuffer<T>::write(const size_t& row, const size_t& col, const T& data)
	{
		return write(buffer, row, col, width, height, data);
	}

	template<typename T>
	bool RawBuffer<T>::read(T* buf, const float& u, const float& v, const size_t& w, const size_t& h, T& out) const
	{
		size_t row, col;
		uv2pixel(w, h, u, v, row, col);
		return read(buf, row, col, w, h, out);
	}

	template<typename T>
	bool RawBuffer<T>::read(T* buf, const size_t& row, const size_t& col, const size_t& w, const size_t& h, T& out) const
	{
		size_t pos = row * w + col;
		if (row >= h || col >= w || pos >= w * h)
		{
			return false;
		}
		out = buf[pos];
		return true;
	}

	template<typename T>
	bool RawBuffer<T>::write(T* buf, const float& u, const float& v, const size_t& w, const size_t& h, const T& data)
	{
		size_t row, col;
		uv2pixel(w, h, u, v, row, col);
		return write(buf, row, col, w, h, data);
	}

	template<typename T>
	bool RawBuffer<T>::write(T* buf, const size_t& row, const size_t& col, const size_t& w, const size_t& h, const T& data)
	{
		size_t pos = row * w + col;
		if (pos >= w * h)
		{
			return false;
		}
		buf[pos] = data;
		return true;
	}

	template<typename T>
	void RawBuffer<T>::resize(size_t w, size_t h)
	{
		if (w == width && h == height)
		{
			return;
		}

		size_t pw = width;
		size_t ph = height;

		T* new_buffer = new T[w * h];

		for (size_t row = 0; row < h; ++row)
		{
			for (size_t col = 0; col < w; ++col)
			{
				float u, v;
				pixel2uv(w, h, row, col, u, v);

				T val;
				read(buffer, u, v, pw, ph, val);
				write(new_buffer, row, col, w, h, val);
			}
		}

		delete[] buffer;
		width = w;
		height = h;
		buffer = new_buffer;
	}

	inline void uv2pixel(const size_t& w, const size_t& h, const float& u, const float& v, size_t& row, size_t& col, float& row_frac, float& col_frac)
	{
		float rowf = v * (float)h;
		float colf = u * (float)w;
		float row_integer = tinymath::floor(rowf - 0.5f);
		float col_integer = tinymath::floor(colf - 0.5f);
		row = (size_t)(row_integer);
		col = (size_t)(col_integer);
		row_frac = rowf - row_integer;
		col_frac = colf - col_integer;
	}

	inline void uv2pixel(const size_t& w, const size_t& h, const float& u, const float& v, size_t& row, size_t& col)
	{
		// [0.0, 1.0] -> [0, w/h - 1]
		row = (size_t)(tinymath::floor(v * (float)h));
		col = (size_t)(tinymath::floor(u * (float)w));
	}

	inline void pixel2uv(const size_t& w, const size_t& h, const size_t& row, const size_t& col, float& u, float& v)
	{
		//[0, w/h - 1] -> [0.0, 1.0]
		u = ((float)col + 0.5f)/ (float)w;
		v = ((float)row + 0.5f) / (float)h;
	}

	template<typename T>
	void RawBuffer<T>::clear(const T& val)
	{
		size_t size = width * height;
		std::fill(buffer, buffer + size, val);
	}

	template<typename T>
	T* RawBuffer<T>::get_ptr(size_t& size)
	{
		size = (size_t)width * (size_t)height * sizeof(T);
		return buffer;
	}

	template<typename T>
	RawBuffer<T>& RawBuffer<T>::operator = (const RawBuffer<T>& other)
	{
		copy(other);
		return *this;
	}

	template<typename T>
	void RawBuffer<T>::copy(const RawBuffer<T>& other)
	{
		this->buffer = other.buffer;
		this->width = other.width;
		this->height = other.height;
	}
}
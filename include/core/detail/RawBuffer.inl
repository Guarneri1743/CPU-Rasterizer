#include "tinymath.h"
#include <type_traits>
#include "RawBuffer.hpp"
#include "ImageUtil.hpp"

namespace CpuRasterizer
{
	template<typename T>
	RawBuffer<T>::RawBuffer()
	{
		this->width = 0;
		this->height = 0;
		this->layer_count = 0;
		this->buffer_length = 0;
		this->buffer = nullptr;
		this->deletor = nullptr;
	}

	template<typename T>
	RawBuffer<T>::RawBuffer(size_t w, size_t h, size_t lc)
	{
		this->width = w;
		this->height = h;
		this->layer_count = lc;
		this->deletor = [](T* ptr)
		{
			delete[] ptr;
		};
		buffer_length = w * h * lc;
		this->buffer = new T[buffer_length];
	}

	template<typename T>
	RawBuffer<T>::RawBuffer(size_t w, size_t h) : RawBuffer(w, h, 1ull)
	{}

	template<typename T>
	RawBuffer<T>::RawBuffer(void* _buffer, size_t w, size_t h, size_t lc, void (*deletor)(T* ptr))
	{
		this->width = w;
		this->height = h;
		this->layer_count = lc;
		this->buffer_length = w * h * lc;
		this->deletor = deletor;
		this->buffer = (T*)_buffer;
	}

	template<typename T>
	RawBuffer<T>::RawBuffer(void* _buffer, size_t w, size_t h, void (*deletor)(T* ptr)) : RawBuffer(_buffer, w, h, 1ull, deletor)
	{}

	template<typename T>
	RawBuffer<T>::~RawBuffer()
	{
		deletor(buffer);
	}

	template<typename T>
	std::shared_ptr<RawBuffer<T>> RawBuffer<T>::create(size_t w, size_t h, size_t lc)
	{
		return std::make_shared<RawBuffer>(w, h, lc);
	}	
	
	template<typename T>
	std::shared_ptr<RawBuffer<T>> RawBuffer<T>::create(size_t w, size_t h)
	{
		return std::make_shared<RawBuffer>(w, h);
	}

	template<typename T>
	std::shared_ptr<RawBuffer<T>> RawBuffer<T>::create(void* _buffer, size_t w, size_t h, size_t lc, void (*deletor)(T* ptr))
	{
		return std::make_shared<RawBuffer>(_buffer, w, h, lc, deletor);
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
	bool RawBuffer<T>::read(float u, float v, T& out) const
	{
		return read(buffer, u, v, width, height, out);
	}

	template<typename T>
	bool RawBuffer<T>::read(size_t row, size_t col, T& out) const
	{
		return read(buffer, row, col, width, height, out);
	}

	template<typename T>
	bool RawBuffer<T>::read(size_t row, size_t col, size_t layer, T& out) const
	{
		return read(buffer, row, col, layer, width, height, layer_count, out);
	}

	template<typename T>
	bool RawBuffer<T>::write(float u, float v, const T& data)
	{
		return write(buffer, u, v, width, height, data);
	}

	template<typename T>
	bool RawBuffer<T>::read(float u, float v, float w, T& out) const
	{
		return read(buffer, u, v, w, width, height, layer_count, out);
	}

	template<typename T>
	bool RawBuffer<T>::write(float u, float v, float w, const T& data)
	{
		return write(buffer, u, v, w, width, height, layer_count, data);
	}

	template<typename T>
	bool RawBuffer<T>::write(size_t row, size_t col, const T& data)
	{
		return write(buffer, row, col, width, height, data);
	}

	template<typename T>
	bool RawBuffer<T>::write(size_t row, size_t col, size_t layer, const T& data)
	{
		return write(buffer, row, col, layer, width, height, layer_count, data);
	}

	template<typename T>
	bool RawBuffer<T>::read(T* buf, float u, float v, size_t w, size_t h, T& out)
	{
		size_t row, col;
		uv2pixel(w, h, u, v, row, col);
		return read(buf, row, col, w, h, out);
	}

	template<typename T>
	bool RawBuffer<T>::read(T* buf, size_t row, size_t col, size_t w, size_t h, T& out)
	{
		size_t pos = row * w + col;
		if (row >= h || col >= w)
		{
			return false;
		}
		out = buf[pos];
		return true;
	}

	template<typename T>
	bool RawBuffer<T>::write(T* buf, float u, float v, size_t w, size_t h, const T& data)
	{
		size_t row, col;
		uv2pixel(w, h, u, v, row, col);
		return write(buf, row, col, w, h, data);
	}

	template<typename T>
	bool RawBuffer<T>::write(T* buf, size_t row, size_t col, size_t w, size_t h, const T& data)
	{
		size_t pos = row * w + col;
		if (row >= h || col >= w)
		{
			return false;
		}
		buf[pos] = data;
		return true;
	}

	template<typename T>
	bool RawBuffer<T>::read(T* buf, float u, float v, float w, size_t width, size_t height, size_t layer_count, T& out)
	{
		size_t row, col, layer;
		uv2pixel(width, height, layer_count, u, v, w, row, col, layer);
		return read(buf, row, col, layer, width, height, layer_count, out);
	}

	template<typename T>
	bool RawBuffer<T>::read(T* buf, size_t row, size_t col, size_t layer, size_t width, size_t height, size_t layer_count, T& out) 
	{
		size_t pos = row * width * layer_count + col * layer_count + layer;
		if (row >= height || col >= width || layer >= layer_count)
		{
			return false;
		}
		out = buf[pos];
		return true;
	}

	template<typename T>
	bool RawBuffer<T>::write(T* buf, float u, float v, float w, size_t width, size_t height, size_t layer_count, const T& data)
	{
		size_t row, col, layer;
		uv2pixel(width, height, layer_count, u, v, w, row, col, layer);
		return write(buf, row, col, layer, width, height, layer_count, data);

	}

	template<typename T>
	bool RawBuffer<T>::write(T* buf, size_t row, size_t col, size_t layer, size_t width, size_t height, size_t layer_count, const T& data)
	{
		size_t pos = row * width * layer_count + col * layer_count + layer;
		if (row >= height || col >= width || layer >= layer_count)
		{
			return false;
		}
		buf[pos] = data;
		return true;
	}

	template<typename T>
	void RawBuffer<T>::reallocate(size_t w, size_t h, size_t lc)
	{
		this->width = w;
		this->height = h;
		this->layer_count = lc;
		this->buffer_length = w * h * lc;
		delete[] buffer;
		buffer = new T[buffer_length];
	}

	template<typename T>
	void RawBuffer<T>::reallocate(size_t w, size_t h)
	{
		reallocate(w, h, 1ull);
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
		size = (size_t)width * (size_t)height * (size_t)layer_count * sizeof(T);
		return buffer;
	}

	template<typename T>
	RawBuffer<T>& RawBuffer<T>::operator = (const RawBuffer<T>& other)
	{
		this->buffer_length = other.width * other.height * other.layer_count;
		if (width == other.width && height == other.height && layer_count == other.layer_count)
		{
			// copy
			memcpy(buffer, other.buffer, buffer_length * sizeof(T));
			this->deletor = other.deletor;
			return *this;
		}
		else
		{
			// reallocate and copy
			delete[] buffer;
			buffer = new T[buffer_length];
			memcpy(buffer, other.buffer, buffer_length * sizeof(T));
			this->width = other.width;
			this->height = other.height;
			this->layer_count = other.layer_count;
			this->deletor = other.deletor;
		}
	}

	template<typename T>
	RawBuffer<T>::RawBuffer(const RawBuffer<T>& other) 
	{
		this->buffer_length = other.width * other.height * other.layer_count;
		buffer = new T[buffer_length];
		memcpy(buffer, other.buffer, buffer_length * sizeof(T));
		this->width = other.width;
		this->height = other.height;
		this->layer_count = other.layer_count;
		this->deletor = other.deletor;
	}
}
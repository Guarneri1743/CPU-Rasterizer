#pragma once
#include <stdint.h>
#include <memory>

namespace Guarneri
{
	template<typename T>
	class RawBuffer
	{
	private:
		T* buffer;
		void (*deletor)(T* ptr);

	public:
		size_t width;
		size_t height;

	public:
		RawBuffer(size_t _width, size_t _height);
		RawBuffer(void* _buffer, size_t _width, size_t _height, void (*deletor)(T* ptr));
		RawBuffer(const RawBuffer<T>& other);
		~RawBuffer();
		static std::shared_ptr<RawBuffer> create(size_t _width, size_t _height);
		static std::shared_ptr<RawBuffer> create(void* _buffer, size_t _width, size_t _height, void (*deletor)(T* ptr));
		static std::shared_ptr<RawBuffer> create(const RawBuffer<T>& other);
		bool read(const float& u, const float& v, T& out) const;
		bool read(const size_t& row, const size_t& col, T& out) const;
		bool write(const float& u, const float& v, const T& data);
		bool write(const size_t& row, const size_t& col, const T& data);
		void clear(const T& val);
		T* get_ptr(size_t& size);
		RawBuffer<T>& operator = (const RawBuffer<T>& other);
		void copy(const RawBuffer<T>& other);
	};

	void uv2pixel(const size_t& width, const size_t& height, const float& u, const float& v, size_t& row, size_t& col);
	void pixel2uv(const size_t& width, const size_t& height, const size_t& row, const size_t& col, float& u, float& v);
}

#include "detail/RawBuffer.inl"
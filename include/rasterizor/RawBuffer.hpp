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
		uint32_t width;
		uint32_t height;

	public:
		RawBuffer(uint32_t _width, uint32_t _height);
		RawBuffer(void* _buffer, uint32_t _width, uint32_t _height, void (*deletor)(T* ptr));
		RawBuffer(const RawBuffer<T>& other);
		~RawBuffer();
		static std::shared_ptr<RawBuffer> create(uint32_t _width, uint32_t _height);
		static std::shared_ptr<RawBuffer> create(void* _buffer, uint32_t _width, uint32_t _height, void (*deletor)(T* ptr));
		static std::shared_ptr<RawBuffer> create(const RawBuffer<T>& other);
		bool read(const float& u, const float& v, T& out) const;
		bool read(const uint32_t& row, const uint32_t& col, T& out) const;
		bool write(const float& u, const float& v, const T& data);
		bool write(const uint32_t& row, const uint32_t& col, const T& data);
		void clear(const T& val);
		T* get_ptr(size_t& size);
		RawBuffer<T>& operator = (const RawBuffer<T>& other);
		void copy(const RawBuffer<T>& other);
	};

	void uv2pixel(const uint32_t& width, const uint32_t& height, const float& u, const float& v, uint32_t& row, uint32_t& col);
	void pixel2uv(const uint32_t& width, const uint32_t& height, const uint32_t& row, const uint32_t& col, float& u, float& v);
}

#include "detail/RawBuffer.inl"
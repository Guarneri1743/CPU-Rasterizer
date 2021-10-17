#pragma once
#include <stdint.h>
#include <memory>

namespace CpuRasterizor
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
		RawBuffer(size_t w, size_t h);
		RawBuffer(void* _buffer, size_t w, size_t h, void (*deletor)(T* ptr));
		RawBuffer(const RawBuffer<T>& other);
		~RawBuffer();
		static std::shared_ptr<RawBuffer> create(size_t w, size_t h);
		static std::shared_ptr<RawBuffer> create(void* _buffer, size_t w, size_t h, void (*deletor)(T* ptr));
		static std::shared_ptr<RawBuffer> create(const RawBuffer<T>& other);
		bool read(float u, float v, T& out) const;
		bool read(size_t row, size_t col, T& out) const;
		bool write(float u, float v, const T& data);
		bool write(size_t row, size_t col, const T& data);

		bool read(T* buf, float u, float v, size_t w, size_t h, T& out) const;
		bool read(T* buf, size_t row, size_t col, size_t w, size_t h, T& out) const;
		bool write(T* buf, float u, float v, size_t w, size_t h, const T& data);
		bool write(T* buf, size_t row, size_t col, size_t w, size_t h, const T& data);

		void resize(size_t w, size_t h);
		void clear(const T& val);
		T* get_ptr(size_t& size);
		RawBuffer<T>& operator = (const RawBuffer<T>& other);
		void copy(const RawBuffer<T>& other);
	};

	void uv2pixel(size_t w, size_t h, float u, float v, size_t& row, size_t& col, float& row_frac, float& col_frac);
	void uv2pixel(size_t w, size_t h, float u, float v, size_t& row, size_t& col);
	void pixel2uv(size_t w, size_t h, size_t row, size_t col, float& u, float& v);
}

#include "detail/RawBuffer.inl"
#pragma once
#include <stdint.h>
#include <memory>
#include <functional>

namespace CpuRasterizer
{
	template<typename T>
	class RawBuffer
	{
	public:
		RawBuffer();
		RawBuffer(size_t width, size_t height, size_t layer_count);
		RawBuffer(size_t width, size_t height);
		RawBuffer(void* _buffer, size_t width, size_t height, size_t layer_count, void (*deletor)(T* ptr));
		RawBuffer(void* _buffer, size_t width, size_t height, void (*deletor)(T* ptr));
		~RawBuffer();

		static std::shared_ptr<RawBuffer> create(size_t width, size_t height, size_t layer_count);
		static std::shared_ptr<RawBuffer> create(size_t width, size_t height);
		static std::shared_ptr<RawBuffer> create(void* _buffer, size_t width, size_t height, size_t layer_count, void (*deletor)(T* ptr));
		static std::shared_ptr<RawBuffer> create(void* _buffer, size_t width, size_t height, void (*deletor)(T* ptr));
		static std::shared_ptr<RawBuffer> create(const RawBuffer<T>& other);

		bool read(float u, float v, T& out) const;
		bool write(float u, float v, const T& data);

		bool read(float u, float v, float w, T& out) const;
		bool write(float u, float v, float w, const T& data);

		bool read(size_t row, size_t col, T& out) const;
		bool write(size_t row, size_t col, const T& data);

		bool read(size_t row, size_t col, size_t layer, T& out) const;
		bool write(size_t row, size_t col, size_t layer, const T& data);

		static bool read(T* buf, float u, float v, size_t width, size_t height, T& out);
		static bool write(T* buf, float u, float v, size_t width, size_t height, const T& data);

		static bool read(T* buf, size_t row, size_t col, size_t width, size_t height, T& out);
		static bool write(T* buf, size_t row, size_t col, size_t width, size_t height, const T& data);

		static bool read(T* buf, float u, float v, float w, size_t width, size_t height, size_t layer_count, T& out);
		static bool write(T* buf, float u, float v, float w, size_t width, size_t height, size_t layer_count, const T& data);

		static bool read(T* buf, size_t row, size_t col, size_t layer, size_t width, size_t height, size_t layer_count, T& out);
		static bool write(T* buf, size_t row, size_t col, size_t layer, size_t width, size_t height, size_t layer_count, const T& data);

		void reallocate(size_t width, size_t height, size_t layer_count);
		void reallocate(size_t width, size_t height);

		inline size_t get_width() const { return width; }
		inline size_t get_height() const { return height; }

		void clear(const T& val);
		T* get_ptr(size_t& size);

		RawBuffer(const RawBuffer<T>& other);
		RawBuffer<T>& operator = (const RawBuffer<T>& other);

	private:
		T* buffer;
		void (*deletor)(T* ptr);
		size_t width;
		size_t height;
		size_t layer_count;
		size_t buffer_length;
	};
}

#include "detail/RawBuffer.inl"
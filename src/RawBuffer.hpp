#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	template<typename T>
	class RawBuffer {
	public:
		RawBuffer(uint32_t width, uint32_t height) {
			this->width = width;
			this->height = height;
			int size = width * height;
			buffer = std::shared_ptr<T>(new T[size], [](T* ptr) { delete[] ptr; });
		}		

		RawBuffer(void* buffer, uint32_t width, uint32_t height, void (*deletor)(T* ptr)) {
			this->width = width;
			this->height = height;
			auto buf_array = (T*)buffer;
			this->buffer = std::shared_ptr<T>(buf_array, deletor);
		}

		RawBuffer(const std::shared_ptr<T>& buffer, uint32_t width, uint32_t height) {
			this->width = width;
			this->height = height;
			this->buffer = buffer;
		}

		RawBuffer(const RawBuffer<T>& other) {
			copy(other);
		}
		
		~RawBuffer() {
			buffer.reset();
		}

	private:
		std::shared_ptr<T> buffer;
		uint32_t width;
		uint32_t height;

	public:
		bool read(const float& u, const float& v, T& out) const {
			uint32_t row, col;
			uv2pixel(u, v, row, col);
			return read(row, col, out);
		}

		bool read(const uint32_t& row, const uint32_t& col, T& out) const {
			uint32_t pos = row * width + col;
			if (row >= height || col >= width || pos >= width * height) {
				return false;
			}
			out = buffer.get()[pos];
			return true;
		}

		bool write(const float& u, const float& v, const T& data) {
			uint32_t row, col;
			uv2pixel(u, v, row, col);
			return this->write(x, y, data);
		}

		bool write(const uint32_t& row, const uint32_t& col, const T& data) {
			uint32_t pos = row * width + col;
			if (row >= height || col >= width || pos >= width * height) {
				return false;
			}
			buffer.get()[pos] = data;
			return true;
		}

		void uv2pixel(const float& u, const float& v, uint32_t& row, uint32_t& col) const {
			// [0.0, 1.0) -> [0, width-1]
			row = FLOOR_UINT(v * this->width - EPSILON);
			col = FLOOR_UINT(u * this->height - EPSILON);
		}

		void clear(const T& val) {
			uint32_t size = width * height;
			std::fill(buffer.get(), buffer.get() + size, val);
		}

		T* get_ptr(int& size) {
			size = width * height * sizeof(T);
			return buffer;
		}

		RawBuffer<T>& operator = (const RawBuffer<T>& other) {
			copy(other);
			return *this;
		}

		void copy(const RawBuffer<T>& other) {
			this->buffer = other.buffer;
			this->width = other.width;
			this->height = other.height;
		}
	};
}
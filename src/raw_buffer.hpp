#pragma once
#include<common.hpp>

namespace guarneri {
	template<typename T>
	class raw_buffer {
	public:
		raw_buffer(uint32_t width, uint32_t height) {
			this->width = width;
			this->height = height;
			int size = width * height;
			buffer = new T[size];
			managed_buffer = true;
		}		

		raw_buffer(void* buffer, uint32_t width, uint32_t height) {
			this->width = width;
			this->height = height;
			this->buffer = (T*)buffer;
			managed_buffer = false;
		}
		
		~raw_buffer() {
			if(managed_buffer)
			{
				if (buffer) delete[] buffer;
			}
		}

	private:
		bool managed_buffer;
		T* buffer;
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
			out = buffer[pos];
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
			buffer[pos] = data;
			return true;
		}

		void uv2pixel(const float& u, const float& v, uint32_t& row, uint32_t& col) const {
			// [0.0, 1.0) -> [0, width-1]
			row = FLOOR_UINT(u * this->width - EPSILON);
			col = FLOOR_UINT(v * this->height - EPSILON);
		}

		void clear(const T& val) {
			uint32_t size = width * height;
			std::fill(buffer, buffer + size, val);
		}

		T* get_ptr(int& size) {
			size = width * height * sizeof(T);
			return buffer;
		}
	};
}
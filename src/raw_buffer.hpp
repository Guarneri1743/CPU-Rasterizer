#pragma once
#include<common.hpp>

namespace guarneri {
	template<typename T>
	struct raw_buffer {
	public:
		raw_buffer(unsigned int width, unsigned int height) {
			this->width = width;
			this->height = height;
			int size = width * height;
			buffer = new T[size];
			managed_buffer = true;
		}		

		raw_buffer(void* buffer, unsigned int width, unsigned int height) {
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
		unsigned int width;
		unsigned int height;

	public:
		bool read(const float& u, const float& v, T& out) const {
			unsigned int x = CEIL(u * this->width);
			unsigned int y = CEIL(v * this->height);
			return read(x, y, out);
		}

		bool read(const unsigned int& row, const unsigned int& col, T& out) const {
			unsigned int pos = row * width + col;
			if (row >= height || col >= width || pos >= width * height) {
				return false;
			}
			out = buffer[pos];
			return true;
		}

		bool write(const float& u, const float& v, const T& data) {
			unsigned int x = CEIL(u * this->width);
			unsigned int y = CEIL(v * this->height);
			return this->write(x, y, data);
		}

		bool write(const unsigned int& row, const unsigned int& col, const T& data) {
			unsigned int pos = row * width + col;
			if (row >= height || col >= width || pos >= width * height) {
				return false;
			}
			buffer[pos] = data;
			return true;
		}

		void clear(const T& val) {
			unsigned int size = width * height;
			//std::memset(buffer, val, static_cast<size_t>(size));
			std::fill(buffer, buffer + size, val);
		}

		T* get_ptr(int& size) {
			size = width * height * sizeof(T);
			return buffer;
		}
	};
}
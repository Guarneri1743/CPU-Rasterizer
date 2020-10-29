#pragma once
#include<common.hpp>

namespace guarneri {
	template<typename T>
	struct raw_buffer {
	public:
		raw_buffer(unsigned int width, unsigned int height) {
			this->width = width;
			this->height = height;
			int size = static_cast<int>(width * height);
			buffer = new T[size];
		}		
		
		~raw_buffer() {
			delete[] buffer;
		}

	private:
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
			int pos = static_cast<int>(row * width + col);
			if (pos >= width * height) {
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
			int pos = static_cast<int>(row * width + col);
			if (pos >= width * height) {
				return false;
			}
			buffer[pos] = data;
			return true;
		}

		void clear() {
			long long size = (long long)width * (long long)height * sizeof(*buffer);
			std::memset(buffer, 0, static_cast<size_t>(size));
		}

		T* get_ptr(int& size) {
			size = width * height * sizeof(T);
			return buffer;
		}
	};
}
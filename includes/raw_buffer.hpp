#pragma once
#include<common.hpp>

namespace guarneri {
	template<typename T>
	struct raw_buffer {
	public:
		raw_buffer(int width, int height) {
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
		int width;
		int height;

	public:
		bool read(const float& u, const float& v, T& out) {
			int x = CEIL(u * this->width);
			int y = CEIL(v * this->height);
			return read(x, y, out);
		}

		bool read(const int& row, const int& col, T& out) {
			int pos = static_cast<int>(row * width + col);
			if (pos >= width * height) {
				std::cerr << "write pixel failed: index out of range" << std::endl;
				return false;
			}
			out = buffer[pos];
			return true;
		}

		bool write(float u, float v, T data) {
			int x = CEIL(u * this->width);
			int y = CEIL(v * this->height);
			return this->write(x, y, data);
		}

		bool write(int row, int col, T data) {
			int pos = static_cast<int>(row * width + col);
			if (pos >= width * height) {
				std::cerr << "write pixel failed: index out of range" << std::endl;
				return false;
			}
			buffer[pos] = data;
			return true;
		}

		void clear() {
			long long size = (long long)width * (long long)height * sizeof(*buffer);
			std::memset(buffer, 0, static_cast<size_t>(size));
		}
	};
}
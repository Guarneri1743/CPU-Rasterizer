#pragma once
#include <common.hpp>
#include <float4.hpp>

namespace guarneri {
	struct framebuffer {
	public:
		framebuffer(void* buffer, unsigned int width, unsigned int height) {
			this->buffer = (color_t*)buffer;
			this->width = width;
			this->height = height;
		}

	private:
		color_t* buffer;
		unsigned int width;
		unsigned int height;

	public:
		bool write(const unsigned int& row, const unsigned int& col, const color_t& color) {
			int pos = static_cast<int>(row * width + col);
			if (row >= height || col >= width || pos >= width * height) {
				return false;
			}
 			this->buffer[pos] = color;
			return true;
		}

		bool read(const unsigned int& row, const unsigned int& col, color_t& color) const {
			int pos = static_cast<int>(row * width + col);
			if (row >= height || col >= width || pos >= width * height) {
				return false;
			}
			color = this->buffer[pos];
			return true;
		}

		void clear() {
			long long size = (long long)width * (long long)height * sizeof(*buffer);
			std::memset(buffer, 0, static_cast<size_t>(size));
		}

		void* get_ptr(int& size) {
			size = width * height * sizeof(color_t);
			return buffer;
		}
	};
}
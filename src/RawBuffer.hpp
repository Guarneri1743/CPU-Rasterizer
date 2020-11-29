#ifndef _RAW_BUFFER_
#define _RAW_BUFFER_
#include <Guarneri.hpp>

namespace Guarneri {
	template<typename T>
	class RawBuffer {
	public:
		RawBuffer(uint32_t _width, uint32_t _height) {
			this->width = _width;
			this->height = _height;
			this->deletor = [](T* ptr) { delete[] ptr; };
			int buf_size = static_cast<int>(static_cast<long>(width) * static_cast<long>(_height));
			this->buffer = new T[buf_size];
		}		

		RawBuffer(void* _buffer, uint32_t _width, uint32_t _height, void (*deletor)(T* ptr)) {
			this->width = _width;
			this->height = _height;
			this->deletor = deletor;
			this->buffer = (T*)_buffer;
		}

		RawBuffer(const RawBuffer<T>& other) {
			copy(other);
		}
		
		~RawBuffer() 
		{
			deletor(buffer);
		}

	private:
		T* buffer;
		void (*deletor)(T* ptr);
		uint32_t width;
		uint32_t height;

	public:
		static std::shared_ptr<RawBuffer> create(uint32_t _width, uint32_t _height) {
			return std::make_shared<RawBuffer>(_width, _height);
		}

		static std::shared_ptr<RawBuffer> create(void* _buffer, uint32_t _width, uint32_t _height, void (*deletor)(T* ptr)) {
			return std::make_shared<RawBuffer>(_buffer, _width, _height, deletor);
		}

		static std::shared_ptr<RawBuffer> create(const RawBuffer<T>& other) {
			return std::make_shared<RawBuffer>(other);
		}

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
			return this->write(row, col, data);
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
			row = (uint32_t)(std::floor(v * this->height + 0.5f));
			col = (uint32_t)(std::floor(u * this->width + 0.5f));
		}

		void clear(const T& val) {
			uint32_t size = width * height;
			std::fill(buffer, buffer + size, val);
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
#endif
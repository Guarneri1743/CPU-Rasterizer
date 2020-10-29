#pragma once
#include <common.hpp>
#include <raw_buffer.hpp>

namespace guarneri {
	typedef std::string texture_id;
	struct texture {
	public:
		texture(const texture_id& id, const int& width, const int& height) {
			this->id = id;
			buffer = new raw_buffer<color_t>(width, height);
		}

		~texture() {
			delete buffer;
		}

	public:
		texture_id id;

	private:
		raw_buffer<color_t>* buffer;

	public:
		bool read(const float& u, const float& v, color_t& ret) const {
			return buffer->read(u, v, ret);
		}

		bool write(const float& u, const float& v, const color_t& ret) {
			return buffer->write(u, v, ret);
		}

		bool read(const int& x, const int& y, color_t& ret) const {
			return buffer->read(x, y, ret);
		}

		bool write(const int& x, const int& y, const color_t& ret) {
			return buffer->write(x, y, ret);
		}

		void clear() {
			buffer->clear();
		}
	};
}
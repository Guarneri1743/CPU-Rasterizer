#pragma once
#include <common.hpp>
#include <raw_buffer.hpp>

namespace guarneri {
	struct texture {
	public:
		texture(std::string name, id_t id) {
			this->name = name;
			this->id = id;
		}

	public:
		std::string name;

	private:
		id_t id;
		raw_buffer<color_t>* buffer;

	public:
		id_t get_id() const {
			return id;
		}

		bool read(const float& u, const float& v, color_t& ret) {
			return buffer->read(u, v, ret);
		}

		bool write(const float& u, const float& v, const color_t& ret) {
			return buffer->write(u, v, ret);
		}

		bool read(const int& x, const int& y, color_t& ret) {
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
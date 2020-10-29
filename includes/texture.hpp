#pragma once
#include <common.hpp>
#include <raw_buffer.hpp>

namespace guarneri {
	enum class wrap_mode {
		repeat,
		clamp
	};

	enum class filtering {
		point,
		bilinear
	};
	template <typename T>
	struct texture {
	public:
		texture(const texture_id& id, const unsigned int& width, const unsigned int& height) {
			this->id = id;
			buffer = new raw_buffer<T>(width, height);
		}

		~texture() {
			delete buffer;
		}

	public:
		texture_id id;
		wrap_mode wrap_mode;
		filtering filtering;

	private:
		raw_buffer<float>* buffer;

	public:
		bool sample(const float& u, const float& v, T& ret) const {
			float wu = u;
			float wv = v;
			this->wrap(wu, wv);
			return buffer->read(wu, wv, ret);
		}

		bool write(const float& u, const float& v, const T& ret) {
			float wu = u;
			float wv = v;
			this->wrap(wu, wv);
			return buffer->write(wu, wv, ret);
		}

		bool sample(const unsigned int& x, const unsigned int& y, T& ret) const {
			return buffer->sample(x, y, ret);
		}

		bool write(const unsigned int& x, const unsigned int& y, const T& ret) {
			return buffer->write(x, y, ret);
		}

	private:
		void wrap(float& u, float& v) const {
			switch (wrap_mode) {
			case wrap_mode::clamp:
				if (u < 0.0f) {
					u = 0.0f;
				}
				if (u > 1.0f) {
					u = 1.0f;
				}
				if (v < 0.0f) {
					v = 0.0f;
				}
				if (v > 1.0f) {
					v = 1.0f;
				}
				break;
			case wrap_mode::repeat:
				if (u < 0.0f) {
					u += 1.0f;
				}
				if (u > 1.0f) {
					u -= 1.0f;
				}
				if (v < 0.0f) {
					v += 1.0f;
				}
				if (v > 1.0f) {
					v -= 1.0f;
				}
				break;
			}
		}

		void clear() {
			buffer->clear();
		}
	};
}
#pragma once
#include <guarneri.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.hpp>

namespace guarneri {
	enum class wrap_mode {
		repeat,
		clamp
	};

	enum class filtering {
		point,
		bilinear
	};

	// todo: support other formats
	enum class texture_format {
		invalid,
		rgb,
		rgba
	};

	class texture {
	public:
		texture(const texture_id& id, const uint32_t& width, const uint32_t& height, const texture_format& fmt) {
			this->id = id;
			this->fmt = fmt;
			release();
			switch (fmt) {
				case texture_format::rgb:
					rgb_buffer = std::make_shared<raw_buffer<color_rgb>>(width, height);
					break;
				case texture_format::rgba:
					rgba_buffer = std::make_shared<raw_buffer<color_rgba>>(width, height);
					break;
			}
		}

		texture(void* tex_buffer, const texture_id& id, const uint32_t& width, const uint32_t& height, const texture_format& fmt) {
			this->id = id;
			this->fmt = fmt;
			release();
			switch (fmt) {
				case texture_format::rgb:
					{
						rgb_buffer = std::make_shared<raw_buffer<color_rgb>>(tex_buffer, width, height);
					}
					break;
				case texture_format::rgba:
					{
						rgba_buffer = std::make_shared<raw_buffer<color_rgba>>(tex_buffer, width, height);
					}
					break;
			}
		}

		texture(const char* path, const texture_id& id) {
			this->id = id;
			this->fmt = texture_format::invalid;
			release();
			int width, height, channels;
			if (!FS::exists(path)) {
				std::cerr << "path does not exist" << std::endl;
			}
			else {
				stb_uchar* tex = stbi_load(path, &width, &height, &channels, 0);
				this->stb_data = std::make_shared<stb_uchar>(tex, [](stb_uchar* ptr) { delete[] ptr; });
				if (channels == 3) {
					rgb_buffer = std::make_shared<raw_buffer<color_rgb>>(tex, width, height);
					this->fmt = texture_format::rgb;
				}
				else if (channels == 4) {
					rgba_buffer = std::make_shared<raw_buffer<color_rgba>>(tex, width, height);
					this->fmt = texture_format::rgba;
				}
				else {
					std::cerr << "invalid channels: " << channels << std::endl;
				}
			}
		}

		texture(const texture& other) {
			deep_copy(other);
		}

		~texture() {
			release();
		}

	public:
		texture_id id;
		wrap_mode wrap_mode;
		filtering filtering;
		texture_format fmt;

	private:
		std::shared_ptr<raw_buffer<color_rgb>> rgb_buffer;
		std::shared_ptr<raw_buffer<color_rgba>> rgba_buffer;
		std::shared_ptr<stb_uchar> stb_data;

	public:
		bool sample(const float& u, const float& v, color& ret) const {
			float wu = u;
			float wv = v;
			this->wrap(wu, wv);
			switch (fmt) {
				case texture_format::rgb:
				{
					if (rgb_buffer == nullptr) return false;
					color_rgb pixel;
					bool ok = rgb_buffer->read(wu, wv, pixel);
					ret = color::decode(pixel);
					return ok;
				}
				case texture_format::rgba:
				{
					if(rgba_buffer == nullptr) return false;
					color_rgba pixel;
					bool ok =rgba_buffer->read(wu, wv, pixel);
					ret = color::decode(pixel);
					return ok;
				}
			}
			return false;
		}

		bool sample(const uint32_t& row, const uint32_t& col, color& ret) const {
			switch (fmt) {
				case texture_format::rgb:
				{
					if (rgb_buffer == nullptr) return false;
					color_rgb pixel;
					bool ok = rgb_buffer->read(row, col, pixel);
					ret = color::decode(pixel);
					return ok;
				}
				case texture_format::rgba:
				{
					if (rgba_buffer == nullptr) return false;
					color_rgba pixel;
					bool ok = rgba_buffer->read(row, col, pixel);
					ret = color::decode(pixel);
					return ok;
				}
			}
			return false;
		}

		bool write(const uint32_t& x, const uint32_t& y, const color& data) {
			switch (fmt) {
				case texture_format::rgb:
					if (rgba_buffer == nullptr) return false;
					return rgb_buffer->write(x, y, color::encode_rgb(data));
				case texture_format::rgba:
					{
						if (rgba_buffer == nullptr) return false;
						return rgba_buffer->write(x, y, color::encode_rgba(data));
					}
			}
			return false;
		}

		void save2file() {
			switch (fmt) {
			case texture_format::rgb:
				break;
			case texture_format::rgba:
				break;
			}
		}

		// todo
		void resize() {

		}

		void release() {
			rgb_buffer.reset();
			rgba_buffer.reset();
			stb_data.reset();
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
			switch (fmt) {
			case texture_format::rgb:
				rgb_buffer->clear(color_rgb());
				break;
			case texture_format::rgba:
				rgba_buffer->clear(color_rgba());
				break;
			}
		}

		void operator =(const texture& other) {
			deep_copy(other);
		}

		void deep_copy(const texture& other) {
			this->id = other.id;
			this->wrap_mode = other.wrap_mode;
			this->filtering = other.filtering;
			this->fmt = other.fmt;
			this->rgba_buffer = other.rgba_buffer;
			this->rgb_buffer = other.rgb_buffer;
			this->stb_data = other.stb_data;
		}
	};
}
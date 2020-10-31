#pragma once
#include <common.hpp>
#include <raw_buffer.hpp>

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

	enum class texture_format {
		invalid,
		rgb,
		rgba
	};

	struct texture {
	public:
		texture(const texture_id& id, const unsigned int& width, const unsigned int& height, const texture_format& fmt) {
			this->id = id;
			this->fmt = fmt;
			this->rgba_buffer = nullptr;
			this->rgb_buffer = nullptr;
			this->stb_data = nullptr;
			switch (fmt) {
			case texture_format::rgb:
				rgb_buffer = new raw_buffer<color_rgb>(width, height);
				break;
			case texture_format::rgba:
				rgba_buffer = new raw_buffer<color_rgba>(width, height);
				break;
			}
		}

		texture(void* tex_buffer, const texture_id& id, const unsigned int& width, const unsigned int& height, const texture_format& fmt) {
			this->id = id;
			this->fmt = fmt;
			this->rgba_buffer = nullptr;
			this->rgb_buffer = nullptr;
			this->stb_data = nullptr;
			switch (fmt) {
			case texture_format::rgb:
				rgb_buffer = new raw_buffer<color_rgb>(tex_buffer, width, height);
				break;
			case texture_format::rgba:
				rgba_buffer = new raw_buffer<color_rgba>(tex_buffer, width, height);
				break;
			}
		}

		texture(const char* path, const texture_id& id) {
			this->id = id;
			this->fmt = texture_format::invalid;
			this->rgba_buffer = nullptr;
			this->rgb_buffer = nullptr;
			this->stb_data = nullptr;
			int width, height, channels;
			if (!FS::exists(path)) {
				std::cerr << "path does not exist" << std::endl;
			}
			else {
				unsigned char* tex = stbi_load(path, &width, &height, &channels, 4);
				this->stb_data = tex;
				if (channels == 3) {
					this->rgb_buffer = new raw_buffer<color_rgb>(tex, width, height);
					this->fmt = texture_format::rgb;
				}
				else if (channels == 4) {

					this->rgba_buffer = new raw_buffer<color_rgba>(tex, width, height);
					this->fmt = texture_format::rgba;
				}
				else {
					std::cerr << "invalid channels: " << channels << std::endl;
				}
			}
		}

		~texture() {
			if (rgb_buffer != nullptr) {
				delete rgb_buffer;
			}
			if (rgba_buffer != nullptr) {
				delete rgba_buffer;
			}
			if (stb_data != nullptr) {
				stbi_image_free(stb_data);
			}
		}

	public:
		texture_id id;
		wrap_mode wrap_mode;
		filtering filtering;
		texture_format fmt;

	private:
		raw_buffer<color_rgb>* rgb_buffer;
		raw_buffer<color_rgba>* rgba_buffer;
		unsigned char* stb_data;

	public:
		bool sample(const float& u, const float& v, color_rgba& ret) const {
			float wu = u;
			float wv = v;
			this->wrap(wu, wv);
			switch (fmt) {
				case texture_format::rgb:
				{
					color_rgb pixel;
					bool ok = false;
					if(rgb_buffer == nullptr) return ok;
					rgb_buffer->read(wu, wv, pixel);
					ret.r = pixel.r; ret.g = pixel.g; ret.b = pixel.b; ret.a = 0;
					return ok;
				}
				case texture_format::rgba:
				{
					if(rgba_buffer == nullptr) return false;
					return rgba_buffer->read(wu, wv, ret);
				}
			}
			return false;
		}

		bool sample(const unsigned int& x, const unsigned int& y, color_rgba& ret) const {
			switch (fmt) {
				case texture_format::rgb:
					{
						color_rgb pixel;
						bool ok = false;
						if(rgb_buffer == nullptr) return ok;
						rgb_buffer->read(x, y, pixel);
						ret.r = pixel.r; ret.g = pixel.g; ret.b = pixel.b; ret.a = 0;
						return ok;
					}
				case texture_format::rgba:
					{
						if (rgba_buffer == nullptr) return false;
						return rgba_buffer->read(x, y, ret);
					}
			}
			return false;
		}

		bool write(const unsigned int& x, const unsigned int& y, const color_rgba& data) {
			switch (fmt) {
				case texture_format::rgb:
					if (rgba_buffer == nullptr) return false;
					return rgb_buffer->write(x, y, { data.r, data.g, data.b});
				case texture_format::rgba:
					{
						if (rgba_buffer == nullptr) return false;
						return rgba_buffer->write(x, y, data);
					}
			}
			return false;
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
	};
}
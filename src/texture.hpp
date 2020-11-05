#pragma once
#include <guarneri.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.hpp>

namespace guarneri {
	class texture : public object{
	public:
		texture(const uint32_t& width, const uint32_t& height, const texture_format& fmt) {
			this->fmt = fmt;
			this->width = width;
			this->height = height;
			release();
			switch (fmt) {
				case texture_format::rgb:
					rgb_buffer = std::make_shared<raw_buffer<color_rgb>>(width, height);
					break;
				case texture_format::rgba:
					rgba_buffer = std::make_shared<raw_buffer<color_rgba>>(width, height);
					break;
			}
			std::cout << this->str() << " created" << std::endl;
		}

		texture(void* tex_buffer, const uint32_t& width, const uint32_t& height, const texture_format& fmt) {
			this->fmt = fmt;
			this->width = width;
			this->height = height;
			release();
			switch (fmt) {
				case texture_format::rgb:
				{
					rgb_buffer = std::make_shared<raw_buffer<color_rgb>>(tex_buffer, width, height, [](color_rgb* ptr) { delete[] ptr; });
				}
				break;
				case texture_format::rgba:
				{
					rgba_buffer = std::make_shared<raw_buffer<color_rgba>>(tex_buffer, width, height, [](color_rgba* ptr) { delete[] ptr; });
				}
				break;
			}
			std::cout << this->str() << " created" << std::endl;
		}

		texture(const char* path) {
			this->path = path;
			this->fmt = texture_format::invalid;
			release();
			int width, height, channels;
			if (!FS::exists(path)) {
				std::cerr << "create texture failed, path does not exist: " << path << std::endl;
			}
			else {
				stbi_set_flip_vertically_on_load(true);
				auto tex = stbi_load(path, &width, &height, &channels, 0);
				this->width = width;
				this->height = height;
				if (channels == 3) {
					rgb_buffer = std::make_shared<raw_buffer<color_rgb>>(tex, width, height, [](color_rgb* ptr) { stbi_image_free((void*)ptr); });
					this->fmt = texture_format::rgb;
				}
				else if (channels == 4) {
					rgba_buffer = std::make_shared<raw_buffer<color_rgba>>(tex, width, height, [](color_rgba* ptr) { stbi_image_free((void*)ptr); });
					this->fmt = texture_format::rgba;
				}
				else {
					std::cerr << "invalid channels: " << channels << std::endl;
				}
			}
			std::cout << this->str() << " created" << std::endl;
		}

		texture(const texture& other) {
			copy(other);
		}

		~texture() {
			release();
			tex_mgr().free(this->path);
			std::cout << this->str() << " freed." << std::endl;
		}

	public:
		wrap_mode wrap_mode;
		filtering filtering;
		texture_format fmt;

	private:
		static std::unordered_map<uint32_t, std::shared_ptr<texture>> texture_cache;
		std::shared_ptr<raw_buffer<color_rgb>> rgb_buffer;
		std::shared_ptr<raw_buffer<color_rgba>> rgba_buffer;
		std::string path;
		uint32_t width;
		uint32_t height;

	public:
		static std::shared_ptr<texture> create(const uint32_t& width, const uint32_t& height, const texture_format& fmt) {
			return std::make_shared<texture>(width, height, fmt);
		}

		static std::shared_ptr<texture> create(void* tex_buffer, const uint32_t& width, const uint32_t& height, const texture_format& fmt) {
			return std::make_shared<texture>(tex_buffer, width, height, fmt);
		}

		static std::shared_ptr<texture> create(const texture& other) {
			return std::make_shared<texture>(other);
		}

		static std::shared_ptr<texture> create(const std::string& path) {
			std::shared_ptr<texture> ret = nullptr;
			if (tex_mgr().get(path, ret)) {
				return ret;
			}
			ret = std::make_shared<texture>(path.c_str());
			tex_mgr().cache(path, ret);
			return ret;
		}

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

		texture& operator =(const texture& other) {
			copy(other);
			return *this;
		}

		void copy(const texture& other) {
			this->id = other.id;
			this->wrap_mode = other.wrap_mode;
			this->filtering = other.filtering;
			this->fmt = other.fmt;
			this->rgba_buffer = other.rgba_buffer;
			this->rgb_buffer = other.rgb_buffer;
		}

		public:
			std::string str() const {
				std::stringstream ss;
				ss << "Texture[" << this->id << "], w["<< this->width <<"], h["<< this->height << "], fmt[" << fmt << "], path[" << this->path << "]";
				return ss.str();
			}
	};
}
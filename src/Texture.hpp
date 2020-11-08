#ifndef _TEXTURE_
#define _TEXTURE_
#include <Guarneri.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.hpp>

namespace Guarneri {
	class Texture : public Object{
	public:
		Texture(const uint32_t& _width, const uint32_t& _height, const TextureFormat& _fmt) {
			this->fmt = _fmt;
			this->width = _width;
			this->height = _height;
			this->filtering = Filtering::BILINEAR;
			release();
			switch (fmt) {
				case TextureFormat::rgb:
					rgb_buffer = std::make_shared<RawBuffer<color_rgb>>(width, height);
					break;
				case TextureFormat::rgba:
					rgba_buffer = std::make_shared<RawBuffer<color_rgba>>(width, height);
					break;
			}
			std::cout << this->str() << " created" << std::endl;
		}

		Texture(void* tex_buffer, const uint32_t& _width, const uint32_t& _height, const TextureFormat& _fmt) {
			this->fmt = _fmt;
			this->width = _width;
			this->height = _height;
			this->filtering = Filtering::BILINEAR;
			release();
			switch (fmt) {
				case TextureFormat::rgb:
				{
					rgb_buffer = std::make_shared<RawBuffer<color_rgb>>(tex_buffer, width, height, [](color_rgb* ptr) { delete[] ptr; });
				}
				break;
				case TextureFormat::rgba:
				{
					rgba_buffer = std::make_shared<RawBuffer<color_rgba>>(tex_buffer, width, height, [](color_rgba* ptr) { delete[] ptr; });
				}
				break;
			}
			std::cout << this->str() << " created" << std::endl;
		}

		Texture(const char* path) {
			this->path = path;
			this->fmt = TextureFormat::INVALID;
			this->filtering = Filtering::BILINEAR;
			release();
			int w, h, channels;
			if (!FS::exists(path)) {
				std::cerr << "create Texture failed, path does not exist: " << path << std::endl;
			}
			else {
				stbi_set_flip_vertically_on_load(true);
				auto tex = stbi_load(path, &w, &h, &channels, 0);
				this->width = w;
				this->height = h;
				assert(channels > 0);
				assert(w <= 4096);
				assert(h <= 4096);
				if (channels == 3) {
					rgb_buffer = std::make_shared<RawBuffer<color_rgb>>(tex, w, h, [](color_rgb* ptr) { stbi_image_free((void*)ptr); });
					this->fmt = TextureFormat::rgb;
				}
				else if (channels == 4) {
					rgba_buffer = std::make_shared<RawBuffer<color_rgba>>(tex, w, h, [](color_rgba* ptr) { stbi_image_free((void*)ptr); });
					this->fmt = TextureFormat::rgba;
				}
				else {
					std::cerr << "invalid channels: " << channels << std::endl;
				}
			}
			std::cout << this->str() << " created" << std::endl;
		}

		Texture(const Texture& other) {
			copy(other);
		}

		~Texture() {
			release();
			tex_mgr().free(this->path);
			std::cout << this->str() << " freed." << std::endl;
		}

	public:
		WrapMode wrapMode;
		Filtering filtering;
		TextureFormat fmt;

	private:
		static std::unordered_map<uint32_t, std::shared_ptr<Texture>> texture_cache;
		std::shared_ptr<RawBuffer<color_rgb>> rgb_buffer;
		std::shared_ptr<RawBuffer<color_rgba>> rgba_buffer;
		std::string path;
		uint32_t width;
		uint32_t height;

	public:
		static std::shared_ptr<Texture> create(const uint32_t& width, const uint32_t& height, const TextureFormat& fmt) {
			return std::make_shared<Texture>(width, height, fmt);
		}

		static std::shared_ptr<Texture> create(void* tex_buffer, const uint32_t& width, const uint32_t& height, const TextureFormat& fmt) {
			return std::make_shared<Texture>(tex_buffer, width, height, fmt);
		}

		static std::shared_ptr<Texture> create(const Texture& other) {
			return std::make_shared<Texture>(other);
		}

		static std::shared_ptr<Texture> create(const std::string& path) {
			std::shared_ptr<Texture> ret = nullptr;
			if (tex_mgr().get(path, ret)) {
				return ret;
			}
			ret = std::make_shared<Texture>(path.c_str());
			tex_mgr().cache(path, ret);
			return ret;
		}

		bool bilinear(const float& u, const float& v, Color& ret) const {
			float rf = v * (float)this->width;
			float cf = u * (float)this->height;

			uint32_t row = FLOOR_UINT(rf - EPSILON);
			uint32_t col = FLOOR_UINT(cf - EPSILON);
		
			float frac_row = rf - (float)row;
			float frac_col = cf - (float)col;

			Color c00, c01, c11, c10;
			sample(row, col, c00);
			sample(row + 1, col, c01);
			sample(row + 1, col + 1, c11);
			sample(row, col + 1, c10);

			Color  a = c00 * (1.0f - frac_row) + c10 * frac_row;
			Color  b = c01 * (1.0f - frac_row) + c11 * frac_row;
			ret = a * (1.0f - frac_col) + b * frac_col;
			return true;
		}

		bool point(const float& u, const float& v, Color& ret) const {
			float wu = u;
			float wv = v;
			this->wrap(wu, wv);
			switch (fmt) {
			case TextureFormat::rgb:
			{
				if (rgb_buffer == nullptr) return false;
				color_rgb pixel;
				bool ok = rgb_buffer->read(wu, wv, pixel);
				ret = Color::decode(pixel);
				return ok;
			}
			case TextureFormat::rgba:
			{
				if (rgba_buffer == nullptr) return false;
				color_rgba pixel;
				bool ok = rgba_buffer->read(wu, wv, pixel);
				ret = Color::decode(pixel);
				return ok;
			}
			}
			return false;
		}

		bool sample(const float& u, const float& v, Color& ret) const {
			switch (this->filtering) {
			case Filtering::BILINEAR:
				return bilinear(u, v, ret);
			case Filtering::POINT:
				return point(u, v, ret);
			}
			return false;
		}

		bool sample(const uint32_t& row, const uint32_t& col, Color& ret) const {
			switch (fmt) {
				case TextureFormat::rgb:
				{
					if (rgb_buffer == nullptr) return false;
					color_rgb pixel;
					bool ok = rgb_buffer->read(row, col, pixel);
					ret = Color::decode(pixel);
					return ok;
				}
				case TextureFormat::rgba:
				{
					if (rgba_buffer == nullptr) return false;
					color_rgba pixel;
					bool ok = rgba_buffer->read(row, col, pixel);
					ret = Color::decode(pixel);
					return ok;
				}
			}
			return false;
		}

		bool write(const uint32_t& x, const uint32_t& y, const Color& data) {
			switch (fmt) {
				case TextureFormat::rgb:
					if (rgba_buffer == nullptr) return false;
					return rgb_buffer->write(x, y, Color::encode_rgb(data));
				case TextureFormat::rgba:
					{
						if (rgba_buffer == nullptr) return false;
						return rgba_buffer->write(x, y, Color::encode_rgba(data));
					}
			}
			return false;
		}

		void save2file() {
			switch (fmt) {
			case TextureFormat::rgb:
				break;
			case TextureFormat::rgba:
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
			switch (wrapMode) {
			case WrapMode::CLAMP:
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
			case WrapMode::REPEAT:
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
			case TextureFormat::rgb:
				rgb_buffer->clear(color_rgb());
				break;
			case TextureFormat::rgba:
				rgba_buffer->clear(color_rgba());
				break;
			}
		}

		Texture& operator =(const Texture& other) {
			copy(other);
			return *this;
		}

		void copy(const Texture& other) {
			this->id = other.id;
			this->wrapMode = other.wrapMode;
			this->filtering = other.filtering;
			this->fmt = other.fmt;
			this->rgba_buffer = other.rgba_buffer;
			this->rgb_buffer = other.rgb_buffer;
		}

		public:
			std::string str() const {
				std::stringstream ss;
				ss << "Texture[" << this->id << "], w["<< this->width <<"], h["<< this->height << "]" << path << "]";
				return ss.str();
			}
	};
}
#endif
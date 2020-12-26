#ifndef _TEXTURE_
#define _TEXTURE_
#include <CPURasterizer.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.hpp>

namespace Guarneri
{
	class Texture : public Object
	{
	public:
		WrapMode wrap_mode;
		Filtering filtering;
		TextureFormat fmt;
		std::string path;
		uint32_t width;
		uint32_t height;
		uint32_t mip_count;
		Filtering mip_filtering;

	private:
		static std::unordered_map<uint32_t, std::shared_ptr<Texture>> texture_cache;
		std::shared_ptr<RawBuffer<color_rgb>> rgb_buffer;
		std::shared_ptr<RawBuffer<color_rgba>> rgba_buffer;
		std::shared_ptr<RawBuffer<color_gray>> gray_buffer;
		std::shared_ptr<RawBuffer<color_rg>> rg_buffer;
		std::vector< std::shared_ptr<RawBuffer<color_gray>>> gray_mipmaps;
		std::vector< std::shared_ptr<RawBuffer<color_rgb>>> rgb_mipmaps;
		std::vector< std::shared_ptr<RawBuffer<color_rgba>>> rgba_mipmaps;
		std::vector< std::shared_ptr<RawBuffer<color_rg>>> rg_mipmaps;

	public:
		Texture(const uint32_t& _width, const uint32_t& _height, const TextureFormat& _fmt);
		Texture(void* tex_buffer, const uint32_t& _width, const uint32_t& _height, const TextureFormat& _fmt);
		Texture(const char* path);
		Texture(const Texture& other);
		~Texture();

		static std::shared_ptr<Texture> create(const uint32_t& width, const uint32_t& height, const TextureFormat& fmt);
		static std::shared_ptr<Texture> create(void* tex_buffer, const uint32_t& width, const uint32_t& height, const TextureFormat& fmt);
		static std::shared_ptr<Texture> create(const Texture& other);
		static std::shared_ptr<Texture> create(const std::string& path);
		bool bilinear(const float& u, const float& v, Color& ret) const;
		bool point(const float& u, const float& v, Color& ret) const;
		void generate_mipmap(const int& mip_count, const Filtering& filtering);
		bool sample(const float& u, const float& v, Color& ret) const;
		bool read(const float& u, const float& v, Color& ret) const;
		bool read(const uint32_t& row, const uint32_t& col, Color& ret) const;
		bool write(const uint32_t& x, const uint32_t& y, const Color& data);
		void save2file();
		void resize();
		void release();
		std::string str() const;

	private:
		//todo:
		void wrap(float& u, float& v) const;
		void clear();
		Texture& operator =(const Texture& other);
		void copy(const Texture& other);
	};

	Texture::Texture(const uint32_t& _width, const uint32_t& _height, const TextureFormat& _fmt)
	{
		this->mip_count = 0;
		this->fmt = _fmt;
		this->width = _width;
		this->height = _height;
		this->filtering = Filtering::BILINEAR;
		this->wrap_mode = WrapMode::CLAMP_TO_EDGE;
		release();
		switch (fmt)
		{
		case TextureFormat::rgb:
			rgb_buffer = RawBuffer<color_rgb>::create(width, height);
			break;
		case TextureFormat::rgba:
			rgba_buffer = RawBuffer<color_rgba>::create(width, height);
			break;
		case TextureFormat::rg:
			rg_buffer = RawBuffer<color_rg>::create(width, height);
			break;
		case TextureFormat::r32:
			gray_buffer = RawBuffer<color_gray>::create(width, height);
			break;
		}
		std::cout << this->str() << " created" << std::endl;
	}

	Texture::Texture(void* tex_buffer, const uint32_t& _width, const uint32_t& _height, const TextureFormat& _fmt)
	{
		this->mip_count = 0;
		this->fmt = _fmt;
		this->width = _width;
		this->height = _height;
		this->filtering = Filtering::BILINEAR;
		this->wrap_mode = WrapMode::CLAMP_TO_EDGE;
		release();
		switch (fmt)
		{
		case TextureFormat::rgb:
		{
			rgb_buffer = RawBuffer<color_rgb>::create(tex_buffer, width, height, [](color_rgb* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::rgba:
		{
			rgba_buffer = RawBuffer<color_rgba>::create(tex_buffer, width, height, [](color_rgba* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::rg:
		{
			rg_buffer = RawBuffer<color_rg>::create(tex_buffer, width, height, [](color_rg* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::r32:
		{
			gray_buffer = RawBuffer<color_gray>::create(tex_buffer, width, height, [](color_gray* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		}
		std::cout << this->str() << " created" << std::endl;
	}

	Texture::Texture(const char* path)
	{
		this->mip_count = 0;
		this->path = path;
		this->fmt = TextureFormat::INVALID;
		this->filtering = Filtering::BILINEAR;
		this->wrap_mode = WrapMode::CLAMP_TO_EDGE;
		release();
		int w, h, channels;
		if (!FS::exists(path))
		{
			std::cerr << "create texture failed, invalid path: " << path << std::endl;
		}
		else
		{
			stbi_set_flip_vertically_on_load(true);
			auto tex = stbi_load(path, &w, &h, &channels, 0);
			this->width = w;
			this->height = h;
			assert(channels > 0);
			assert(w <= TEXTURE_MAX_SIZE);
			assert(h <= TEXTURE_MAX_SIZE);
			if (channels == RGB_CHANNEL)
			{
				rgb_buffer = RawBuffer<color_rgb>::create(tex, w, h, [](color_rgb* ptr)
				{
					stbi_image_free((void*)ptr);
				});
				this->fmt = TextureFormat::rgb;
			}
			else if (channels == RGBA_CHANNEL)
			{
				rgba_buffer = RawBuffer<color_rgba>::create(tex, w, h, [](color_rgba* ptr)
				{
					stbi_image_free((void*)ptr);
				});
				this->fmt = TextureFormat::rgba;
			}
			else if (channels == RG_CHANNEL)
			{
				rg_buffer = RawBuffer<color_rg>::create(tex, w, h, [](color_rg* ptr)
				{
					stbi_image_free((void*)ptr);
				});
				this->fmt = TextureFormat::rg;
			}
			else if (channels == R_CHANNEL)
			{
				gray_buffer = RawBuffer<color_gray>::create(tex, w, h, [](color_gray* ptr)
				{
					stbi_image_free((void*)ptr);
				});
				this->fmt = TextureFormat::r32;
			}
			else
			{
				std::cerr << "invalid channels: " << channels << std::endl;
			}
		}
		std::cout << this->str() << " created" << std::endl;
	}

	Texture::Texture(const Texture& other)
	{
		copy(other);
	}

	Texture::~Texture()
	{
		release();
		TextureMgr().free(this->path);
		std::cout << this->str() << " freed." << std::endl;
	}

	std::shared_ptr<Texture> Texture::create(const uint32_t& width, const uint32_t& height, const TextureFormat& fmt)
	{
		return std::make_shared<Texture>(width, height, fmt);
	}

	std::shared_ptr<Texture> Texture::create(void* tex_buffer, const uint32_t& width, const uint32_t& height, const TextureFormat& fmt)
	{
		return std::make_shared<Texture>(tex_buffer, width, height, fmt);
	}

	std::shared_ptr<Texture> Texture::create(const Texture& other)
	{
		return std::make_shared<Texture>(other);
	}

	std::shared_ptr<Texture> Texture::create(const std::string& path)
	{
		std::shared_ptr<Texture> ret = nullptr;
		if (TextureMgr().get(path, ret))
		{
			return ret;
		}
		ret = std::make_shared<Texture>(path.c_str());
		TextureMgr().cache(path, ret);
		return ret;
	}

	bool Texture::bilinear(const float& u, const float& v, Color& ret) const
	{
		float rf = v * (float)this->width + 0.5f;
		float cf = u * (float)this->height + 0.5f;

		uint32_t row = (uint32_t)std::floor(rf);
		uint32_t col = (uint32_t)std::floor(cf);

		float frac_row = rf - (float)row;
		float frac_col = cf - (float)col;

		Color c00, c01, c11, c10;
		read(row, col, c00);
		read(row + 1, col, c01);
		read(row + 1, col + 1, c11);
		read(row, col + 1, c10);

		Color  a = c00 * (1.0f - frac_row) + c10 * frac_row;
		Color  b = c01 * (1.0f - frac_row) + c11 * frac_row;
		ret = a * (1.0f - frac_col) + b * frac_col;
		return true;
	}

	bool Texture::point(const float& u, const float& v, Color& ret) const
	{
		read(u, v, ret);
		return true;
	}

	void Texture::generate_mipmap(const int& count, const Filtering& mip_filter)
	{
		switch (this->fmt)
		{
		case TextureFormat::rgb:
		{
			for (int i = 1; i < count; i++)
			{
				rgb_mipmaps[i].reset();
			}
			rgb_mipmaps.clear();

			rgb_mipmaps.emplace_back(rgb_buffer);

			for (int i = 1; i < count; i++)
			{
				long w = this->width >> i;
				long h = this->height >> i;
				rgb_mipmaps.emplace_back(std::make_shared<RawBuffer<color_rgb>>(w, h));
			}
			break;
		}
		case TextureFormat::rgba:
		{
			for (int i = 1; i < count; i++)
			{
				rgba_mipmaps[i].reset();
			}
			rgba_mipmaps.clear();

			rgba_mipmaps.emplace_back(rgba_buffer);

			for (int i = 1; i < count; i++)
			{
				long w = this->width >> i;
				long h = this->height >> i;
				rgba_mipmaps.emplace_back(std::make_shared<RawBuffer<color_rgba>>(w, h));
			}
			break;
		}
		case TextureFormat::rg:
		{
			for (int i = 1; i < count; i++)
			{
				rg_mipmaps[i].reset();
			}
			rg_mipmaps.clear();

			rg_mipmaps.emplace_back(rg_buffer);

			for (int i = 1; i < count; i++)
			{
				long w = this->width >> i;
				long h = this->height >> i;
				rg_mipmaps.emplace_back(std::make_shared<RawBuffer<color_rg>>(w, h));
			}
			break;
		}
		case TextureFormat::r32:
		{
			for (int i = 1; i < count; i++)
			{
				gray_mipmaps[i].reset();
			}
			gray_mipmaps.clear();

			gray_mipmaps.emplace_back(gray_buffer);

			for (int i = 1; i < count; i++)
			{
				long w = this->width >> i;
				long h = this->height >> i;
				gray_mipmaps.emplace_back(std::make_shared<RawBuffer<color_gray>>(w, h));
			}
			break;
		}
		}

		this->mip_count = count;
		this->mip_filtering = mip_filter;
	}

	bool Texture::sample(const float& u, const float& v, Color& ret) const
	{
		switch (this->filtering)
		{
		case Filtering::BILINEAR:
			return bilinear(u, v, ret);
		case Filtering::POINT:
			return point(u, v, ret);
		}
		return false;
	}

	bool Texture::read(const float& u, const float& v, Color& ret) const
	{
		float wu = u;
		float wv = v;
		this->wrap(wu, wv);
		switch (fmt)
		{
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
		case TextureFormat::rg:
		{
			if (rg_buffer == nullptr) return false;
			color_rg pixel;
			bool ok = rg_buffer->read(wu, wv, pixel);
			ret = Color::decode(pixel);
			return ok;
		}
		case TextureFormat::r32:
		{
			if (gray_buffer == nullptr) return false;
			color_gray pixel;
			bool ok = gray_buffer->read(wu, wv, pixel);
			ret = Color::decode(pixel);
			return ok;
		}
		}
		return false;
	}

	bool Texture::read(const uint32_t& row, const uint32_t& col, Color& ret) const
	{
		switch (fmt)
		{
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
		case TextureFormat::rg:
		{
			if (rg_buffer == nullptr) return false;
			color_rg pixel;
			bool ok = rg_buffer->read(row, col, pixel);
			ret = Color::decode(pixel);
			return ok;
		}
		case TextureFormat::r32:
		{
			if (gray_buffer == nullptr) return false;
			color_gray pixel;
			bool ok = gray_buffer->read(row, col, pixel);
			ret = Color::decode(pixel);
			return ok;
		}
		}
		return false;
	}

	bool Texture::write(const uint32_t& x, const uint32_t& y, const Color& data)
	{
		switch (fmt)
		{
		case TextureFormat::rgb:
			if (rgba_buffer == nullptr) return false;
			return rgb_buffer->write(x, y, Color::encode_rgb(data));
		case TextureFormat::rgba:
		{
			if (rgba_buffer == nullptr) return false;
			return rgba_buffer->write(x, y, Color::encode_rgba(data));
		}
		case TextureFormat::rg:
		{
			if (rg_buffer == nullptr) return false;
			return rg_buffer->write(x, y, Color::encode_rg(data.r, data.g));
		}
		case TextureFormat::r32:
		{
			if (gray_buffer == nullptr) return false;
			return gray_buffer->write(x, y, Color::encode_gray(data));
		}
		}
		return false;
	}

	void Texture::save2file()
	{
		switch (fmt)
		{
		case TextureFormat::rgb:
			break;
		case TextureFormat::rgba:
			break;
		case TextureFormat::r32:
			break;
		}
	}

	// todo
	void Texture::resize()
	{

	}

	void Texture::release()
	{
		gray_buffer.reset();
		rg_buffer.reset();
		rgb_buffer.reset();
		rgba_buffer.reset();
		for (uint32_t i = 1; i < this->mip_count; i++)
		{
			switch (this->fmt)
			{
			case TextureFormat::rgb:
				this->rgb_mipmaps[i].reset();
				break;
			case TextureFormat::rgba:
				this->rgba_mipmaps[i].reset();
				break;
			case TextureFormat::rg:
				this->rg_mipmaps[i].reset();
				break;
			case TextureFormat::r32:
				this->gray_mipmaps[i].reset();
				break;
			}
		}
	}

	void Texture::wrap(float& u, float& v) const
	{
		switch (wrap_mode)
		{
		case WrapMode::CLAMP_TO_BORDER:
			if (LESS_EQUAL(u, 0.0f))
			{
				u = 0.0f;
			}
			if (GREATAER_EQUAL(u, 1.0f))
			{
				u = 1.0f;
			}
			if (LESS_EQUAL(v, 0.0f))
			{
				v = 0.0f;
			}
			if (GREATAER_EQUAL(v, 1.0f))
			{
				v = 1.0f;
			}
			break;
		case WrapMode::CLAMP_TO_EDGE:
			if (LESS_EQUAL(u, 0.0f))
			{
				u = 0.0f;
			}
			if (GREATAER_EQUAL(u, 1.0f))
			{
				u = 1.0f;
			}
			if (LESS_EQUAL(v, 0.0f))
			{
				v = 0.0f;
			}
			if (GREATAER_EQUAL(v, 1.0f))
			{
				v = 1.0f;
			}
			break;
		case WrapMode::REPEAT:
			if (LESS_EQUAL(u, 0.0f))
			{
				u += 1.0f;
			}
			if (GREATAER_EQUAL(u, 1.0f))
			{
				u -= 1.0f;
			}
			if (LESS_EQUAL(v, 0.0f))
			{
				v += 1.0f;
			}
			if (GREATAER_EQUAL(v, 1.0f))
			{
				v -= 1.0f;
			}
			break;
		}
	}

	void Texture::clear()
	{
		switch (fmt)
		{
		case TextureFormat::rgb:
			rgb_buffer->clear(color_rgb());
			break;
		case TextureFormat::rgba:
			rgba_buffer->clear(color_rgba());
			break;
		case TextureFormat::rg:
			rg_buffer->clear(color_rg());
			break;
		case TextureFormat::r32:
			gray_buffer->clear(color_gray());
			break;
		}
	}

	Texture& Texture::operator =(const Texture& other)
	{
		copy(other);
		return *this;
	}

	void Texture::copy(const Texture& other)
	{
		this->id = other.id;
		this->wrap_mode = other.wrap_mode;
		this->filtering = other.filtering;
		this->fmt = other.fmt;
		this->rgba_buffer = other.rgba_buffer;
		this->rgb_buffer = other.rgb_buffer;
		this->gray_buffer = other.gray_buffer;
		this->rg_buffer = other.rg_buffer;
		this->mip_count = other.mip_count;
		this->gray_mipmaps = other.gray_mipmaps;
		this->rg_mipmaps = other.rg_mipmaps;
		this->rgba_mipmaps = other.rgba_mipmaps;
		this->rgb_mipmaps = other.rgb_mipmaps;
	}

	std::string Texture::str() const
	{
		std::stringstream ss;
		ss << "Texture[" << this->id << "], w[" << this->width << "], h[" << this->height << "]" << path << "]";
		return ss.str();
	}
}
#endif
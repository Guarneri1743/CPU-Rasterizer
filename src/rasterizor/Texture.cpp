#include "Texture.hpp"
#include <iostream>
#include <filesystem>
#include "Singleton.hpp"
#include "Cache.hpp"
#include "Utility.hpp"
#include "Logger.hpp"
#include "Serialization.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

constexpr size_t kMaxTextureSize = 4096;
constexpr uint8_t kChannelR = 1;
constexpr uint8_t kChannelRG = 2;
constexpr uint8_t kChannelRGB = 3;
constexpr uint8_t kChannelRGBA = 4;

namespace Guarneri
{
	Texture::Texture()
	{
		wrap_mode = WrapMode::REPEAT;
		filtering = Filtering::POINT;
		format = TextureFormat::INVALID;
		width = 0;
		height = 0;
		mip_count = 0;
		mip_filtering = Filtering::POINT;
		enable_mip = true;
	}

	Texture::Texture(const Texture& other)
	{
		copy(other);
	}

	Texture::~Texture()
	{
		release();
		INST(Cache<Texture>).free(this->raw_path);
	}

	Texture::Texture(const size_t& _width, const size_t& _height, const TextureFormat& _fmt) : Texture()
	{
		release();
		width = _width;
		height = _height;
		format = _fmt;
		switch (format)
		{
		case TextureFormat::rgb:
			rgb_buffer = RawBuffer<tinymath::color_rgb>::create(width, height);
			break;
		case TextureFormat::rgba:
			rgba_buffer = RawBuffer<tinymath::color_rgba>::create(width, height);
			break;
		case TextureFormat::rg:
			rg_buffer = RawBuffer<tinymath::color_rg>::create(width, height);
			break;
		case TextureFormat::r32:
			gray_buffer = RawBuffer<tinymath::color_gray>::create(width, height);
			break;
		case TextureFormat::rgb16f:
			rgb16f_buffer = RawBuffer<tinymath::color_rgb16f>::create(width, height);
			break; 
		case TextureFormat::rgba16f:
			rgba16f_buffer = RawBuffer<tinymath::color_rgba16f>::create(width, height);
			break;
		}

		if (enable_mip)
		{
			generate_mipmap(kMaxMip, Filtering::POINT);
		}
	}

	Texture::Texture(void* tex_buffer, const size_t& _width, const size_t& _height, const TextureFormat& _fmt) : Texture()
	{
		release();
		width = _width;
		height = _height;
		format = _fmt;
		switch (format)
		{
		case TextureFormat::rgb:
		{
			rgb_buffer = RawBuffer<tinymath::color_rgb>::create(tex_buffer, width, height, [](tinymath::color_rgb* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::rgba:
		{
			rgba_buffer = RawBuffer<tinymath::color_rgba>::create(tex_buffer, width, height, [](tinymath::color_rgba* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::rg:
		{
			rg_buffer = RawBuffer<tinymath::color_rg>::create(tex_buffer, width, height, [](tinymath::color_rg* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::r32:
		{
			gray_buffer = RawBuffer<tinymath::color_gray>::create(tex_buffer, width, height, [](tinymath::color_gray* ptr)
			{
				delete[] ptr;
			});
		}
		break; 
		case TextureFormat::rgb16f:
		{
			rgb16f_buffer = RawBuffer<tinymath::color_rgb16f>::create(tex_buffer, width, height, [](tinymath::color_rgb16f* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::rgba16f:
		{
			rgba16f_buffer = RawBuffer<tinymath::color_rgba16f>::create(tex_buffer, width, height, [](tinymath::color_rgba16f* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		}

		if (enable_mip)
		{
			generate_mipmap(kMaxMip, Filtering::POINT);
		}
	}

	std::shared_ptr<Texture> Texture::load_asset(const char* path)
	{
		std::shared_ptr<Texture> ret = nullptr;
		if (INST(Cache<Texture>).get(path, ret) && ret != nullptr)
		{
			return ret;
		}
		Texture* tex = new Texture();
		Serializer::deserialize(path, *tex);
		ret = std::shared_ptr<Texture>(tex);
		INST(Cache<Texture>).put(path, ret);
		return ret;
	}

	std::shared_ptr<Texture> Texture::load_asset(const std::string& path)
	{
		return load_asset(path.c_str());
	}

	std::shared_ptr<Texture> Texture::load_raw(const std::string& path)
	{
		return load_raw(path.c_str());
	}

	std::shared_ptr<Texture> Texture::load_raw(const char* path)
	{
		std::shared_ptr<Texture> ret = std::shared_ptr<Texture>(new Texture());
		ret->raw_path = path;
		ret->reload(path); 
		return ret;
	}

	void Texture::reload(const char* texture_path)
	{
		release();

		std::string abs_path = RES_PATH + texture_path;
		int w, h, channels;
		if (!std::filesystem::exists(abs_path))
		{
			std::cerr << "create texture failed, invalid path: " << abs_path << std::endl;
		}
		else
		{
			stbi_set_flip_vertically_on_load(true);
			const char* path = abs_path.c_str();
			
			bool hdr = false;
			if (abs_path.compare(abs_path.size() - 3, 3, "hdr") == 0)
			{
				hdr = true;
			}
			
			void* tex = nullptr;

			if (hdr)
			{
				tex = (void*)stbi_loadf(path, &w, &h, &channels, 0);
			}
			else
			{
				tex = (void*)stbi_load(path, &w, &h, &channels, 0);
			}

			this->width = w;
			this->height = h;
			assert(channels > 0);
			assert(w <= kMaxTextureSize);
			assert(h <= kMaxTextureSize);
			if (channels == kChannelRGB)
			{
				if (hdr)
				{
					rgb16f_buffer = RawBuffer<tinymath::color_rgb16f>::create(tex, w, h, [](tinymath::color_rgb16f* ptr)
					{
						stbi_image_free((void*)ptr);
					});
					this->format = TextureFormat::rgb16f;
				}
				else
				{
					rgb_buffer = RawBuffer<tinymath::color_rgb>::create(tex, w, h, [](tinymath::color_rgb* ptr)
					{
						stbi_image_free((void*)ptr);
					});
					this->format = TextureFormat::rgb;
				}
			}
			else if (channels == kChannelRGBA)
			{
				if (hdr)
				{
					rgba16f_buffer = RawBuffer<tinymath::color_rgba16f>::create(tex, w, h, [](tinymath::color_rgba16f* ptr)
					{
						stbi_image_free((void*)ptr);
					});
					this->format = TextureFormat::rgba16f;
				}
				else
				{
					rgba_buffer = RawBuffer<tinymath::color_rgba>::create(tex, w, h, [](tinymath::color_rgba* ptr)
					{
						stbi_image_free((void*)ptr);
					});
					this->format = TextureFormat::rgba;
				}
			}
			else if (channels == kChannelRG)
			{
				rg_buffer = RawBuffer<tinymath::color_rg>::create(tex, w, h, [](tinymath::color_rg* ptr)
				{
					stbi_image_free((void*)ptr);
				});
				this->format = TextureFormat::rg;
			}
			else if (channels == kChannelR)
			{
				gray_buffer = RawBuffer<tinymath::color_gray>::create(tex, w, h, [](tinymath::color_gray* ptr)
				{
					stbi_image_free((void*)ptr);
				});
				this->format = TextureFormat::r32;
			}
			else
			{
				std::cerr << "invalid channels: " << channels << std::endl;
			}
		}

		if (enable_mip)
		{
			generate_mipmap(kMaxMip, Filtering::POINT);
		}

		LOG("raw texture loaded: {}", abs_path.c_str());
	}

	bool Texture::bilinear(const float& u, const float& v, const size_t& mip, tinymath::Color& ret) const
	{
		float rf = v * (float)this->width + 0.5f;
		float cf = u * (float)this->height + 0.5f;

		size_t row = (size_t)std::floor(rf);
		size_t col = (size_t)std::floor(cf);

		float frac_row = rf - (float)row;
		float frac_col = cf - (float)col;

		tinymath::Color c00, c01, c11, c10;
		read(row, col, mip, c00);
		read(row + 1, col, mip, c01);
		read(row + 1, col + 1, mip, c11);
		read(row, col + 1, mip, c10);

		tinymath::Color  a = c00 * (1.0f - frac_row) + c10 * frac_row;
		tinymath::Color  b = c01 * (1.0f - frac_row) + c11 * frac_row;
		ret = a * (1.0f - frac_col) + b * frac_col;
		return true;
	}

	bool Texture::point(const float& u, const float& v, const size_t& mip, tinymath::Color& ret) const
	{
		read(u, v, mip, ret);
		return true;
	}

	void Texture::generate_mipmap(const int& count, const Filtering& mip_filter)
	{
		switch (this->format)
		{
		case TextureFormat::rgb:
		{
			for (int i = 1; i < rgb_mipmaps.size(); i++)
			{
				rgb_mipmaps[i].reset();
			}
			rgb_mipmaps.clear();

			rgb_mipmaps.emplace_back(rgb_buffer);

			for (int i = 1; i < count; i++)
			{
				size_t w = this->width >> i;
				size_t h = this->height >> i;
				auto mipmap = std::make_shared<RawBuffer<tinymath::color_rgb>>(w, h);
			
				for (size_t row = 0; row < h; row++)
				{
					for (size_t col = 0; col < w; col++)
					{
						float u, v;
						pixel2uv(w, h, row, col, u, v);
						tinymath::color_rgb color;
						rgb_buffer->read(u, v, color);
						mipmap->write(row, col, color);
					}
				}

				rgb_mipmaps.emplace_back(mipmap);
			}

			break;
		}
		case TextureFormat::rgba:
		{
			for (int i = 1; i < rgba_mipmaps.size(); i++)
			{
				rgba_mipmaps[i].reset();
			}
			rgba_mipmaps.clear();

			rgba_mipmaps.emplace_back(rgba_buffer);

			for (int i = 1; i < count; i++)
			{
				size_t w = this->width >> i;
				size_t h = this->height >> i;
				auto mipmap = std::make_shared<RawBuffer<tinymath::color_rgba>>(w, h);

				for (size_t row = 0; row < h; row++)
				{
					for (size_t col = 0; col < w; col++)
					{
						float u, v;
						pixel2uv(w, h, row, col, u, v);
						tinymath::color_rgba color;
						rgba_buffer->read(u, v, color);
						mipmap->write(row, col, color);
					}
				}

				rgba_mipmaps.emplace_back(mipmap);
			}

			break;
		}
		case TextureFormat::rg:
		{
			for (int i = 1; i < rg_mipmaps.size(); i++)
			{
				rg_mipmaps[i].reset();
			}
			rg_mipmaps.clear();

			rg_mipmaps.emplace_back(rg_buffer);

			for (int i = 1; i < count; i++)
			{
				size_t w = this->width >> i;
				size_t h = this->height >> i;
				auto mipmap = std::make_shared<RawBuffer<tinymath::color_rg>>(w, h);

				for (size_t row = 0; row < h; row++)
				{
					for (size_t col = 0; col < w; col++)
					{
						float u, v;
						pixel2uv(w, h, row, col, u, v);
						tinymath::color_rg color;
						rg_buffer->read(u, v, color);
						mipmap->write(row, col, color);
					}
				}

				rg_mipmaps.emplace_back(mipmap);
			}
			break;
		}
		case TextureFormat::r32:
		{
			for (int i = 1; i < gray_mipmaps.size(); i++)
			{
				gray_mipmaps[i].reset();
			}
			gray_mipmaps.clear();

			gray_mipmaps.emplace_back(gray_buffer);

			for (int i = 1; i < count; i++)
			{
				size_t w = this->width >> i;
				size_t h = this->height >> i;
				auto mipmap = std::make_shared<RawBuffer<tinymath::color_gray>>(w, h);

				for (size_t row = 0; row < h; row++)
				{
					for (size_t col = 0; col < w; col++)
					{
						float u, v;
						pixel2uv(w, h, row, col, u, v);
						tinymath::color_gray color;
						gray_buffer->read(u, v, color);
						mipmap->write(row, col, color);
					}
				}

				gray_mipmaps.emplace_back(mipmap);
			}
			break;
		}
		case TextureFormat::rgb16f:
		{
			for (int i = 1; i < rgb16f_mipmaps.size(); i++)
			{
				rgb16f_mipmaps[i].reset();
			}
			rgb16f_mipmaps.clear();

			rgb16f_mipmaps.emplace_back(rgb16f_buffer);

			for (int i = 1; i < count; i++)
			{
				size_t w = this->width >> i;
				size_t h = this->height >> i;
				auto mipmap = std::make_shared<RawBuffer<tinymath::color_rgb16f>>(w, h);

				for (size_t row = 0; row < h; row++)
				{
					for (size_t col = 0; col < w; col++)
					{
						float u, v;
						pixel2uv(w, h, row, col, u, v);
						tinymath::color_rgb16f color;
						rgb16f_buffer->read(u, v, color);
						mipmap->write(row, col, color);
					}
				}

				rgb16f_mipmaps.emplace_back(mipmap);
			}
			break;
		}
		case TextureFormat::rgba16f:
		{
			for (int i = 1; i < rgba16f_mipmaps.size(); i++)
			{
				rgba16f_mipmaps[i].reset();
			}
			rgba16f_mipmaps.clear();

			rgba16f_mipmaps.emplace_back(rgba16f_buffer);

			for (int i = 1; i < count; i++)
			{
				size_t w = this->width >> i;
				size_t h = this->height >> i;
				auto mipmap = std::make_shared<RawBuffer<tinymath::color_rgba16f>>(w, h);

				for (size_t row = 0; row < h; row++)
				{
					for (size_t col = 0; col < w; col++)
					{
						float u, v;
						pixel2uv(w, h, row, col, u, v);
						tinymath::color_rgba16f color;
						rgba16f_buffer->read(u, v, color);
						mipmap->write(row, col, color);
					}
				}

				rgba16f_mipmaps.emplace_back(mipmap);
			}
			break;
		}
		}

		this->mip_count = count;
		this->mip_filtering = mip_filter;
	}

	bool Texture::sample(const float& u, const float& v, tinymath::Color& ret) const
	{
		return sample(u, v, 0ull, ret);
	}

	bool Texture::sample(const float& u, const float& v, const size_t& mip, tinymath::Color& ret) const
	{
		switch (this->filtering)
		{
		case Filtering::BILINEAR:
			return bilinear(u, v, mip, ret);
		case Filtering::POINT:
			return point(u, v, mip, ret);
		}
		return false;
	}

	bool Texture::sample(const float& u, const float& v, const float& lod, tinymath::Color& ret) const
	{
		return sample(u, v, (size_t)std::floor(lod * kMaxMip), ret);
	}

	bool Texture::read(const float& u, const float& v, tinymath::Color& ret) const
	{
		return read(u, v, 0, ret);
	}

	bool Texture::read(const size_t& row, const size_t& col, tinymath::Color& ret) const
	{
		return read(row, col, 0, ret);
	}

	bool Texture::read(const float& u, const float& v, const size_t& mip, tinymath::Color& ret) const
	{
		float wu = u;
		float wv = v;
		this->wrap(wu, wv);
		bool use_mip = enable_mip;
		switch (format)
		{
		case TextureFormat::rgb:
		{
			auto buffer = use_mip ? rgb_mipmaps[mip] : rgb_buffer;
			tinymath::color_rgb pixel;
			bool ok = buffer->read(u, v, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::rgba:
		{
			auto buffer = use_mip ? rgba_mipmaps[mip] : rgba_buffer;
			tinymath::color_rgba pixel;
			bool ok = buffer->read(u, v, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::rg:
		{
			auto buffer = use_mip ? rg_mipmaps[mip] : rg_buffer;
			tinymath::color_rg pixel;
			bool ok = buffer->read(u, v, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::r32:
		{
			auto buffer = use_mip ? gray_mipmaps[mip] : gray_buffer;
			tinymath::color_gray pixel;
			bool ok = buffer->read(u, v, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::rgb16f:
		{
			auto buffer = use_mip ? rgb16f_mipmaps[mip] : rgb16f_buffer;
			tinymath::color_rgb16f pixel;
			bool ok = buffer->read(u, v, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::rgba16f:
		{
			auto buffer = use_mip ? rgba16f_mipmaps[mip] : rgba16f_buffer;
			tinymath::color_rgba16f pixel;
			bool ok = buffer->read(u, v, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		}
		return false;
	}

	bool Texture::read(const size_t& row, const size_t& col, const size_t& mip, tinymath::Color& ret) const
	{
		switch (format)
		{
		case TextureFormat::rgb:
		{
			auto buffer = enable_mip ? rgb_mipmaps[mip] : rgb_buffer;
			tinymath::color_rgb pixel;
			bool ok = buffer->read(row, col, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::rgba:
		{
			auto buffer = enable_mip ? rgba_mipmaps[mip] : rgba_buffer;
			tinymath::color_rgba pixel;
			bool ok = buffer->read(row, col, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::rg:
		{
			auto buffer = enable_mip ? rg_mipmaps[mip] : rg_buffer;
			tinymath::color_rg pixel;
			bool ok = buffer->read(row, col, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::r32:
		{
			auto buffer = enable_mip ? gray_mipmaps[mip] : gray_buffer;
			tinymath::color_gray pixel;
			bool ok = buffer->read(row, col, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::rgb16f:
		{
			auto buffer = enable_mip ? rgb16f_mipmaps[mip] : rgb16f_buffer;
			tinymath::color_rgb16f pixel;
			bool ok = buffer->read(row, col, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::rgba16f:
		{
			auto buffer = enable_mip ? rgba16f_mipmaps[mip] : rgba16f_buffer;
			tinymath::color_rgba16f pixel;
			bool ok = buffer->read(row, col, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		}
		return false;
	}

	bool Texture::write(const size_t& x, const size_t& y, const tinymath::Color& data)
	{
		switch (format)
		{
		case TextureFormat::rgb:
		{
			return rgb_buffer->write(x, y, ColorEncoding::encode_rgb(data));
		}
		case TextureFormat::rgba:
		{
			return rgba_buffer->write(x, y, ColorEncoding::encode_rgba(data));
		}
		case TextureFormat::rg:
		{
			return rg_buffer->write(x, y, ColorEncoding::encode_rg(data.r, data.g));
		}
		case TextureFormat::r32:
		{
			return gray_buffer->write(x, y, ColorEncoding::encode_gray(data));
		}
		case TextureFormat::rgb16f:
		{
			return rgb16f_buffer->write(x, y, ColorEncoding::encode_rgb16f(data));
		}
		case TextureFormat::rgba16f:
		{
			return rgba16f_buffer->write(x, y, ColorEncoding::encode_rgba16f(data));
		}
		}
		return false;
	}

	bool Texture::write(const float& u, const float& v, const tinymath::Color& data)
	{
		float wu = u;
		float wv = v;
		this->wrap(wu, wv);
		switch (format)
		{
		case TextureFormat::rgb:
		{
			return rgb_buffer->write(wu, wv, ColorEncoding::encode_rgb(data));
		}
		case TextureFormat::rgba:
		{
			return rgba_buffer->write(wu, wv, ColorEncoding::encode_rgba(data));
		}
		case TextureFormat::rg:
		{
			return rg_buffer->write(wu, wv, ColorEncoding::encode_rg(data.r, data.g));
		}
		case TextureFormat::r32:
		{
			return gray_buffer->write(wu, wv, ColorEncoding::encode_gray(data));
		}
		case TextureFormat::rgb16f:
		{
			return rgb16f_buffer->write(wu, wv, ColorEncoding::encode_rgb16f(data));
		}
		case TextureFormat::rgba16f:
		{
			return rgba16f_buffer->write(wu, wv, ColorEncoding::encode_rgba16f(data));
		}
		}
		return false;
	}

	void Texture::resize(const size_t& w, const size_t& h)
	{
		switch (format)
		{
		case TextureFormat::rgb:
		{
			return rgb_buffer->resize(w, h);
		}
		case TextureFormat::rgba:
		{
			return rgba_buffer->resize(w, h);
		}
		case TextureFormat::rg:
		{
			return rg_buffer->resize(w, h);
		}
		case TextureFormat::r32:
		{
			return gray_buffer->resize(w, h);
		}
		case TextureFormat::rgb16f:
		{
			return rgb16f_buffer->resize(w, h);
		}
		case TextureFormat::rgba16f:
		{
			return rgba16f_buffer->resize(w, h);
		}
		}
	}

	void Texture::release()
	{
		gray_buffer.reset();
		rg_buffer.reset();
		rgb_buffer.reset();
		rgba_buffer.reset();
		rgb16f_buffer.reset();
		rgba16f_buffer.reset();
		for (size_t i = 1; i < this->mip_count; i++)
		{
			switch (this->format)
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

	void Texture::export_image(const Texture& tex, const std::string& path)
	{
		std::string abs_path = RES_PATH + path;
		const char* dest = abs_path.c_str();
		int ret = 0;
		switch (tex.format)
		{
		case TextureFormat::rgb:
		{
			size_t size;
			auto data = (*tex.rgb_buffer).get_ptr(size);
			ret = stbi_write_png(dest, (int)tex.width, (int)tex.height, 3, data, 0);
		}
			break;
		case TextureFormat::rgba:
		{
			size_t size;
			auto data = (*tex.rgba_buffer).get_ptr(size);
			ret = stbi_write_png(dest, (int)tex.width, (int)tex.height, 4, data, 0);
		}
			break;
		case TextureFormat::rg:
		{
			size_t size;
			auto data = (*tex.rg_buffer).get_ptr(size);
			ret = stbi_write_png(dest, (int)tex.width, (int)tex.height, 2, data, 0);
		}
			break;
		case TextureFormat::r32:
		{
			size_t size;
			auto data = (*tex.gray_buffer).get_ptr(size);
			ret = stbi_write_png(dest, (int)tex.width, (int)tex.height, 1, data, 0);
		}
			break;
		case TextureFormat::rgb16f:
		{
			size_t size;
			auto data = (*tex.rgb16f_buffer).get_ptr(size);
			ret = stbi_write_hdr(dest, (int)tex.width, (int)tex.height, 3, (float*)data);
		}
			break;
		case TextureFormat::rgba16f:
		{
			size_t size;
			auto data = (*tex.rgba16f_buffer).get_ptr(size);
			ret = stbi_write_hdr(dest, (int)tex.width, (int)tex.height, 4, (float*)data);
		}
			break;
		}

		LOG("export image: {}, ret: {}", abs_path.c_str(), ret);
	}

	void Texture::wrap(float& u, float& v) const
	{
		switch (wrap_mode)
		{
		case WrapMode::CLAMP_TO_BORDER:
			u = tinymath::clamp(u, 0.0f, 1.0f);
			v = tinymath::clamp(v, 0.0f, 1.0f);
			break;
		case WrapMode::CLAMP_TO_EDGE:
			u = tinymath::clamp(u, 0.0f, 1.0f);
			v = tinymath::clamp(v, 0.0f, 1.0f);
			break;
		case WrapMode::REPEAT:
			if (u <= 0.0f)
			{
				u += 1.0f;
			}
			if (u >= 1.0f)
			{
				u -= 1.0f;
			}
			if (v <= 0.0f)
			{
				v += 1.0f;
			}
			if (v >= 1.0f)
			{
				v -= 1.0f;
			}
			break;
		}
	}

	void Texture::clear()
	{
		switch (format)
		{
		case TextureFormat::rgb:
			rgb_buffer->clear(tinymath::color_rgb());
			break;
		case TextureFormat::rgba:
			rgba_buffer->clear(tinymath::color_rgba());
			break;
		case TextureFormat::rg:
			rg_buffer->clear(tinymath::color_rg());
			break;
		case TextureFormat::r32:
			gray_buffer->clear(tinymath::color_gray());
			break;
		case TextureFormat::rgb16f:
			rgb16f_buffer->clear(tinymath::color_rgb16f());
			break;
		case TextureFormat::rgba16f:
			rgba16f_buffer->clear(tinymath::color_rgba16f());
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
		this->format = other.format;
		this->rgba_buffer = other.rgba_buffer;
		this->rgb_buffer = other.rgb_buffer;
		this->rgb16f_buffer = other.rgb16f_buffer;
		this->rgba16f_buffer = other.rgba16f_buffer;
		this->gray_buffer = other.gray_buffer;
		this->rg_buffer = other.rg_buffer;
		this->mip_count = other.mip_count;
		this->gray_mipmaps = other.gray_mipmaps;
		this->rg_mipmaps = other.rg_mipmaps;
		this->rgba_mipmaps = other.rgba_mipmaps;
		this->rgb_mipmaps = other.rgb_mipmaps;
	}
}
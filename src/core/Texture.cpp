#include "Texture.hpp"
#include <iostream>
#include <filesystem>
#include "Singleton.hpp"
#include "Cache.hpp"
#include "Utility.hpp"
#include "Logger.hpp"
#include "Serialization.hpp"
#include "Sampling.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

constexpr size_t kMaxTextureSize = 4096;
constexpr uint8_t kChannelR = 1;
constexpr uint8_t kChannelRG = 2;
constexpr uint8_t kChannelRGB = 3;
constexpr uint8_t kChannelRGBA = 4;

static Cache<CpuRasterizor::Texture> texture_cache;

namespace CpuRasterizor
{
	Texture::Texture()
	{
		wrap_mode = WrapMode::kRepeat;
		filtering = Filtering::kPoint;
		format = TextureFormat::kInvalid;
		width = 0;
		height = 0;
		mip_count = 0;
		mip_filtering = Filtering::kPoint;
		enable_mip = true;
	}

	Texture::Texture(const Texture& other)
	{
		copy(other);
	}

	Texture::~Texture()
	{
		release();
		texture_cache.free(this->raw_path);
	}

	Texture::Texture(size_t _width, size_t _height, TextureFormat _fmt) : Texture()
	{
		release();
		width = _width;
		height = _height;
		format = _fmt;
		switch (format)
		{
		case TextureFormat::kRGB:
			rgb_buffer = RawBuffer<tinymath::color_rgb>::create(width, height);
			break;
		case TextureFormat::kRGBA:
			rgba_buffer = RawBuffer<tinymath::color_rgba>::create(width, height);
			break;
		case TextureFormat::kRG:
			rg_buffer = RawBuffer<tinymath::color_rg>::create(width, height);
			break;
		case TextureFormat::kGray:
			gray_buffer = RawBuffer<tinymath::color_gray>::create(width, height);
			break;
		case TextureFormat::kRGB16:
			rgb16f_buffer = RawBuffer<tinymath::color_rgb16f>::create(width, height);
			break; 
		case TextureFormat::kRGBA16:
			rgba16f_buffer = RawBuffer<tinymath::color_rgba16f>::create(width, height);
			break;
		}

		if (enable_mip)
		{
			generate_mipmap(kMaxMip, Filtering::kPoint);
		}
	}

	Texture::Texture(void* tex_buffer, size_t _width, size_t _height, TextureFormat _fmt) : Texture()
	{
		release();
		width = _width;
		height = _height;
		format = _fmt;
		switch (format)
		{
		case TextureFormat::kRGB:
		{
			rgb_buffer = RawBuffer<tinymath::color_rgb>::create(tex_buffer, width, height, [](tinymath::color_rgb* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::kRGBA:
		{
			rgba_buffer = RawBuffer<tinymath::color_rgba>::create(tex_buffer, width, height, [](tinymath::color_rgba* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::kRG:
		{
			rg_buffer = RawBuffer<tinymath::color_rg>::create(tex_buffer, width, height, [](tinymath::color_rg* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::kGray:
		{
			gray_buffer = RawBuffer<tinymath::color_gray>::create(tex_buffer, width, height, [](tinymath::color_gray* ptr)
			{
				delete[] ptr;
			});
		}
		break; 
		case TextureFormat::kRGB16:
		{
			rgb16f_buffer = RawBuffer<tinymath::color_rgb16f>::create(tex_buffer, width, height, [](tinymath::color_rgb16f* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::kRGBA16:
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
			generate_mipmap(kMaxMip, Filtering::kPoint);
		}
	}

	std::shared_ptr<Texture> Texture::load_asset(const char* path)
	{
		std::shared_ptr<Texture> ret = nullptr;
		if (texture_cache.get(path, ret) && ret != nullptr)
		{
			return ret;
		}
		Texture* tex = new Texture();
		Serializer::deserialize(path, *tex);
		ret = std::shared_ptr<Texture>(tex);
		texture_cache.put(path, ret);
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
					this->format = TextureFormat::kRGB16;
				}
				else
				{
					rgb_buffer = RawBuffer<tinymath::color_rgb>::create(tex, w, h, [](tinymath::color_rgb* ptr)
					{
						stbi_image_free((void*)ptr);
					});
					this->format = TextureFormat::kRGB;
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
					this->format = TextureFormat::kRGBA16;
				}
				else
				{
					rgba_buffer = RawBuffer<tinymath::color_rgba>::create(tex, w, h, [](tinymath::color_rgba* ptr)
					{
						stbi_image_free((void*)ptr);
					});
					this->format = TextureFormat::kRGBA;
				}
			}
			else if (channels == kChannelRG)
			{
				rg_buffer = RawBuffer<tinymath::color_rg>::create(tex, w, h, [](tinymath::color_rg* ptr)
				{
					stbi_image_free((void*)ptr);
				});
				this->format = TextureFormat::kRG;
			}
			else if (channels == kChannelR)
			{
				gray_buffer = RawBuffer<tinymath::color_gray>::create(tex, w, h, [](tinymath::color_gray* ptr)
				{
					stbi_image_free((void*)ptr);
				});
				this->format = TextureFormat::kGray;
			}
			else
			{
				std::cerr << "invalid channels: " << channels << std::endl;
			}
		}

		if (enable_mip)
		{
			generate_mipmap(kMaxMip, Filtering::kPoint);
		}

		LOG("raw texture loaded: {}", abs_path.c_str());
	}

	bool Texture::bilinear(float u, float v, size_t mip, tinymath::Color& ret) const
	{
		size_t row, col;
		float frac_row, frac_col;
		uv2pixel(width >> mip, height >> mip, u, v, row, col, frac_row, frac_col);
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

	void Texture::generate_mipmap(int count, Filtering mip_filter)
	{
		switch (this->format)
		{
		case TextureFormat::kRGB:
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
		case TextureFormat::kRGBA:
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
		case TextureFormat::kRG:
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
		case TextureFormat::kGray:
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
		case TextureFormat::kRGB16:
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
		case TextureFormat::kRGBA16:
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

	bool Texture::sample(float u, float v, tinymath::Color& ret) const
	{
		return read(u, v, ret);
	}

	bool Texture::sample(float u, float v, const tinymath::vec2f ddx, const tinymath::vec2f ddy, tinymath::Color& ret) const
	{
		float mip = get_mip_level(ddx, ddy, width, height);
		return sample(u, v, mip, ret);
	}

	bool Texture::sample(float u, float v, float mip, tinymath::Color& ret) const
	{
		int mip_int = (int)tinymath::floor(mip);
		float frac = mip - (float)mip_int;

		if (mip_int < kMaxMip - 1)
		{
			int next_mip = mip_int + 1;
			tinymath::Color c1, c2;
			read(u, v, mip_int, c1);
			read(u, v, next_mip, c2);
			ret = c1 + (c2 - c1) * frac;
			return true;
		}
		else
		{
			return read(u, v, mip_int, ret);
		}
	}

	bool Texture::read(float u, float v, tinymath::Color& ret) const
	{
		return read(u, v, 0, ret);
	}

	bool Texture::read(size_t row, size_t col, tinymath::Color& ret) const
	{
		return read(row, col, 0, ret);
	}

	bool Texture::read(float u, float v, size_t mip, tinymath::Color& ret) const
	{
		float wu = u;
		float wv = v;
		this->wrap(wu, wv);
		bool use_mip = enable_mip;
		switch (format)
		{
		case TextureFormat::kRGB:
		{
			auto buffer = use_mip ? rgb_mipmaps[mip] : rgb_buffer;
			tinymath::color_rgb pixel;
			bool ok = buffer->read(u, v, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::kRGBA:
		{
			auto buffer = use_mip ? rgba_mipmaps[mip] : rgba_buffer;
			tinymath::color_rgba pixel;
			bool ok = buffer->read(u, v, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::kRG:
		{
			auto buffer = use_mip ? rg_mipmaps[mip] : rg_buffer;
			tinymath::color_rg pixel;
			bool ok = buffer->read(u, v, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::kGray:
		{
			auto buffer = use_mip ? gray_mipmaps[mip] : gray_buffer;
			tinymath::color_gray pixel;
			bool ok = buffer->read(u, v, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::kRGB16:
		{
			auto buffer = use_mip ? rgb16f_mipmaps[mip] : rgb16f_buffer;
			tinymath::color_rgb16f pixel;
			bool ok = buffer->read(u, v, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::kRGBA16:
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

	bool Texture::read(size_t row, size_t col, size_t mip, tinymath::Color& ret) const
	{
		switch (format)
		{
		case TextureFormat::kRGB:
		{
			auto buffer = enable_mip ? rgb_mipmaps[mip] : rgb_buffer;
			tinymath::color_rgb pixel;
			bool ok = buffer->read(row, col, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::kRGBA:
		{
			auto buffer = enable_mip ? rgba_mipmaps[mip] : rgba_buffer;
			tinymath::color_rgba pixel;
			bool ok = buffer->read(row, col, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::kRG:
		{
			auto buffer = enable_mip ? rg_mipmaps[mip] : rg_buffer;
			tinymath::color_rg pixel;
			bool ok = buffer->read(row, col, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::kGray:
		{
			auto buffer = enable_mip ? gray_mipmaps[mip] : gray_buffer;
			tinymath::color_gray pixel;
			bool ok = buffer->read(row, col, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::kRGB16:
		{
			auto buffer = enable_mip ? rgb16f_mipmaps[mip] : rgb16f_buffer;
			tinymath::color_rgb16f pixel;
			bool ok = buffer->read(row, col, pixel);
			ret = ColorEncoding::decode(pixel);
			return ok;
		}
		case TextureFormat::kRGBA16:
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

	bool Texture::write(size_t x, size_t y, const tinymath::Color& data)
	{
		switch (format)
		{
		case TextureFormat::kRGB:
		{
			return rgb_buffer->write(x, y, ColorEncoding::encode_rgb(data));
		}
		case TextureFormat::kRGBA:
		{
			return rgba_buffer->write(x, y, ColorEncoding::encode_rgba(data));
		}
		case TextureFormat::kRG:
		{
			return rg_buffer->write(x, y, ColorEncoding::encode_rg(data.r, data.g));
		}
		case TextureFormat::kGray:
		{
			return gray_buffer->write(x, y, ColorEncoding::encode_gray(data));
		}
		case TextureFormat::kRGB16:
		{
			return rgb16f_buffer->write(x, y, ColorEncoding::encode_rgb16f(data));
		}
		case TextureFormat::kRGBA16:
		{
			return rgba16f_buffer->write(x, y, ColorEncoding::encode_rgba16f(data));
		}
		}
		return false;
	}

	bool Texture::write(float u, float v, const tinymath::Color& data)
	{
		float wu = u;
		float wv = v;
		this->wrap(wu, wv);
		switch (format)
		{
		case TextureFormat::kRGB:
		{
			return rgb_buffer->write(wu, wv, ColorEncoding::encode_rgb(data));
		}
		case TextureFormat::kRGBA:
		{
			return rgba_buffer->write(wu, wv, ColorEncoding::encode_rgba(data));
		}
		case TextureFormat::kRG:
		{
			return rg_buffer->write(wu, wv, ColorEncoding::encode_rg(data.r, data.g));
		}
		case TextureFormat::kGray:
		{
			return gray_buffer->write(wu, wv, ColorEncoding::encode_gray(data));
		}
		case TextureFormat::kRGB16:
		{
			return rgb16f_buffer->write(wu, wv, ColorEncoding::encode_rgb16f(data));
		}
		case TextureFormat::kRGBA16:
		{
			return rgba16f_buffer->write(wu, wv, ColorEncoding::encode_rgba16f(data));
		}
		}
		return false;
	}

	void Texture::resize(size_t w, size_t h)
	{
		if (w == width && h == height)
		{
			return;
		}

		width = w;
		height = h;

		switch (format)
		{
		case TextureFormat::kRGB:
		{
			rgb_buffer->resize(w, h);
			for(int mip = 1; mip < rgb_mipmaps.size(); ++mip)
			{
				rgb_mipmaps[mip]->resize(w >> mip, h >> mip);
			}
			break;
		}
		case TextureFormat::kRGBA:
		{
			rgba_buffer->resize(w, h);
			for (int mip = 1; mip < rgba_mipmaps.size(); ++mip)
			{
				rgba_mipmaps[mip]->resize(w >> mip, h >> mip);
			}
			break;
		}
		case TextureFormat::kRG:
		{
			rg_buffer->resize(w, h);
			for (int mip = 1; mip < rg_mipmaps.size(); ++mip)
			{
				rg_mipmaps[mip]->resize(w >> mip, h >> mip);
			}
			break;
		}
		case TextureFormat::kGray:
		{
			gray_buffer->resize(w, h);
			for (int mip = 1; mip < gray_mipmaps.size(); ++mip)
			{
				gray_mipmaps[mip]->resize(w >> mip, h >> mip);
			}
			break;
		}
		case TextureFormat::kRGB16:
		{
			rgb16f_buffer->resize(w, h);
			for (int mip = 1; mip < rgb16f_mipmaps.size(); ++mip)
			{
				rgb16f_mipmaps[mip]->resize(w >> mip, h >> mip);
			}
			break;
		}
		case TextureFormat::kRGBA16:
		{
			rgba16f_buffer->resize(w, h);
			for (int mip = 1; mip < rgba16f_mipmaps.size(); ++mip)
			{
				rgba16f_mipmaps[mip]->resize(w >> mip, h >> mip);
			}
			break;
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
			case TextureFormat::kRGB:
				this->rgb_mipmaps[i].reset();
				break;
			case TextureFormat::kRGBA:
				this->rgba_mipmaps[i].reset();
				break;
			case TextureFormat::kRG:
				this->rg_mipmaps[i].reset();
				break;
			case TextureFormat::kGray:
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
		stbi_flip_vertically_on_write(true);
		switch (tex.format)
		{
		case TextureFormat::kRGB:
		{
			size_t size;
			auto data = (*tex.rgb_buffer).get_ptr(size);
			ret = stbi_write_png(dest, (int)tex.width, (int)tex.height, 3, data, 0);
		}
			break;
		case TextureFormat::kRGBA:
		{
			size_t size;
			auto data = (*tex.rgba_buffer).get_ptr(size);
			ret = stbi_write_png(dest, (int)tex.width, (int)tex.height, 4, data, 0);
		}
			break;
		case TextureFormat::kRG:
		{
			size_t size;
			auto data = (*tex.rg_buffer).get_ptr(size);
			ret = stbi_write_png(dest, (int)tex.width, (int)tex.height, 2, data, 0);
		}
			break;
		case TextureFormat::kGray:
		{
			size_t size;
			auto data = (*tex.gray_buffer).get_ptr(size);
			ret = stbi_write_png(dest, (int)tex.width, (int)tex.height, 1, data, 0);
		}
			break;
		case TextureFormat::kRGB16:
		{
			size_t size;
			auto data = (*tex.rgb16f_buffer).get_ptr(size);
			ret = stbi_write_hdr(dest, (int)tex.width, (int)tex.height, 3, (float*)data);
		}
			break;
		case TextureFormat::kRGBA16:
		{
			size_t size;
			auto data = (*tex.rgba16f_buffer).get_ptr(size);
			ret = stbi_write_hdr(dest, (int)tex.width, (int)tex.height, 4, (float*)data);
		}
			break;
		}

		LOG("export image: {}, ret: {}", abs_path.c_str(), ret);
		Logger::log(Logger::Severity::kLog,"export image: {}, ret: {}", abs_path.c_str(), ret);
	}

	void Texture::wrap(float& u, float& v) const
	{
		switch (wrap_mode)
		{
		case WrapMode::kClampToBorder:
			u = tinymath::clamp(u, 0.0f, 1.0f);
			v = tinymath::clamp(v, 0.0f, 1.0f);
			break;
		case WrapMode::kClampToEdge:
			u = tinymath::clamp(u, 0.0f, 1.0f);
			v = tinymath::clamp(v, 0.0f, 1.0f);
			break;
		case WrapMode::kRepeat:
			if (u < 0.0f)
			{
				u += 1.0f;
			}
			if (u > 1.0f)
			{
				u -= 1.0f;
			}
			if (v < 0.0f)
			{
				v += 1.0f;
			}
			if (v > 1.0f)
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
		case TextureFormat::kRGB:
			rgb_buffer->clear(tinymath::color_rgb());
			break;
		case TextureFormat::kRGBA:
			rgba_buffer->clear(tinymath::color_rgba());
			break;
		case TextureFormat::kRG:
			rg_buffer->clear(tinymath::color_rg());
			break;
		case TextureFormat::kGray:
			gray_buffer->clear(tinymath::color_gray());
			break;
		case TextureFormat::kRGB16:
			rgb16f_buffer->clear(tinymath::color_rgb16f());
			break;
		case TextureFormat::kRGBA16:
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
		this->wrap_mode = other.wrap_mode;
		this->filtering = other.filtering;
		this->format = other.format;
		this->raw_path = other.raw_path;
		this->meta_path = other.meta_path;
		this->enable_mip = other.enable_mip;
		this->height = other.height;
		this->width = other.width;
		this->mip_count = other.mip_count;
		this->mip_filtering = other.mip_filtering;
		this->rgba_buffer = other.rgba_buffer;
		this->rgb_buffer = other.rgb_buffer;
		this->rgb16f_buffer = other.rgb16f_buffer;
		this->rgba16f_buffer = other.rgba16f_buffer;
		this->gray_buffer = other.gray_buffer;
		this->rg_buffer = other.rg_buffer;
		this->gray_mipmaps = other.gray_mipmaps;
		this->rg_mipmaps = other.rg_mipmaps;
		this->rgba_mipmaps = other.rgba_mipmaps;
		this->rgb_mipmaps = other.rgb_mipmaps;
		this->rgb16f_mipmaps = other.rgb16f_mipmaps;
		this->rgba16f_mipmaps = other.rgba16f_mipmaps;
	}
}
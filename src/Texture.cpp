#include "Texture.hpp"
#include <sstream>
#include <iostream>
#include <filesystem>
#include "Singleton.hpp"
#include "ResourceManager.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "Utility.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.hpp>

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
	}

	Texture::Texture(const Texture& other)
	{
		copy(other);
	}

	Texture::~Texture()
	{
		release();
		INST(ResourceManager<Texture>).free(this->raw_path);
	}

	std::shared_ptr<Texture> Texture::create(const Texture& other)
	{
		return std::shared_ptr<Texture>(new Texture(other));
	}

	std::shared_ptr<Texture> Texture::create()
	{
		std::shared_ptr<Texture> ret = std::shared_ptr<Texture>(new Texture());
		return ret;
	}

	std::shared_ptr<Texture> Texture::create(const uint32_t& _width, const uint32_t& _height, const TextureFormat& _fmt)
	{
		std::shared_ptr<Texture> ret = std::shared_ptr<Texture>(new Texture());
		ret->release();
		ret->width = _width;
		ret->height = _height;
		ret->format = _fmt;
		switch (ret->format)
		{
		case TextureFormat::rgb:
			ret->rgb_buffer = RawBuffer<color_rgb>::create(ret->width, ret->height);
			break;
		case TextureFormat::rgba:
			ret->rgba_buffer = RawBuffer<color_rgba>::create(ret->width, ret->height);
			break;
		case TextureFormat::rg:
			ret->rg_buffer = RawBuffer<color_rg>::create(ret->width, ret->height);
			break;
		case TextureFormat::r32:
			ret->gray_buffer = RawBuffer<color_gray>::create(ret->width, ret->height);
			break;
		}
		return ret;
	}

	std::shared_ptr<Texture> Texture::create(void* tex_buffer, const uint32_t& _width, const uint32_t& _height, const TextureFormat& _fmt)
	{
		std::shared_ptr<Texture> ret = std::shared_ptr<Texture>(new Texture());
		ret->release();
		ret->width = _width;
		ret->height = _height;
		ret->format = _fmt;
		switch (ret->format)
		{
		case TextureFormat::rgb:
		{
			ret->rgb_buffer = RawBuffer<color_rgb>::create(tex_buffer, ret->width, ret->height, [](color_rgb* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::rgba:
		{
			ret->rgba_buffer = RawBuffer<color_rgba>::create(tex_buffer, ret->width, ret->height, [](color_rgba* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::rg:
		{
			ret->rg_buffer = RawBuffer<color_rg>::create(tex_buffer, ret->width, ret->height, [](color_rg* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		case TextureFormat::r32:
		{
			ret->gray_buffer = RawBuffer<color_gray>::create(tex_buffer, ret->width, ret->height, [](color_gray* ptr)
			{
				delete[] ptr;
			});
		}
		break;
		}
		return ret;
	}

	std::shared_ptr<Texture> Texture::create(const char* path)
	{
		std::shared_ptr<Texture> ret = nullptr;
		if (INST(ResourceManager<Texture>).get(path, ret))
		{
			return ret;
		}
		ret = std::shared_ptr<Texture>(new Texture());
		Texture::deserialize(path, *ret);
		INST(ResourceManager<Texture>).cache(path, ret);
		return ret;
	}

	std::shared_ptr<Texture> Texture::create(const std::string& path)
	{
		return create(path.c_str());
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
			auto tex = stbi_load(abs_path.c_str(), &w, &h, &channels, 0);
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
				this->format = TextureFormat::rgb;
			}
			else if (channels == RGBA_CHANNEL)
			{
				rgba_buffer = RawBuffer<color_rgba>::create(tex, w, h, [](color_rgba* ptr)
				{
					stbi_image_free((void*)ptr);
				});
				this->format = TextureFormat::rgba;
			}
			else if (channels == RG_CHANNEL)
			{
				rg_buffer = RawBuffer<color_rg>::create(tex, w, h, [](color_rg* ptr)
				{
					stbi_image_free((void*)ptr);
				});
				this->format = TextureFormat::rg;
			}
			else if (channels == R_CHANNEL)
			{
				gray_buffer = RawBuffer<color_gray>::create(tex, w, h, [](color_gray* ptr)
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
		std::cout << this->str() << " raw texture loaded" << std::endl;
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
		switch (this->format)
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
		switch (format)
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
		switch (format)
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
		switch (format)
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
		switch (format)
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

	void Texture::serialize(const Texture& tex, std::string path)
	{
		rapidjson::Document doc;
		doc.SetObject();

		rapidjson::Value raw_path;
		raw_path.SetString(tex.raw_path.c_str(), doc.GetAllocator());
		rapidjson::Value meta_path;
		meta_path.SetString(path.c_str(), doc.GetAllocator());
		doc.AddMember("raw_path", raw_path, doc.GetAllocator());
		doc.AddMember("meta_path", meta_path, doc.GetAllocator());
		doc.AddMember("filtering", (int32_t)tex.filtering, doc.GetAllocator());
		doc.AddMember("wrap_mode", (int32_t)tex.wrap_mode, doc.GetAllocator());
		doc.AddMember("format", (int32_t)tex.format, doc.GetAllocator());
		doc.AddMember("width", (uint32_t)tex.width, doc.GetAllocator());
		doc.AddMember("height", (uint32_t)tex.height, doc.GetAllocator());
		doc.AddMember("mip_count", (uint32_t)tex.mip_count, doc.GetAllocator());
		doc.AddMember("mip_filtering", (int32_t)tex.mip_filtering, doc.GetAllocator());

		std::filesystem::path abs_path(ASSETS_PATH + path);
		if (!std::filesystem::exists(abs_path.parent_path()))
		{
			std::filesystem::create_directories(abs_path.parent_path());
		}
		std::FILE* fd = fopen(abs_path.string().c_str(), "w+");
		if (fd != nullptr)
		{
			char write_buffer[256];
			rapidjson::FileWriteStream fs(fd, write_buffer, sizeof(write_buffer));
			rapidjson::PrettyWriter<rapidjson::FileWriteStream> material_writer(fs);
			doc.Accept(material_writer);
			fclose(fd);
			std::cout << "save texture: " << path << std::endl;
		}
		else
		{
			std::cout << "path does not exist: " << ASSETS_PATH + path << std::endl;
		}
	}

	void Texture::deserialize(std::string path, Texture & tex)
	{
		std::FILE* fd = fopen((ASSETS_PATH + path).c_str(), "r");
		if (fd != nullptr)
		{
			std::cout << "deserialize: " << ASSETS_PATH + path << std::endl;
			char read_buffer[256];
			rapidjson::FileReadStream fs(fd, read_buffer, sizeof(read_buffer));
			rapidjson::Document doc;
			doc.ParseStream(fs);
			tex.raw_path = doc["raw_path"].GetString();
			tex.meta_path = doc["meta_path"].GetString();
			tex.filtering = (Filtering)doc["filtering"].GetInt();
			tex.wrap_mode = (WrapMode)doc["wrap_mode"].GetInt();
			tex.format = (TextureFormat)doc["format"].GetInt();
			tex.width = doc["width"].GetUint();
			tex.height = doc["height"].GetUint();
			tex.mip_count = doc["mip_count"].GetUint();
			tex.mip_filtering = (Filtering)doc["mip_filtering"].GetInt();
			tex.reload(tex.raw_path.c_str());
			fclose(fd);
			std::cout << "read texture: " << path << std::endl;
		}
		else
		{
			std::cout << "path does not exist: " << ASSETS_PATH + path << std::endl;
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
		switch (format)
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
		this->format = other.format;
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
		ss << "Texture[" << this->id << "], w[" << this->width << "], h[" << this->height << "]" << raw_path << "]";
		return ss.str();
	}
}
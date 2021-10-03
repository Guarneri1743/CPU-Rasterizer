#pragma once
#include <stdint.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Define.hpp"
#include "Object.hpp"
#include "tinymath/color/Color.h"
#include "RawBuffer.hpp"

namespace Guarneri
{
	enum class WrapMode
	{
		REPEAT = 0,
		CLAMP_TO_EDGE = 1,
		CLAMP_TO_BORDER = 2
	};

	enum class Filtering
	{
		POINT = 0,
		BILINEAR = 1,
		MAX = 2,
		MIN = 3
	};

	enum class TextureFormat
	{
		INVALID = 0,
		rgb = 1,
		rgba = 2,
		rg = 3,
		r32 = 4,
		rgb16f = 5,
		rgba16f = 6
	};

	class Texture : public Object
	{
	public:
		WrapMode wrap_mode;
		Filtering filtering;
		TextureFormat format;
		std::string raw_path;
		std::string meta_path;
		size_t width;
		size_t height;
		size_t mip_count;
		Filtering mip_filtering;
		bool enable_mip;

	private:
		static std::unordered_map<size_t, std::shared_ptr<Texture>> texture_cache;
		std::shared_ptr<RawBuffer<tinymath::color_rgb16f>> rgb16f_buffer;
		std::shared_ptr<RawBuffer<tinymath::color_rgba16f>> rgba16f_buffer;
		std::shared_ptr<RawBuffer<tinymath::color_rgb>> rgb_buffer;
		std::shared_ptr<RawBuffer<tinymath::color_rgba>> rgba_buffer;
		std::shared_ptr<RawBuffer<tinymath::color_gray>> gray_buffer;
		std::shared_ptr<RawBuffer<tinymath::color_rg>> rg_buffer;
		std::vector< std::shared_ptr<RawBuffer<tinymath::color_gray>>> gray_mipmaps;
		std::vector< std::shared_ptr<RawBuffer<tinymath::color_rgb>>> rgb_mipmaps;
		std::vector< std::shared_ptr<RawBuffer<tinymath::color_rgba>>> rgba_mipmaps;
		std::vector< std::shared_ptr<RawBuffer<tinymath::color_rg>>> rg_mipmaps;
		std::vector< std::shared_ptr<RawBuffer<tinymath::color_rgb16f>>> rgb16f_mipmaps;
		std::vector< std::shared_ptr<RawBuffer<tinymath::color_rgba16f>>> rgba16f_mipmaps;

	public:
		Texture(const size_t& _width, const size_t& _height, const TextureFormat& _fmt);
		Texture(void* tex_buffer, const size_t& _width, const size_t& _height, const TextureFormat& _fmt);
		Texture(const Texture& other);
		~Texture();

		static std::shared_ptr<Texture> load_asset(const std::string& path);
		static std::shared_ptr<Texture> load_asset(const char* path);
		static std::shared_ptr<Texture> load_raw(const std::string& path);
		static std::shared_ptr<Texture> load_raw(const char* path);

		void reload(const char* texture_path);
		bool bilinear(const float& u, const float& v, const size_t& mip, tinymath::Color& ret) const;
		bool point(const float& u, const float& v, const size_t& mip, tinymath::Color& ret) const;
		void generate_mipmap(const int& mip_count, const Filtering& filtering);
		bool sample(const float& u, const float& v, tinymath::Color& ret) const;
		bool sample(const float& u, const float& v, const size_t& mip, tinymath::Color& ret) const;
		bool sample(const float& u, const float& v, const float& lod, tinymath::Color& ret) const;
		bool read(const float& u, const float& v, const size_t& mip, tinymath::Color& ret) const;
		bool read(const size_t& row, const size_t& col, const size_t& mip, tinymath::Color& ret) const;
		bool read(const float& u, const float& v, tinymath::Color& ret) const;
		bool read(const size_t& row, const size_t& col, tinymath::Color& ret) const;
		bool write(const size_t& x, const size_t& y, const tinymath::Color& data);
		bool write(const float& u, const float& v, const tinymath::Color& data);
		void resize(const size_t& w, const size_t& h);
		void release();
		static void export_image(const Texture& tex, const std::string& path);

	private:
		//todo:
		void wrap(float& u, float& v) const;
		void clear();
		Texture& operator =(const Texture& other);
		void copy(const Texture& other);
		Texture();
	};
}
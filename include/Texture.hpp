#ifndef _TEXTURE_
#define _TEXTURE_
#include <stdint.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Define.hpp"
#include "Object.hpp"
#include "Color.hpp"
#include "RawBuffer.hpp"
#include "rapidjson/document.h"

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
		r32 = 4
	};

	class Texture : public Object
	{
	public:
		WrapMode wrap_mode;
		Filtering filtering;
		TextureFormat format;
		std::string raw_path;
		std::string meta_path;
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
		Texture(const Texture& other);
		~Texture();

		static std::shared_ptr<Texture> create();
		static std::shared_ptr<Texture> create(const uint32_t& width, const uint32_t& height, const TextureFormat& format);
		static std::shared_ptr<Texture> create(void* tex_buffer, const uint32_t& width, const uint32_t& height, const TextureFormat& format);
		static std::shared_ptr<Texture> create(const Texture& other);
		static std::shared_ptr<Texture> create(const std::string& path);
		static std::shared_ptr<Texture> create(const char* path);
		static std::shared_ptr<Texture> load_raw(const std::string& path);
		static std::shared_ptr<Texture> load_raw(const char* path);

		void reload(const char* texture_path);
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
		static void serialize(const Texture& tex, std::string path);
		static void deserialize(std::string path, Texture& tex);
		std::string str() const;

	private:
		//todo:
		void wrap(float& u, float& v) const;
		void clear();
		Texture& operator =(const Texture& other);
		void copy(const Texture& other);
		Texture();
	};
}
#endif
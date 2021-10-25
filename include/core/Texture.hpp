#pragma once
#include <stdint.h>
#include <string>
#include <memory>
#include <vector>
#include "Define.hpp"
#include "tinymath.h"

namespace CpuRasterizer
{
	template<typename T>
	class RawBuffer;

	class Texture
	{
	public:
		Texture();

		// 3d ctor
		Texture(size_t width, size_t height, size_t layer_count, TextureFormat fmt);
		Texture(void* raw_buffer, size_t width, size_t height, size_t layer_count, TextureFormat fmt);

		// 2d ctor
		Texture(size_t width, size_t height, TextureFormat fmt);
		Texture(void* raw_buffer, size_t width, size_t height, TextureFormat fmt);

		~Texture();

		static std::shared_ptr<Texture> load_asset(const std::string& path);
		static std::shared_ptr<Texture> load_asset(const char* path);
		static std::shared_ptr<Texture> load_raw(const std::string& path);
		static std::shared_ptr<Texture> load_raw(const char* path);

		void reload(const char* texture_path);
		void generate_mipmap(int mip_count);

		// sample pixel 2d
		bool sample(float u, float v, tinymath::Color& ret) const;
		bool sample(float u, float v, const tinymath::vec2f ddx, const tinymath::vec2f ddy, tinymath::Color& ret) const;
		bool sample_mip(float u, float v, int mip, tinymath::Color& ret) const;
		bool sample_mip(float u, float v, float mip, tinymath::Color& ret) const;

		// sample pixel 3d 
		bool sample(float u, float v, float w, tinymath::Color& ret) const;
		bool sample(float u, float v, float w, const tinymath::vec2f ddx, const tinymath::vec2f ddy, tinymath::Color& ret) const;
		bool sample_mip(float u, float v, float w, int mip, tinymath::Color& ret) const;
		bool sample_mip(float u, float v, float w, float mip, tinymath::Color& ret) const;

		// linear pixel 2d
		bool linear(float u, float v, tinymath::Color& ret) const;

		// linear pixel 3d
		bool linear(float u, float v, float w, tinymath::Color& ret) const;

		// linear pixel 2d of specified mip level
		bool linear_mip(float u, float v, int mip, tinymath::Color& ret) const;

		// linear pixel 3d of specified mip level
		bool linear_mip(float u, float v, float w, int mip, tinymath::Color& ret) const;

		// read pixel 2d
		bool read(float u, float v, tinymath::Color& ret) const;
		bool read(size_t row, size_t col, tinymath::Color& ret) const;

		// read pixel 3d
		bool read(float u, float v, float w, tinymath::Color& ret) const;
		bool read(size_t row, size_t col, size_t layer, tinymath::Color& ret) const;
	
		// read pixel 2d of specified mip level
		bool read_mip(float u, float v, int mip, tinymath::Color& ret) const;
		bool read_mip(size_t row, size_t col, int mip, tinymath::Color& ret) const;

		// read pixel 3d of specified mip level
		bool read_mip(float u, float v, float w, int mip, tinymath::Color& ret) const;
		bool read_mip(size_t row, size_t col, size_t layer, int mip, tinymath::Color& ret) const;

		// write pixel 2d
		bool write(size_t row, size_t col, const tinymath::Color& data);
		bool write(float u, float v, const tinymath::Color& data);

		// write pixel 3d
		bool write(size_t row, size_t col, size_t layer, const tinymath::Color& data);
		bool write(float u, float v, float w, const tinymath::Color& data);

		void resize(size_t width, size_t height);
		void resize(size_t width, size_t height, size_t layer_count);

		inline size_t get_width() const { return width; }
		inline size_t get_height() const { return height; }

		void release();
		void copy(const Texture& other);

		// save to disk
		static void export_image(const Texture& tex, const std::string& path);

		Texture(const Texture& other);
		Texture& operator =(const Texture& other);

	private:
		void wrap(float& u, float& v) const;
		void wrap(float& u, float& v, float& w) const;
		void clear();

	public:
		WrapMode wrap_mode;
		Filtering filtering;
		TextureFormat format;
		std::string raw_path;
		std::string meta_path;
		size_t width;
		size_t height;
		size_t layer_count;
		int mip_count;
		bool enable_mip;

	private:
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
	};
}
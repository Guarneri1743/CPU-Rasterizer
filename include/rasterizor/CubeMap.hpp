#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Define.hpp"
#include "tinymath.h"

namespace CpuRasterizor
{
	class Texture;
	class CubeMap {
	public:
		CubeMap(const char* path);
		void reload(const std::string& path);
		bool sample(const tinymath::vec3f& dir, tinymath::Color& ret);
		bool sample_irradiance_map(const tinymath::vec3f& dir, tinymath::Color& ret);
		bool sample_prefilter_map(const tinymath::vec3f& dir, tinymath::Color& ret);
		bool sample_prefilter_map_lod(const tinymath::vec3f& dir, const float& lod, tinymath::Color& ret);
		bool sample_brdf(const tinymath::vec2f& uv, tinymath::Color& ret);
		void precompute_ibl_textures();
		void precompute_irradiance_map();
		void precompute_prefilter_map(const size_t& mip);
		void precompute_prefilter_map_fast(const size_t& mip);
		void precompute_brdf_lut();

		std::string texture_path;
		std::string meta_path;
		std::string name;
		WrapMode wrap_mode;
		Filtering filtering;
		void copy_from(const CubeMap& cubemap);

		static std::shared_ptr<CubeMap> load_asset(const char* path);
		static std::shared_ptr<CubeMap> load_asset(std::string path);
		static void spawn(std::string path, std::shared_ptr<CubeMap>& cubemap);

	private:
		std::shared_ptr<Texture> texture;
		std::shared_ptr<Texture> irradiance_map;
		std::vector<std::shared_ptr<Texture>> prefiltered_maps;
		std::shared_ptr<Texture> brdf_lut;

	private:
		CubeMap();
	};
}
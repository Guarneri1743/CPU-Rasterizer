#pragma once
#include <memory>
#include <vector>
#include <string>
#include "tinymath.h"
#include "Color.hpp"
#include "Texture.hpp"

namespace Guarneri
{
	class CubeMap {
	public:
		std::string texture_path;

	private:
		std::shared_ptr<Texture> texture;

		std::shared_ptr<Texture> irradiance_map;
		std::vector<std::shared_ptr<Texture>> prefiltered_maps;
		std::shared_ptr<Texture> brdf_lut;

	public:
		CubeMap(const char* path);
		void reload(const std::string& path);
		bool sample(const tinymath::vec3f& dir, Color& ret);
		bool sample_irradiance_map(const tinymath::vec3f& dir, Color& ret);
		bool sample_prefilter_map(const tinymath::vec3f& dir, Color& ret);
		bool sample_prefilter_map_lod(const tinymath::vec3f& dir, const float& lod, Color& ret);
		bool sample_brdf(const tinymath::vec2f& uv, Color& ret);
		void precompute_ibl_textures();
		void precompute_irradiance_map();
		void precompute_prefilter_map(const uint32_t& roughness);
		void precompute_brdf_lut();
		//tinymath::vec2f sample(const tinymath::vec3f& dir, int& index);
		std::string meta_path;
		std::string name;
		WrapMode wrap_mode;
		Filtering filtering;
		void copy_from(const CubeMap& cubemap);

		static std::shared_ptr<CubeMap> load_asset(const char* path);
		static std::shared_ptr<CubeMap> load_asset(std::string path);
		static void spawn(std::string path, std::shared_ptr<CubeMap>& cubemap);

	private:
		CubeMap();
	};
}
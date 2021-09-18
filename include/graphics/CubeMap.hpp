#ifndef _CUBEMAP_
#define _CUBEMAP_
#include <memory>
#include <vector>
#include <string>
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Color.hpp"
#include "Texture.hpp"

namespace Guarneri
{
	class CubeMap {
	private:
		std::shared_ptr<Texture> texture;
		std::string texture_path;

		std::shared_ptr<Texture> irradiance_map;
		std::shared_ptr<Texture> prefilter_map;
		std::shared_ptr<Texture> brdf_lut;

	public:
		CubeMap(const char* path);
		void reload(const std::string& path);
		bool sample(const Vector3& dir, Color& ret);
		bool sample_irradiance_map(const Vector3& dir, Color& ret);
		bool sample_prefilter_map(const Vector3& dir, Color& ret);
		bool sample_brdf(const Vector3& dir, Color& ret);
		void precompute_ibl_textures();
		void precompute_irradiance_map();
		void precompute_prefilter_map(const float& roughness);
		void precompute_brdf_lut();
		//Vector2 sample(const Vector3& dir, int& index);
		std::string meta_path;
		std::string name;
		WrapMode wrap_mode;
		Filtering filtering;
		void copy_from(const CubeMap& cubemap);

		static std::shared_ptr<CubeMap> load_asset(const char* path);
		static std::shared_ptr<CubeMap> load_asset(std::string path);
		static void serialize(const CubeMap& cube_map, std::string path);
		static void deserialize(std::string path, CubeMap& cubemap);
		static void spawn(std::string path, std::shared_ptr<CubeMap>& cubemap);

	private:
		CubeMap();
	};
}
#endif
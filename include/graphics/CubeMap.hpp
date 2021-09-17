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

	public:
		CubeMap(const char* path);
		CubeMap(const char* name, const std::string& path);
		void reload(const std::string& path);
		Vector2 sample_spherical_map(Vector3 dir);
		bool sample(const Vector3& dir, Color& ret);
		//Vector2 sample(const Vector3& dir, int& index);
		std::string meta_path;
		std::string name;
		WrapMode wrap_mode;
		Filtering filtering;

		static std::shared_ptr<CubeMap> load_asset(const char* path);
		static std::shared_ptr<CubeMap> load_asset(std::string path);
		static void serialize(const CubeMap& cube_map, std::string path);
		static 	void CubeMap::deserialize(std::string path, CubeMap& cubemap);

	private:
		CubeMap();
	};
}
#endif
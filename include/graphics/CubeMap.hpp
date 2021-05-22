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
		std::vector<std::shared_ptr<Texture>> textures;
		std::vector<std::string> texture_paths;

	public:
		void reload(const std::vector<std::string>& paths);
		bool sample(const Vector3& dir, Color& ret);
		Vector2 sample(const Vector3& dir, int& index);
		std::string meta_path;
		std::string name;
		WrapMode wrap_mode;
		Filtering filtering;

		static std::shared_ptr<CubeMap> create();
		static std::shared_ptr<CubeMap> create(const char* path);
		static std::shared_ptr<CubeMap> create(std::string path);
		static std::shared_ptr<CubeMap> create(const char* name, std::vector<std::string> path);
		static std::shared_ptr<CubeMap> create(std::string name, std::vector<std::string> path);
		static void serialize(const CubeMap& cube_map, std::string path);
		static 	void CubeMap::deserialize(std::string path, CubeMap& cubemap);

	private:
		CubeMap();
		CubeMap(const char* path);
		CubeMap(const char* name, const std::vector<std::string>& paths);
	};
}
#endif
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

	public:
		CubeMap(const std::vector<std::string>& path);
		bool sample(const Vector3& dir, Color& ret);
		Vector2 sample(const Vector3& dir, int& index);

		static std::shared_ptr<CubeMap> create(std::vector<std::string> path);
	};
}
#endif
#ifndef _CUBEMAP_
#define _CUBEMAP_
#include <Guarneri.hpp>

namespace Guarneri {
	class CubeMap {
	public:
		CubeMap(std::vector<std::string> path) {
			assert(path.size() == 6);
			auto right = Texture::create(path[0]);
			auto left = Texture::create(path[1]);
			auto top = Texture::create(path[2]);
			auto bottom = Texture::create(path[3]);
			auto front = Texture::create(path[4]);
			auto back = Texture::create(path[5]);
			textures.push_back(right);
			textures.push_back(left);
			textures.push_back(top);
			textures.push_back(bottom);
			textures.push_back(front);
			textures.push_back(back);
		}

	private:
		std::vector<std::shared_ptr<Texture>> textures;

	public:
		bool sample(const Vector3& dir, Color& ret) {
			int index = -1;
			auto uv = sample(dir, index);
			if (index >= 0 && index < textures.size()) {
				return textures[index]->sample(uv.x, uv.y, ret);
			}
			return false;
		}

		Vector2 sample(const Vector3& dir, int& index)
		{
			Vector3 dir_abs = Vector3::abs(dir);
			float ma;
			Vector2 uv;
			if (dir_abs.z >= dir_abs.x && dir_abs.z >= dir_abs.y)
			{
				index = dir.z < 0.0 ? 5 : 4;
				ma = 0.5f / dir_abs.z;
				uv = Vector2(dir.z < 0.0 ? -dir.x : dir.x, -dir.y);
			}
			else if (dir_abs.y >= dir_abs.x)
			{
				index = dir.y < 0.0 ? 3 : 2;
				ma = 0.5f / dir_abs.y;
				uv = Vector2(dir.x, dir.y < 0.0 ? -dir.z : dir.z);
			}
			else
			{
				index = dir.x < 0.0 ? 1 : 0;
				ma = 0.5f / dir_abs.x;
				uv = Vector2(dir.x < 0.0 ? dir.z : -dir.z, -dir.y);
			}
			return uv * ma + 0.5;
		}
	};
}
#endif
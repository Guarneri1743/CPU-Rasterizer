#ifndef _CUBEMAP_
#define _CUBEMAP_
#include <Guarneri.hpp>

namespace Guarneri {
	class CubeMap {
	public:
		CubeMap(const std::vector<std::string>& path) {
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
			for (auto& tex : textures) {
				tex->wrap_mode = WrapMode::CLAMP_TO_EDGE;
			}
		}

	private:
		std::vector<std::shared_ptr<Texture>> textures;

	public:
		static std::unique_ptr<CubeMap> create(std::vector<std::string> path) {
			return std::make_unique<CubeMap>(path);
		}

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
			Vector3 abs_dir = Vector3::abs(dir);
			float max_axis = std::max(std::max(abs_dir.x, abs_dir.y), abs_dir.z);
			Vector3 normalized_dir = dir / max_axis;
			if (abs_dir.x > abs_dir.y && abs_dir.x > abs_dir.z) {
				if (dir.x >= 0) {
					index = 0;
					return Vector2(1.0f - (normalized_dir.z + 1.0f) / 2.0f, (normalized_dir.y + 1.0f) / 2.0f);
				}
				else {
					index = 1;
					return Vector2((normalized_dir.z + 1.0f) / 2.0f, (normalized_dir.y + 1.0f) / 2.0f);
				}
			}
			else if (abs_dir.y > abs_dir.x && abs_dir.y > abs_dir.z) {
				if (dir.y >= 0) {
					index = 2;
					return Vector2((normalized_dir.x + 1.0f) / 2.0f, 1.0f - (normalized_dir.z + 1.0f) / 2.0f);
				}
				else {
					index = 3;
					return Vector2((normalized_dir.x + 1.0f) / 2.0f, (normalized_dir.z + 1.0f) / 2.0f);
				}
			}
			else {
				if (dir.z >= 0) {
					index = 4;
					return Vector2((normalized_dir.x + 1.0f) / 2.0f, (normalized_dir.y + 1.0f) / 2.0f);
				}
				else {
					index = 5;
					return Vector2(1.0f - (normalized_dir.x + 1.0f) / 2.0f, (normalized_dir.y + 1.0f) / 2.0f);
				}
			}
		}
	};
}
#endif
#pragma
#include <Guarneri.hpp>
#include <SkyboxShader.hpp>

namespace Guarneri {
	class CubeMap {
	public:
		CubeMap(std::vector<std::string> path) {
			assert(path.size() == 6);
			 right = Texture::create(path[0]);
			 left = Texture::create(path[1]);
			 top = Texture::create(path[2]);
			 bottom = Texture::create(path[3]);
			 front = Texture::create(path[4]);
			 back = Texture::create(path[5]);
		}

	private:
		std::shared_ptr<Texture> right;
		std::shared_ptr<Texture> left;
		std::shared_ptr<Texture> top;
		std::shared_ptr<Texture> bottom;
		std::shared_ptr<Texture> front;
		std::shared_ptr<Texture> back;
	};
}
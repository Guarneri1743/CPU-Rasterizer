#pragma once
#include <guarneri.hpp>

namespace guarneri {
	class mesh {
	public:
		mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, const material& material) {
			this->vertices = vertices;
			this->indices = indices;
			this->material = material;
		}

		mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices) {
			this->vertices = vertices;
			this->indices = indices;
		}

	public:
		std::vector<vertex> vertices;
		std::vector<uint32_t> indices;
		material material;
	};
}
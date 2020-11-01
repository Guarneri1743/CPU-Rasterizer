#pragma once
#include <guarneri.hpp>

namespace guarneri {
	class mesh {
	public:
		mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, const std::shared_ptr<material>& material) {
			this->vertices = vertices;
			this->indices = indices;
			this->material = material;
		}

		mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices) {
			this->vertices = vertices;
			this->indices = indices;
		}

		mesh(const mesh& other) {
			deep_copy(other);
		}

		~mesh() {
			material.reset();
			vertices.clear();
			indices.clear();
		}

	public:
		std::vector<vertex> vertices;
		std::vector<uint32_t> indices;
		std::shared_ptr<material> material;

		void operator =(const mesh& other) {
			deep_copy(other);
		}

		void deep_copy(const mesh& other) {
			this->vertices = other.vertices;
			this->indices = other.indices;
			this->material = other.material;
		}
	};
}
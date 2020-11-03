#pragma once
#include <guarneri.hpp>

namespace guarneri {
	class mesh {
	public:
		mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<material> material) {
			this->vertices = vertices;
			this->indices = indices;
			this->material = std::move(material);
		}

		mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices) {
			this->vertices = vertices;
			this->indices = indices;
		}

		mesh(const mesh& other) {
			deep_copy(other);
		}

		~mesh() {
		
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
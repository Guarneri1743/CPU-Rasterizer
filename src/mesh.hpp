#pragma once
#include <guarneri.hpp>

namespace guarneri {
	class mesh : public object{
	public:
		mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<material> material) {
			this->vertices = vertices;
			this->indices = indices;
			this->mat = std::move(material);
		}

		mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices) {
			this->vertices = vertices;
			this->indices = indices;
		}

		mesh(const mesh& other) {
			deep_copy(other);
		}

	public:
		std::vector<vertex> vertices;
		std::vector<uint32_t> indices;
		std::shared_ptr<material> mat;

	public:
		std::unique_ptr<mesh> create(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<material> material) {
			return std::make_unique<mesh>(vertices, indices, std::move(material));
		}

		std::unique_ptr<mesh> create(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices) {
			return std::make_unique<mesh>(vertices, indices);
		}

		std::unique_ptr<mesh> create(const mesh& other) {
			return std::make_unique<mesh>(other);
		}

		void operator =(const mesh& other) {
			deep_copy(other);
		}

		void deep_copy(const mesh& other) {
			this->vertices = other.vertices;
			this->indices = other.indices;
			this->mat = other.mat;
		}
	};
}
#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	class Mesh : public Object{
	public:
		Mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<Material> Material) {
			this->vertices = vertices;
			this->indices = indices;
			this->mat = std::move(Material);
		}

		Mesh(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices) {
			this->vertices = vertices;
			this->indices = indices;
		}

	public:
		std::vector<vertex> vertices;
		std::vector<uint32_t> indices;
		std::shared_ptr<Material> mat;

	public:
		std::unique_ptr<Mesh> create(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<Material> Material) {
			return std::make_unique<Mesh>(vertices, indices, std::move(Material));
		}

		std::unique_ptr<Mesh> create(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices) {
			return std::make_unique<Mesh>(vertices, indices);
		}

		std::unique_ptr<Mesh> create(const Mesh& other) {
			return std::make_unique<Mesh>(other);
		}

		std::string str() const {
			std::stringstream ss;
			ss << "Mesh[" << this->id << " vertices: " << vertices.size() << " indices: " << indices.size() << "]";
			return ss.str();
		}
	};
}
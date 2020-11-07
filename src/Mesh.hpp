#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	class Mesh : public Object{
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<Material> Material) {
			this->vertices = vertices;
			this->indices = indices;
			this->material = std::move(Material);
		}

		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
			this->vertices = vertices;
			this->indices = indices;
		}

	public:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::shared_ptr<Material> material;

	public:
		std::unique_ptr<Mesh> create(const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices, std::unique_ptr<Material> _material) {
			return std::make_unique<Mesh>(_vertices, _indices, std::move(_material));
		}

		std::unique_ptr<Mesh> create(const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices) {
			return std::make_unique<Mesh>(_vertices, _indices);
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
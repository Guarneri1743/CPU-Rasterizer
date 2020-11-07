#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	class Mesh : public Object{
	public:
		Mesh(const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices) {
			this->vertices = _vertices;
			this->indices = _indices;
		}

	public:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

	public:
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
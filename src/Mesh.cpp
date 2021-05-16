#include "Mesh.hpp"
#include <sstream>

namespace Guarneri
{
	Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices)
	{
		this->vertices = _vertices;
		this->indices = _indices;
	}

	Mesh::~Mesh()
	{}

	std::string Mesh::str() const
	{
		std::stringstream ss;
		ss << "Mesh[" << this->id << " vertices: " << vertices.size() << " indices: " << indices.size() << "]";
		return ss.str();
	}
}
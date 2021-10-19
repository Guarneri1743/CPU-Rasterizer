#include "Mesh.hpp"

namespace CpuRasterizor
{
	Mesh::Mesh() {}

	Mesh::Mesh(const Mesh & other)
	{
		this->vertices = other.vertices;
		this->indices = other.indices;
	}

	Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<size_t>& _indices)
	{
		this->vertices = _vertices;
		this->indices = _indices;
	}

	Mesh::~Mesh()
	{}

	Mesh& Mesh::operator=(const Mesh& other)
	{
		this->vertices = other.vertices;
		this->indices = other.indices;
		return *this;
	}
}
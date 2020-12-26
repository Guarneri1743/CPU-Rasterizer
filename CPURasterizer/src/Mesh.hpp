#ifndef _MESH_
#define _MESH_
#include <CPURasterizer.hpp>

namespace Guarneri
{
	class Mesh : public Object
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

	public:
		Mesh(const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices);
		~Mesh();
		std::string str() const;
	};


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
#endif
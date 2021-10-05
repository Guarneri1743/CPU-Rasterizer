#pragma once
#include <string>
#include <vector>
#include "Object.hpp"
#include "RasterAttributes.hpp"

namespace Guarneri
{
	class Mesh : public Object
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

	public:
		Mesh();
		Mesh(const Mesh& other);
		Mesh(const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices);
		~Mesh();
		Mesh& operator= (const Mesh& other);
	};
}
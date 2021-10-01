#ifndef _MESH_
#define _MESH_
#include <string>
#include <vector>
#include "Object.hpp"
#include "Vertex.hpp"
#include "rapidjson/document.h"

namespace Guarneri
{
	class Mesh : public Object
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

	public:
		Mesh(const Mesh& other);
		Mesh(const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices);
		~Mesh();
		Mesh& operator= (const Mesh& other);

		static rapidjson::Value serialize(rapidjson::Document& doc, const Mesh& mesh);
		static Mesh deserialize(const rapidjson::Value& v);
	};
}
#endif
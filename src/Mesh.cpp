#include "Mesh.hpp"
#include <sstream>

namespace Guarneri
{
	Mesh::Mesh(const Mesh & other)
	{
		this->vertices = other.vertices;
		this->indices = other.indices;
	}

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

	Mesh& Mesh::operator=(const Mesh& other)
	{
		this->vertices = other.vertices;
		this->indices = other.indices;
		return *this;
	}

	rapidjson::Value Mesh::serialize(rapidjson::Document& doc, const Mesh& mesh)
	{
		rapidjson::Value v;
		v.SetObject();

		rapidjson::Value indices;
		indices.SetArray();
		for (auto& i : mesh.indices)
		{
			indices.PushBack(i, doc.GetAllocator());
		}

		rapidjson::Value vertices;
		vertices.SetArray();
		for (auto& vert : mesh.vertices)
		{
			vertices.PushBack(Vertex::serialize(doc, vert), doc.GetAllocator());
		}

		v.AddMember("indices", indices, doc.GetAllocator());
		v.AddMember("vertices", vertices, doc.GetAllocator());

		return v;
	}

	Mesh Mesh::deserialize(const rapidjson::Value& v)
	{
		auto indices_array = v["indices"].GetArray();
		auto vertices_array = v["vertices"].GetArray();

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		for (rapidjson::SizeType idx = 0; idx < indices_array.Size(); idx++)
		{
			indices.emplace_back(indices_array[idx].GetUint());
		}

		for (rapidjson::SizeType idx = 0; idx < vertices_array.Size(); idx++)
		{
			vertices.emplace_back(Vertex::deserialize(vertices_array[idx].GetObject()));
		}

		return Mesh(vertices, indices);
	}
}
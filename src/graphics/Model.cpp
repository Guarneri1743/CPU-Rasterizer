#include "Model.hpp"
#include <sstream>
#include <filesystem>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "Utility.hpp"

namespace Guarneri
{
	std::vector<int> Model::free_names;
	int Model::current_name;

	Model::Model()
	{
		transform = std::make_unique<Transform>();
		name = "";
		raw_path = "";
		meta_path = "";
		flip_uv = false;
	}

	Model::Model(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material) : Model()
	{
		assert(vertices.size() != 0 && indices.size() != 0);
		assert(indices.size() % 3 == 0);
		meshes.emplace_back(Mesh(vertices, indices));
		this->material = material;
	}

	Model::Model(std::string path, bool flip) : Model()
	{
		this->raw_path = path;
		this->material = std::make_shared<Material>();
		this->flip_uv = flip;
		load_raw_internal(path);
	}

	void Model::load_raw_internal(std::string path)
	{
		std::string abs_path = RES_PATH + path;
		Assimp::Importer importer;
		auto flag = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
		if (flip_uv)
		{
			flag |= aiProcess_FlipUVs;
		}
		const aiScene* Scene = importer.ReadFile(abs_path, flag);

		if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
		{
			std::cerr << "load Model failed, path: " << abs_path << " error code: " << importer.GetErrorString() << std::endl;
			return;
		}
		meshes.clear();
		reload_mesh(Scene->mRootNode, Scene);
		std::cout << "load model: " << abs_path << " mesh count: " << this->meshes.size() << std::endl;
		importer.FreeScene();
	}

	Model::Model(const Model& other)
	{
		this->meshes = other.meshes;
		this->transform = std::unique_ptr<Transform>(new Transform(*other.transform));
		this->name = other.name + "(copy)";
		this->material = other.material;
		this->raw_path = other.raw_path;
		this->meta_path = other.meta_path;
		this->flip_uv = other.flip_uv;
	}

	Model::~Model()
	{}

	Model& Model::operator=(const Model & other)
	{
		this->meshes = other.meshes;
		this->transform = std::unique_ptr<Transform>(new Transform(*other.transform));
		this->name = other.name + "(copy)";
		this->material = other.material;
		this->raw_path = other.raw_path;
		this->meta_path = other.meta_path;
		this->flip_uv = other.flip_uv;
		return *this;
	}

	std::shared_ptr<Model> Model::load_asset(std::string path)
	{
		std::shared_ptr<Model> ret = std::shared_ptr<Model>(new Model());
		Model::deserialize(path, *ret);
		return ret;
	}

	std::shared_ptr<Model> Model::load_raw(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material)
	{
		return std::shared_ptr<Model>(new Model(vertices, indices, material));
	}

	std::shared_ptr<Model> Model::load_raw(std::string path, bool flip_uv)
	{
		return std::shared_ptr<Model>(new Model(path, flip_uv));
	}

	void Model::reload_mesh(aiNode* node, const aiScene* Scene)
	{
		//std::cout << "traverse_nodes: " << node->mName.C_Str() << ", mesh: " << node->mNumMeshes << std::endl;
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* Mesh = Scene->mMeshes[node->mMeshes[i]];
			load_vertices(Mesh);
		}
		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			reload_mesh(node->mChildren[i], Scene);
		}
	}

	void Model::load_vertices(aiMesh* ai_mesh)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		for (uint32_t i = 0; i < ai_mesh->mNumVertices; i++)
		{
			Vertex Vertex;
			Vector3 vector;

			vector.x = ai_mesh->mVertices[i].x;
			vector.y = ai_mesh->mVertices[i].y;
			vector.z = ai_mesh->mVertices[i].z;
			Vertex.position = vector;

			if (ai_mesh->HasNormals())
			{
				vector.x = ai_mesh->mNormals[i].x;
				vector.y = ai_mesh->mNormals[i].y;
				vector.z = ai_mesh->mNormals[i].z;
				Vertex.normal = vector;
			}

			if (ai_mesh->mTextureCoords[0])
			{
				Vector2 vec;

				vec.x = ai_mesh->mTextureCoords[0][i].x;
				vec.y = ai_mesh->mTextureCoords[0][i].y;
				Vertex.uv = vec;

				vector.x = ai_mesh->mTangents[i].x;
				vector.y = ai_mesh->mTangents[i].y;
				vector.z = ai_mesh->mTangents[i].z;
				Vertex.tangent = vector;

				vector.x = ai_mesh->mBitangents[i].x;
				vector.y = ai_mesh->mBitangents[i].y;
				vector.z = ai_mesh->mBitangents[i].z;
				Vertex.bitangent = vector;
			}
			else
				Vertex.uv = Vector2(0.0f, 0.0f);

			Vertex.color = Vertex.tangent;
			vertices.emplace_back(Vertex);
		}

		for (uint32_t i = 0; i < ai_mesh->mNumFaces; i++)
		{
			aiFace face = ai_mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				indices.emplace_back(face.mIndices[j]);
		}

		meshes.emplace_back(Mesh(vertices, indices));
	}

	void Model::serialize(const Model& model, std::string path)
	{
		rapidjson::Document doc;
		doc.SetObject();

		rapidjson::Value name;
		name.SetString(model.name.c_str(), doc.GetAllocator());
		doc.AddMember("name", name, doc.GetAllocator());

		rapidjson::Value raw_path;
		raw_path.SetString(model.raw_path.c_str(), doc.GetAllocator());
		doc.AddMember("raw_path", raw_path, doc.GetAllocator());

		if (model.raw_path == "" && model.meshes.size() > 0)
		{
			rapidjson::Value meshes;
			meshes.SetArray();
			for (auto& m : model.meshes)
			{
				meshes.PushBack(Mesh::serialize(doc, m), doc.GetAllocator());
			}
			doc.AddMember("meshes", meshes, doc.GetAllocator());
		}

		rapidjson::Value meta_path;
		meta_path.SetString(path.c_str(), doc.GetAllocator());
		doc.AddMember("meta_path", meta_path, doc.GetAllocator());

		rapidjson::Value material;
		if (model.material != nullptr)
		{
			material.SetString(model.material->meta_path.c_str(), doc.GetAllocator());
		}
		else
		{
			material.SetString("", doc.GetAllocator());
		}
		doc.AddMember("material", material, doc.GetAllocator());

		doc.AddMember("transform", Transform::serialize(doc, *model.transform), doc.GetAllocator());

		doc.AddMember("flip_uv", model.flip_uv, doc.GetAllocator());

		std::filesystem::path abs_path(ASSETS_PATH + path);
		if (!std::filesystem::exists(abs_path.parent_path()))
		{
			std::filesystem::create_directories(abs_path.parent_path());
		}
		std::FILE* fd = fopen(abs_path.string().c_str(), "w+");
		if (fd != nullptr)
		{
			char write_buffer[256];
			rapidjson::FileWriteStream fs(fd, write_buffer, sizeof(write_buffer));
			rapidjson::PrettyWriter<rapidjson::FileWriteStream> material_writer(fs);
			doc.Accept(material_writer);
			fclose(fd);
			std::cout << "save model: " << path << std::endl;
		}
		else
		{
			std::cout << "path does not exist: " << ASSETS_PATH + path << std::endl;
		}
	}

	void Model::deserialize(std::string path, Model & model)
	{
		std::FILE* fd = fopen((ASSETS_PATH + path).c_str(), "r");
		if (fd != nullptr)
		{
			char read_buffer[256];
			rapidjson::FileReadStream fs(fd, read_buffer, sizeof(read_buffer));
			rapidjson::Document doc;
			doc.ParseStream(fs);
			fclose(fd);
			
			model.name = doc["name"].GetString();
			if (model.name == "")
			{
				if (free_names.size() > 0)
				{
					int num = free_names.back();
					free_names.pop_back();
					model.name = "Model(" + std::to_string(num) + ")";
				}
				else
				{
					model.name = "Model(" + std::to_string(current_name++) + ")";
				}
			}
			model.raw_path = doc["raw_path"].GetString();
			model.meta_path = doc["meta_path"].GetString();
			model.flip_uv = doc["flip_uv"].GetBool();
			std::string material_path = doc["material"].GetString();

			if (model.raw_path != "")
			{
				model.load_raw_internal(model.raw_path);
			}
			else
			{
				rapidjson::Value meshes = doc["meshes"].GetArray();
				for (rapidjson::SizeType idx = 0; idx < meshes.Size(); idx++)
				{
					model.meshes.emplace_back(Mesh::deserialize(meshes[idx].GetObject()));
				}
			}

			if (material_path != "")
			{
				model.material = Material::load_asset(material_path);
			}
			else
			{
				model.material = std::make_shared<Material>();
			}

			model.transform = std::unique_ptr<Transform>(Transform::deserialize(doc["transform"].GetObject()));
			model.transform->name = model.name;
		}
		else
		{
			std::cout << "path does not exist: " << ASSETS_PATH + path << std::endl;
		}
	}

	std::string Model::str() const
	{
		std::stringstream ss;
		ss << "Model[" << this->id << " path: " << raw_path << " mesh count: " << meshes.size() << "]";
		return ss.str();
	}
}
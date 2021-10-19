#include "Model.hpp"
#include <filesystem>
#include <iostream>
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/Scene.h"
#include "Utility.hpp"
#include "Logger.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Transform.hpp"

namespace CpuRasterizor
{
	Model::Model()
	{
		transform = std::make_unique<Transform>();
		name = "";
		raw_path = "";
		meta_path = "";
		flip_uv = false;
	}

	Model::Model(const std::vector<Vertex>& vertices, const std::vector<size_t>& indices, std::shared_ptr<Material> _material) : Model()
	{
		assert(vertices.size() != 0 && indices.size() != 0);
		assert(indices.size() % 3 == 0);
		meshes.emplace_back(Mesh(vertices, indices));
		if (_material == nullptr)
		{
			material = std::make_shared<Material>();
		}
		else
		{
			this->material = _material;
		}
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

	Model& Model::operator=(const Model& other)
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

	std::shared_ptr<Model> Model::load_raw(const std::vector<Vertex>& vertices, const std::vector<size_t>& indices, std::shared_ptr<Material> material)
	{
		return std::shared_ptr<Model>(new Model(vertices, indices, material));
	}

	void Model::load_raw(std::string path, bool flip)
	{
		std::string abs_path = RES_PATH + path;
		Assimp::Importer importer;
		auto flag = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
		if (flip)
		{
			flag |= aiProcess_FlipUVs;
		}
		const aiScene* Scene = importer.ReadFile(abs_path, flag);

		if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
		{
			std::cerr << "load Model failed, path: " << abs_path << " error code: " << importer.GetErrorString() << std::endl;
			return;
		}

		raw_path = path;
		material = std::make_shared<Material>();
		flip_uv = flip;
		meshes.clear();
		reload_mesh(Scene->mRootNode, Scene);
		LOG("load model: {}, mesh count: {}", abs_path, meshes.size());
		importer.FreeScene();
	}

	const Transform* Model::get_transform() const 
	{ 
		return transform.get(); 
	}

	void Model::set_transform(Transform* _transform)
	{
		transform.reset();  transform = std::unique_ptr<Transform>(_transform); transform->set_model(this);
	}

	void Model::reload_mesh(aiNode* node, const aiScene* Scene)
	{
		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* Mesh = Scene->mMeshes[node->mMeshes[i]];
			load_vertices(Mesh);
		}
		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			reload_mesh(node->mChildren[i], Scene);
		}
	}

	void Model::load_vertices(aiMesh* ai_mesh)
	{
		std::vector<Vertex> vertices;
		std::vector<size_t> indices;

		for (size_t i = 0; i < ai_mesh->mNumVertices; i++)
		{
			Vertex Vertex;
			tinymath::vec3f vector;

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
				tinymath::vec2f vec;

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
				Vertex.uv = tinymath::vec2f(0.0f, 0.0f);

			Vertex.color = Vertex.tangent;
			vertices.emplace_back(Vertex);
		}

		for (size_t i = 0; i < ai_mesh->mNumFaces; i++)
		{
			aiFace face = ai_mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++)
				indices.emplace_back(face.mIndices[j]);
		}

		meshes.emplace_back(Mesh(vertices, indices));
	}
}
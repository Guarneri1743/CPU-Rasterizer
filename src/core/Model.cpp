#include "Model.hpp"
#include <sstream>
#include <filesystem>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Utility.hpp"
#include "Logger.hpp"

namespace Guarneri
{
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

	std::shared_ptr<Model> Model::load_raw(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material)
	{
		return std::shared_ptr<Model>(new Model(vertices, indices, material));
	}

	std::shared_ptr<Model> Model::load_raw(std::string path, bool flip)
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
			return nullptr;
		}

		Model* model = new Model();
		model->raw_path = path;
		model->material = std::make_shared<Material>();
		model->flip_uv = flip;
		model->meshes.clear();
		model->reload_mesh(Scene->mRootNode, Scene);
		LOG("load model: {}, mesh count: {}", abs_path, model->meshes.size());
		importer.FreeScene();
		return std::shared_ptr<Model>(model);
	}

	void Model::reload_mesh(aiNode* node, const aiScene* Scene)
	{
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

	std::string Model::str() const
	{
		std::stringstream ss;
		ss << "Model[" << this->id << " path: " << raw_path << " mesh count: " << meshes.size() << "]";
		return ss.str();
	}
}
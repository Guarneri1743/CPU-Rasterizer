#ifndef _MODEL_
#define _MODEL_
#include <Guarneri.hpp>
#include <assimp/Importer.hpp>
#include <assimp/Scene.h>
#include <assimp/postprocess.h>

namespace Guarneri {
	class Model : public Object {
	public:
		Model(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<Material> material) {
			if (vertices.size() == 0 || indices.size() == 0) {
				std::cerr << "load vertices failed." << std::endl;
			}
			assert(indices.size() % 3 == 0);
			auto m = std::make_unique<Mesh>(vertices, indices);
			meshes.emplace_back(std::move(m));
			this->material = std::move(material);
		}

		Model(std::string path) {
			this->material = std::make_shared<Material>();
			Assimp::Importer importer;
			const aiScene* Scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
			if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
			{
				std::cerr << "load Model failed, path: " << path << " error code: " << importer.GetErrorString() << std::endl;
				return;
			}
			auto parent_path = FS::path(path).parent_path();
			auto p = parent_path.string();
			parent_dir = p;
			traverse_nodes(Scene->mRootNode, Scene);
			importer.FreeScene();
		}

		~Model() {}

	public:
		std::vector<std::unique_ptr<Mesh>> meshes;
		Transform transform;
		std::shared_ptr<Material> material;

	private:
		std::string parent_dir;

	public:
		static std::unique_ptr<Model> create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<Material>& material) {
			return std::make_unique<Model>(vertices, indices, std::move(material));
		}

		static std::unique_ptr<Model> create(std::string path) {
			return std::make_unique<Model>(path);
		}

	private:
		void traverse_nodes(aiNode* node, const aiScene* Scene)
		{
			for (uint32_t i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* Mesh = Scene->mMeshes[node->mMeshes[i]];
				meshes.emplace_back(std::move(load_mesh(Mesh, Scene)));
			}
			for (uint32_t i = 0; i < node->mNumChildren; i++)
			{
				traverse_nodes(node->mChildren[i], Scene);
			}
		}

		std::unique_ptr<Mesh> load_mesh(aiMesh* ai_mesh, const aiScene* scene)
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

			aiMaterial* aiMat = scene->mMaterials[ai_mesh->mMaterialIndex];

			auto diffuse = load_textures(aiMat, aiTextureType_DIFFUSE);
			auto specular = load_textures(aiMat, aiTextureType_SPECULAR);
			auto normal = load_textures(aiMat, aiTextureType_HEIGHT);
			auto ao = load_textures(aiMat, aiTextureType_AMBIENT);

			if (diffuse != nullptr) {
				diffuse->filtering = Filtering::POINT;
			}
			if (specular != nullptr) {
				specular->filtering = Filtering::POINT;
			}
			if (normal != nullptr) {
				normal->filtering = Filtering::POINT;
			}
			if (ao != nullptr) {
				ao->filtering = Filtering::POINT;
			}

			material->set_texture(albedo_prop, diffuse);
			material->set_texture(specular_prop, specular);
			material->set_texture(normal_prop, normal);
			material->set_texture(ao_prop, ao);

			return std::make_unique<Mesh>(vertices, indices);
		}

		std::shared_ptr<Texture> load_textures(aiMaterial* ai_material, aiTextureType type) {
			std::shared_ptr<Texture> ret;
			for (unsigned int i = 0; i < ai_material->GetTextureCount(type); i++)
			{
				aiString str;
				ai_material->GetTexture(type, i, &str);
				std::string relative_path = str.C_Str();
				std::string tex_path = parent_dir + "/" + relative_path;
				ret = Texture::create(tex_path);
				break;
			}
			return ret;
		}

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "Model[" << this->id << " path: " << parent_dir << " Mesh count: " << meshes.size() << "]";
			return ss.str();
		}
	};
}
#endif
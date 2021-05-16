#ifndef _MODEL_
#define _MODEL_
#include <string>
#include <vector>
#include <memory>
#include <stdint.h>

#include "Mesh.hpp"
#include "Vertex.hpp"
#include "Material.hpp"
#include "Transform.hpp"
#include <assimp/Scene.h>

namespace Guarneri
{
	class Model : public Object
	{
	public:
		std::vector<std::unique_ptr<Mesh>> meshes;
		Transform transform;
		std::shared_ptr<Material> material;
		std::string parent_dir;

	public:
		Model(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<Material> material);
		Model(std::string path, bool flip_uv);
		~Model();
		static std::unique_ptr<Model> create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<Material>& material);
		static std::unique_ptr<Model> create(std::string path, bool flip_uv);
		void traverse_nodes(aiNode* node, const aiScene* Scene);
		std::unique_ptr<Mesh> load_mesh(aiMesh* ai_mesh, const aiScene* scene);
		std::shared_ptr<Texture> load_textures(aiMaterial* ai_material, aiTextureType type);
		std::string str() const;
	};
}
#endif
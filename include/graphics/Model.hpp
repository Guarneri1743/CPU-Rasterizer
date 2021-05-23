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
#include <vector>

namespace Guarneri
{
	class Model : public Object
	{
	public:
		std::vector<Mesh> meshes;
		std::unique_ptr<Transform> transform;
		std::shared_ptr<Material> material;
		std::string name;
		std::string raw_path;
		std::string meta_path;
		bool flip_uv;

	public:
		Model(const Model& other);
		~Model();
		Model& operator= (const Model& other);
		std::string str() const;

		static std::shared_ptr<Model> load_raw(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material);
		static std::shared_ptr<Model> load_raw(std::string path, bool flip_uv);
		static std::shared_ptr<Model> load_asset(std::string path);
		static void serialize(const Model& model, std::string path);
		static void deserialize(std::string path, Model& model);

	private:
		static std::unordered_map<uint32_t, std::shared_ptr<Model>> texture_cache;
		Model();
		Model(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material);
		Model(std::string path, bool flip_uv);
		void load_raw_internal(std::string path);
		void load_vertices(aiMesh* ai_mesh);
		void reload_mesh(aiNode* node, const aiScene* Scene);	
		static std::vector<int> free_names;
		static int current_name;
	};
}
#endif
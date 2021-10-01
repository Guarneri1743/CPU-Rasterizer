#ifndef _MODEL_
#define _MODEL_
#include <string>
#include <vector>
#include <memory>
#include <stdint.h>
#include <vector>

#include "assimp/Scene.h"

#include "Mesh.hpp"
#include "Vertex.hpp"
#include "Material.hpp"
#include "Transform.hpp"


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
		Model();
		Model(const Model& other);
		~Model();

		void load_raw(std::string path, bool flip_uv);
		const Transform* get_transform() const { return transform.get(); }

		Model& operator= (const Model& other);
		static std::shared_ptr<Model> load_raw(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material);
	
	private:
		static std::unordered_map<uint32_t, std::shared_ptr<Model>> texture_cache;
		Model(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material);
		void load_vertices(aiMesh* ai_mesh);
		void reload_mesh(aiNode* node, const aiScene* Scene);	
	};
}
#endif
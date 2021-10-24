#pragma once
#include <string>
#include <vector>
#include <memory>
#include <stdint.h>
#include <vector>
#include "Object.hpp"
#include "RasterAttributes.hpp"
#include "Mesh.hpp"

struct aiScene;
struct aiNode;
struct aiMesh;

namespace CpuRasterizer
{
	class Material;
	class Transform;

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
		const Transform* get_transform() const;
		void set_transform(Transform* _transform);

		Model& operator= (const Model& other);
		static std::shared_ptr<Model> load_raw(const std::vector<Vertex>& vertices, const std::vector<size_t>& indices, std::shared_ptr<Material> material);
	
	private:
		Model(const std::vector<Vertex>& vertices, const std::vector<size_t>& indices, std::shared_ptr<Material> material);
		void load_vertices(aiMesh* ai_mesh);
		void reload_mesh(aiNode* node, const aiScene* Scene);	
	};
}
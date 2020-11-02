#pragma once
#include <guarneri.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace guarneri {
	class model {
	public:
		model() {
		}

        model(const model& other) {
            deep_copy(other);
        }

        ~model() {
            for (auto& m : meshes) {
                m.reset();
            }
            for (auto& kv : texture_cache) {
                kv.second.reset();
            }
        }

    public:
        std::vector<std::shared_ptr<mesh>> meshes;
        std::unordered_map<std::string, std::shared_ptr<texture>> texture_cache;
        transform transform;

    private:
        std::string parent_dir;

    public:
        void load_from_vertices(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, const std::shared_ptr<material>& material) {
            if (vertices.size() == 0 || indices.size() == 0) {
                std::cerr << "load vertices failed." << std::endl;
            }
            assert(indices.size() % 3 == 0);
            auto m = std::make_shared<mesh>(vertices, indices, material);
            meshes.push_back(m);
        }

         void load_from_file(std::string path) {
             Assimp::Importer importer;
             const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
             if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
             {
                 std::cerr << "load model failed, path: " << path << " error code: " << importer.GetErrorString() << std::endl;
                 return;
             }
             auto parent_path = FS::path(path).parent_path();
             auto p = parent_path.string();
             parent_dir = p;
             traverse_nodes(scene->mRootNode, scene);
             importer.FreeScene();
        }

        void traverse_nodes(aiNode* node, const aiScene* scene)
        {
            for (uint32_t i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(load_mesh(mesh, scene));
            }
            for (uint32_t i = 0; i < node->mNumChildren; i++)
            {
                traverse_nodes(node->mChildren[i], scene);
            }
        }

        std::shared_ptr<mesh> load_mesh(aiMesh* ai_mesh, const aiScene* scene)
        {
            std::vector<vertex> vertices;
            std::vector<uint32_t> indices;

            for (uint32_t i = 0; i < ai_mesh->mNumVertices; i++)
            {
                vertex vertex;
                float3 vector; 
            
                vector.x = ai_mesh->mVertices[i].x;
                vector.y = ai_mesh->mVertices[i].y;
                vector.z = ai_mesh->mVertices[i].z;
                vertex.position = vector;
           
                if (ai_mesh->HasNormals())
                {
                    vector.x = ai_mesh->mNormals[i].x;
                    vector.y = ai_mesh->mNormals[i].y;
                    vector.z = ai_mesh->mNormals[i].z;
                    vertex.normal = vector;
                }

                if (ai_mesh->mTextureCoords[0]) 
                {
                    float2 vec;

                    vec.x = ai_mesh->mTextureCoords[0][i].x;
                    vec.y = ai_mesh->mTextureCoords[0][i].y;
                    vertex.uv = vec;
                
                    vector.x = ai_mesh->mTangents[i].x;
                    vector.y = ai_mesh->mTangents[i].y;
                    vector.z = ai_mesh->mTangents[i].z;
                    vertex.tangent = vector;
              
                    vector.x = ai_mesh->mBitangents[i].x;
                    vector.y = ai_mesh->mBitangents[i].y;
                    vector.z = ai_mesh->mBitangents[i].z;
                    vertex.bitangent = vector;
                }
                else
                    vertex.uv = float2(0.0f, 0.0f);

                vertices.push_back(vertex);
            }
            for (uint32_t i = 0; i < ai_mesh->mNumFaces; i++)
            {
                aiFace face = ai_mesh->mFaces[i];
                for (uint32_t j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }

            aiMaterial* aiMat = scene->mMaterials[ai_mesh->mMaterialIndex];

            auto diffuse = load_textures(aiMat, aiTextureType_DIFFUSE, albedo_prop);
            auto specular = load_textures(aiMat, aiTextureType_SPECULAR, specular_prop);
            auto normal = load_textures(aiMat, aiTextureType_HEIGHT, normal_prop);
            auto height = load_textures(aiMat, aiTextureType_AMBIENT, height_prop);
            
            auto mat = std::make_shared<material>();
            mat->set_texture(albedo_prop, diffuse);
            mat->set_texture(specular_prop, specular);
            mat->set_texture(normal_prop, normal);
            mat->set_texture(height_prop, height);

            return std::make_shared<mesh>(vertices, indices, mat);
        }

        std::shared_ptr<texture> load_textures(aiMaterial* mat, aiTextureType type, std::string property_name)
        {
            for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);
                std::string relative_path = str.C_Str();
                if (texture_cache.count(relative_path) <= 0) {
                    std::string tex_path = parent_dir + "/" + relative_path;
                    std::cerr << "load: " << tex_path << std::endl;
                    auto tex = std::make_shared<texture>(tex_path.c_str(), property_name);
                    texture_cache[relative_path] = tex;
                    return tex;
                }
                else {
                    auto kv = texture_cache.find(relative_path);
                    return kv->second;
                }
            }
            return nullptr;
        }

        void operator =(const model& other) {
            deep_copy(other);
        }

        void deep_copy(const model& other) {
            this->meshes = other.meshes;
            this->texture_cache = other.texture_cache;
            this->transform = other.transform;
            this->parent_dir = other.parent_dir;
        }
	};
}
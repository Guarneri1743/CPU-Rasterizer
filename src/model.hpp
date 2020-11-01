#pragma once
#include <guarneri.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace guarneri {
	class model {
	public:
		model() {
            parent_dir = "";
		}

        ~model() {
           
        }

    public:
        std::vector<mesh> meshes;
        std::unordered_map<std::string, texture*> texture_cache;
        transform transform;

    private:
        std::string parent_dir;

    public:
        void load_from_vertices(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, const material& material) {
            if (vertices.size() == 0 || indices.size() == 0) {
                std::cerr << "load vertices failed." << std::endl;
            }
            assert(indices.size() % 3 == 0);
            mesh m(vertices, indices, material);
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
             parent_dir = FS::path(path).parent_path().string();
             traverse_nodes(scene->mRootNode, scene);
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

        mesh load_mesh(aiMesh* mesh, const aiScene* scene)
        {
            std::vector<vertex> vertices;
            std::vector<uint32_t> indices;

            for (uint32_t i = 0; i < mesh->mNumVertices; i++)
            {
                vertex vertex;
                float3 vector; 
            
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.position = vector;
           
                if (mesh->HasNormals())
                {
                    vector.x = mesh->mNormals[i].x;
                    vector.y = mesh->mNormals[i].y;
                    vector.z = mesh->mNormals[i].z;
                    vertex.normal = vector;
                }

                if (mesh->mTextureCoords[0]) 
                {
                    float2 vec;

                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.uv = vec;
                
                    vector.x = mesh->mTangents[i].x;
                    vector.y = mesh->mTangents[i].y;
                    vector.z = mesh->mTangents[i].z;
                    vertex.tangent = vector;
              
                    vector.x = mesh->mBitangents[i].x;
                    vector.y = mesh->mBitangents[i].y;
                    vector.z = mesh->mBitangents[i].z;
                    vertex.bitangent = vector;
                }
                else
                    vertex.uv = float2(0.0f, 0.0f);

                vertices.push_back(vertex);
            }
            for (uint32_t i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (uint32_t j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }

            aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];

            texture* diffuse = load_textures(aiMat, aiTextureType_DIFFUSE, albedo_prop);
            texture* specular = load_textures(aiMat, aiTextureType_SPECULAR, specular_prop);
            texture* normal = load_textures(aiMat, aiTextureType_HEIGHT, normal_prop);
            texture* height = load_textures(aiMat, aiTextureType_AMBIENT, height_prop);
            
            material material;
            material.set_texture(albedo_prop, diffuse);
            material.set_texture(specular_prop, specular);
            material.set_texture(normal_prop, normal);
            material.set_texture(height_prop, height);

            return guarneri::mesh(vertices, indices, material);
        }

        texture* load_textures(aiMaterial* mat, aiTextureType type, std::string property_name)
        {
            texture* ret = nullptr;
            for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);
                std::string relative_path = str.C_Str();
                if (texture_cache.count(relative_path) <= 0) {
                    std::string tex_path = parent_dir + "/" + relative_path;
                    std::cerr << "load: " << tex_path << std::endl;
                    auto tex = new texture(tex_path.c_str(), property_name);
                    texture_cache[relative_path] = tex;
                    ret = tex;
                }
                else {
                    auto kv = texture_cache.find(relative_path);
                    ret = kv->second;
                }
                // assume only one texture per texture property
                break;
            }
            return ret;
        }
	};
}
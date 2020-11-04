#pragma once
#include <guarneri.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace guarneri {
	class model : public object {
    public:
        model(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<material> material) {
            if (vertices.size() == 0 || indices.size() == 0) {
                std::cerr << "load vertices failed." << std::endl;
            }
            assert(indices.size() % 3 == 0);
            auto m = std::make_unique<mesh>(vertices, indices, std::move(material));
            meshes.push_back(std::move(m));
        }

        model(std::string path) {
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

        ~model() {}

    public:
        std::vector<std::unique_ptr<mesh>> meshes;
        transform transform;

    private:
        std::string parent_dir;

    public:
        static std::unique_ptr<model> create(const std::vector<vertex>& vertices, const std::vector<uint32_t>& indices, std::unique_ptr<material> material) {
            return std::make_unique<model>(vertices, indices, std::move(material));
        }

        static std::unique_ptr<model> create(std::string path) {
            return std::make_unique<model>(path);
        }

    private:
        void traverse_nodes(aiNode* node, const aiScene* scene)
        {
            for (uint32_t i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(std::move(load_mesh(mesh, scene)));
            }
            for (uint32_t i = 0; i < node->mNumChildren; i++)
            {
                traverse_nodes(node->mChildren[i], scene);
            }
        }

        std::unique_ptr<mesh> load_mesh(aiMesh* ai_mesh, const aiScene* scene)
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
            
            auto mat = std::make_unique<material>();
            mat->set_texture(albedo_prop, diffuse);
            mat->set_texture(specular_prop, specular);
            mat->set_texture(normal_prop, normal);
            mat->set_texture(height_prop, height);

            return std::make_unique<mesh>(vertices, indices, std::move(mat));
        }

        std::shared_ptr<texture> load_textures(aiMaterial* mat, aiTextureType type, std::string property_name)
        {
            for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);
                std::string relative_path = str.C_Str();
                std::string tex_path = parent_dir + "/" + relative_path;
                return texture::create(tex_path);
            }
            return nullptr;
        }

        public:
            std::string str() const {
                std::stringstream ss;
                ss << "Model[" << this->id << " path: " << parent_dir << " mesh count: " << meshes.size() << "]";
                return ss.str();
            }
	};
}
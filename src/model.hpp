#pragma once
#include <common.hpp>
#include <mesh.hpp>
#include <texture.hpp>
#include <vertex.hpp>
#include <float3.hpp>
#include <unordered_set>
#include <ply.hpp>

namespace guarneri {
	class model {
	public:
		model() {
			root_dir = "";
		}

	public:
		std::string root_dir;

	public:
		void load_from_file(std::string path) {
            int i, j, k;
            PlyFile* ply;
            int nelems;
            char** elist;
            int file_type;
            float version;
            int nprops;
            int num_elems;
            PlyProperty** plist;
            PlyVertex** vlist;
            PlyFace** flist;
            char* elem_name;
            int num_comments;
            char** comments;
            int num_obj_info;
            char** obj_info;

            ply = ply_open_for_reading(path.c_str(), &nelems, &elist, &file_type, &version);

            printf("ply version %f\n", version);
            printf("ply type %d\n", file_type);


            for (i = 0; i < nelems; i++) {

                elem_name = elist[i];
                plist = ply_get_element_description(ply, elem_name, &num_elems, &nprops);

                printf("element %s %d\n", elem_name, num_elems);

                if (equal_strings("vertex", elem_name)) {

                    int size = sizeof(PlyVertex*) * num_elems;
                    vlist = (PlyVertex**)malloc(size);

                    ply_get_property(ply, elem_name, &vert_props[0]);
                    ply_get_property(ply, elem_name, &vert_props[1]);
                    ply_get_property(ply, elem_name, &vert_props[2]);

                    for (j = 0; j < num_elems; j++) {

                        vlist[j] = (PlyVertex*)malloc(sizeof(PlyVertex));
                        ply_get_element(ply, (void*)vlist[j]);

                        //printf("vertex: %g %g %g\n", vlist[j]->x, vlist[j]->y, vlist[j]->z);
                    }
                }

                if (equal_strings("face", elem_name)) {

                    flist = (PlyFace**)malloc(sizeof(PlyFace*) * num_elems);

                    ply_get_property(ply, elem_name, &face_props[0]);
                    ply_get_property(ply, elem_name, &face_props[1]);

                    for (j = 0; j < num_elems; j++) {

                        flist[j] = (PlyFace*)malloc(sizeof(PlyFace));
                        ply_get_element(ply, (void*)flist[j]);

                        printf("face: %d, list = ", flist[j]->intensity);
                        for (k = 0; k < flist[j]->nverts; k++)
                            printf("%d ", flist[j]->verts[k]);
                        printf("\n");
                    }
                }

                for (j = 0; j < nprops; j++)
                    printf("property %s\n", plist[j]->name);
            }

            comments = ply_get_comments(ply, &num_comments);
            for (i = 0; i < num_comments; i++)
                printf("comment = '%s'\n", comments[i]);

            obj_info = ply_get_obj_info(ply, &num_obj_info);
            for (i = 0; i < num_obj_info; i++)
                printf("obj_info = '%s'\n", obj_info[i]);

            ply_close(ply);
		}
	};
}


//#pragma once
//#include <common.hpp>
//#include <mesh.hpp>
//#include <texture.hpp>
//#include <vertex.hpp>
//#include <float3.hpp>
//#include <unordered_set>
//
//
//namespace guarneri {
//	class model {
//	public:
//		model() {
//            root_dir = "";
//		}
//
//        ~model() {
//            for (auto iter = textures.begin(); iter != textures.end(); iter++) {
//                delete &(*iter);
//            }
//        }
//
//    public:
//        std::vector<mesh> meshes;
//        std::vector<texture> textures;
//
//    private:
//        std::string root_dir;
//
//    public:
//         void load_from_file(std::string path) {
//             Assimp::Importer importer;
//             const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
//             if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//             {
//                 std::cerr << importer.GetErrorString() << std::endl;
//                 return;
//             }
//             root_dir = FS::path(path).root_directory().string();
//             traverse_nodes(scene->mRootNode, scene);
//        }
//
//        void traverse_nodes(aiNode* node, const aiScene* scene)
//        {
//            for (uint32_t i = 0; i < node->mNumMeshes; i++)
//            {
//                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//                meshes.push_back(load_mesh(mesh, scene));
//            }
//            for (uint32_t i = 0; i < node->mNumChildren; i++)
//            {
//                traverse_nodes(node->mChildren[i], scene);
//            }
//        }
//
//        mesh load_mesh(aiMesh* mesh, const aiScene* scene)
//        {
//            std::vector<vertex> vertices;
//            std::vector<uint32_t> indices;
//            std::vector<texture> textures;
//
//            for (uint32_t i = 0; i < mesh->mNumVertices; i++)
//            {
//                vertex vertex;
//                float3 vector; 
//            
//                vector.x = mesh->mVertices[i].x;
//                vector.y = mesh->mVertices[i].y;
//                vector.z = mesh->mVertices[i].z;
//                vertex.position = vector;
//           
//                if (mesh->HasNormals())
//                {
//                    vector.x = mesh->mNormals[i].x;
//                    vector.y = mesh->mNormals[i].y;
//                    vector.z = mesh->mNormals[i].z;
//                    vertex.normal = vector;
//                }
//
//                if (mesh->mTextureCoords[0]) 
//                {
//                    float2 vec;
//
//                    vec.x = mesh->mTextureCoords[0][i].x;
//                    vec.y = mesh->mTextureCoords[0][i].y;
//                    vertex.uv = vec;
//                
//                    vector.x = mesh->mTangents[i].x;
//                    vector.y = mesh->mTangents[i].y;
//                    vector.z = mesh->mTangents[i].z;
//                    vertex.tangent = vector;
//              
//                    vector.x = mesh->mBitangents[i].x;
//                    vector.y = mesh->mBitangents[i].y;
//                    vector.z = mesh->mBitangents[i].z;
//                    vertex.bitangent = vector;
//                }
//                else
//                    vertex.uv = float2(0.0f, 0.0f);
//
//                vertices.push_back(vertex);
//            }
//            for (uint32_t i = 0; i < mesh->mNumFaces; i++)
//            {
//                aiFace face = mesh->mFaces[i];
//                for (uint32_t j = 0; j < face.mNumIndices; j++)
//                    indices.push_back(face.mIndices[j]);
//            }
//            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
//        
//            std::vector<texture> albedo = load_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
//            textures.insert(textures.end(), albedo.begin(), albedo.end());
//
//            std::vector<texture> specular = load_textures(material, aiTextureType_SPECULAR, "texture_specular");
//            textures.insert(textures.end(), specular.begin(), specular.end());
//
//            std::vector<texture> normal = load_textures(material, aiTextureType_HEIGHT, "texture_normal");
//            textures.insert(textures.end(), normal.begin(), normal.end());
//     
//            std::vector<texture> height = load_textures(material, aiTextureType_AMBIENT, "texture_height");
//            textures.insert(textures.end(), height.begin(), height.end());
//
//            return guarneri::mesh(vertices, indices);
//        }
//
//        std::vector<texture> load_textures(aiMaterial* mat, aiTextureType type, std::string property_name)
//        {
//            std::vector<texture> ret;
//            std::unordered_set<std::string> texture_cache;
//            for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
//            {
//                aiString str;
//                mat->GetTexture(type, i, &str);
//                if (texture_cache.count(str.C_Str()) <= 0) {
//                    texture_cache.insert(str.C_Str());
//                    std::string tex_path = root_dir + str.C_Str();
//                    ret.push_back(texture(tex_path.c_str(), property_name));
//                }
//            }
//            return ret;
//        }
//	};
//}
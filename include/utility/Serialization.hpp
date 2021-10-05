#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "tinymath.h"
#include "Transform.hpp"
#include "Camera.hpp"
#include "CubeMap.hpp"
#include "ShaderPropertyMap.hpp"
#include "Material.hpp"
#include "ShaderLab.hpp"
#include "Model.hpp"
#include "Scene.hpp"
#include "Utility.hpp"
#include "Logger.hpp"
#include "RasterAttributes.hpp"

#undef GetObject

namespace CpuRasterizor
{
	class Serializer
	{
	public:
		//====================================================================================
		// TMath::vec2f
		static rapidjson::Value serialize(rapidjson::Document& doc, const tinymath::vec2f& vec)
		{
			rapidjson::Value v;
			v.SetObject();
			v.AddMember("x", vec.x, doc.GetAllocator());
			v.AddMember("y", vec.y, doc.GetAllocator());
			return v;
		}

		static void deserialize(const rapidjson::Value& v, tinymath::vec2f& vec)
		{
			vec.x = v["x"].GetFloat();
			vec.y = v["y"].GetFloat();
		}
		// TMath::vec2f
		//====================================================================================


		//====================================================================================
		// TMath::vec3f
		static rapidjson::Value serialize(rapidjson::Document& doc, const tinymath::vec3f& vec)
		{
			rapidjson::Value v;
			v.SetObject();
			v.AddMember("x", vec.x, doc.GetAllocator());
			v.AddMember("y", vec.y, doc.GetAllocator());
			v.AddMember("z", vec.z, doc.GetAllocator());
			return v;
		}

		static void deserialize(const rapidjson::Value& v, tinymath::vec3f& vec)
		{
			vec.x = v["x"].GetFloat();
			vec.y = v["y"].GetFloat();
			vec.z = v["z"].GetFloat();
		}
		// TMath::vec3f
		//====================================================================================


		//====================================================================================
		// TMath::vec4f
		static rapidjson::Value serialize(rapidjson::Document& doc, const tinymath::vec4f& vec)
		{
			rapidjson::Value v;
			v.SetObject();
			v.AddMember("x", vec.x, doc.GetAllocator());
			v.AddMember("y", vec.y, doc.GetAllocator());
			v.AddMember("z", vec.z, doc.GetAllocator());
			v.AddMember("w", vec.z, doc.GetAllocator());
			return v;
		}

		static void deserialize(const rapidjson::Value& v, tinymath::vec4f& vec)
		{
			vec.x = v["x"].GetFloat();
			vec.y = v["y"].GetFloat();
			vec.z = v["z"].GetFloat();
			vec.w = v["w"].GetFloat();
		}
		// TMath::vec4f
		//====================================================================================


		//====================================================================================
		// TMath::mat3x3
		static rapidjson::Value serialize(rapidjson::Document& doc, const tinymath::mat3x3& mat)
		{
			rapidjson::Value v;
			v.SetObject();
			v.AddMember("m00", mat.m00, doc.GetAllocator());
			v.AddMember("m01", mat.m01, doc.GetAllocator());
			v.AddMember("m02", mat.m02, doc.GetAllocator());
			v.AddMember("m10", mat.m10, doc.GetAllocator());
			v.AddMember("m11", mat.m11, doc.GetAllocator());
			v.AddMember("m12", mat.m12, doc.GetAllocator());
			v.AddMember("m20", mat.m20, doc.GetAllocator());
			v.AddMember("m21", mat.m21, doc.GetAllocator());
			v.AddMember("m22", mat.m22, doc.GetAllocator());
			return v;
		}

		static void deserialize(const rapidjson::Value& v, tinymath::mat3x3& mat)
		{
			mat.m00 = v["m00"].GetFloat();
			mat.m01 = v["m01"].GetFloat();
			mat.m02 = v["m02"].GetFloat();
			mat.m10 = v["m10"].GetFloat();
			mat.m11 = v["m11"].GetFloat();
			mat.m12 = v["m12"].GetFloat();
			mat.m20 = v["m20"].GetFloat();
			mat.m21 = v["m21"].GetFloat();
			mat.m22 = v["m22"].GetFloat();
		}		
		// TMath::mat3x3
		//====================================================================================


		//====================================================================================
		// TMath::mat4x4
		static rapidjson::Value serialize(rapidjson::Document& doc, const tinymath::mat4x4& mat)
		{
			rapidjson::Value v;
			v.SetObject();
			v.AddMember("m00", mat.m00, doc.GetAllocator());
			v.AddMember("m01", mat.m01, doc.GetAllocator());
			v.AddMember("m02", mat.m02, doc.GetAllocator());
			v.AddMember("m03", mat.m03, doc.GetAllocator());
			v.AddMember("m10", mat.m10, doc.GetAllocator());
			v.AddMember("m11", mat.m11, doc.GetAllocator());
			v.AddMember("m12", mat.m12, doc.GetAllocator());
			v.AddMember("m13", mat.m13, doc.GetAllocator());
			v.AddMember("m20", mat.m20, doc.GetAllocator());
			v.AddMember("m21", mat.m21, doc.GetAllocator());
			v.AddMember("m22", mat.m22, doc.GetAllocator());
			v.AddMember("m23", mat.m23, doc.GetAllocator());
			v.AddMember("m30", mat.m30, doc.GetAllocator());
			v.AddMember("m31", mat.m31, doc.GetAllocator());
			v.AddMember("m32", mat.m32, doc.GetAllocator());
			v.AddMember("m33", mat.m33, doc.GetAllocator());
			return v;
		}

		static void deserialize(const rapidjson::Value& v, tinymath::mat4x4& mat)
		{
			mat.m00 = v["m00"].GetFloat();
			mat.m01 = v["m01"].GetFloat();
			mat.m02 = v["m02"].GetFloat();
			mat.m03 = v["m03"].GetFloat();
			mat.m10 = v["m10"].GetFloat();
			mat.m11 = v["m11"].GetFloat();
			mat.m12 = v["m12"].GetFloat();
			mat.m13 = v["m13"].GetFloat();
			mat.m20 = v["m20"].GetFloat();
			mat.m21 = v["m21"].GetFloat();
			mat.m22 = v["m22"].GetFloat();
			mat.m23 = v["m23"].GetFloat();
			mat.m30 = v["m30"].GetFloat();
			mat.m31 = v["m31"].GetFloat();
			mat.m32 = v["m32"].GetFloat();
			mat.m33 = v["m33"].GetFloat();
		}
		// TMath::mat4x4
		//====================================================================================


		//====================================================================================
		// Transform
		static rapidjson::Value serialize(rapidjson::Document& doc, const Transform& t)
		{
			rapidjson::Value serialized_transform;
			serialized_transform.SetObject();
			rapidjson::Value name;
			name.SetString(t.name.c_str(), doc.GetAllocator());
			rapidjson::Value local_position = Serializer::serialize(doc, t.local_position);
			rapidjson::Value local_rotation = Serializer::serialize(doc, t.local_rotation);
			rapidjson::Value local_scale = Serializer::serialize(doc, t.local_scale);
			rapidjson::Value world_trs = Serializer::serialize(doc, t.world_trs);
			rapidjson::Value rotation_axis = Serializer::serialize(doc, t.rotation_axis);
			rapidjson::Value rotation_angle; rotation_angle.SetFloat(t.rotation_angle);
			serialized_transform.AddMember("name", name, doc.GetAllocator());
			serialized_transform.AddMember("local_position", local_position, doc.GetAllocator());
			serialized_transform.AddMember("local_rotation", local_rotation, doc.GetAllocator());
			serialized_transform.AddMember("local_scale", local_scale, doc.GetAllocator());
			serialized_transform.AddMember("world_trs", world_trs, doc.GetAllocator());
			serialized_transform.AddMember("rotation_axis", rotation_axis, doc.GetAllocator());
			serialized_transform.AddMember("rotation_angle", rotation_angle, doc.GetAllocator());

			rapidjson::Value serialized_children;
			serialized_children.SetArray();
			if (t.child_count() > 0)
			{
				for(int child_idx = 0; child_idx < t.child_count(); ++child_idx)
				{
					Transform* child = t.access_child(child_idx);
					if (child == nullptr) continue;
					rapidjson::Value cv = serialize(doc, *child);
					serialized_children.PushBack(cv, doc.GetAllocator());
				}
			}
			serialized_transform.AddMember("children", serialized_children, doc.GetAllocator());
			return serialized_transform;
		}

		static void deserialize(const rapidjson::Value& v, Transform& t)
		{
			t.name = v["name"].GetString();
			Serializer::deserialize(v["local_position"].GetObject(), t.local_position);
			Serializer::deserialize(v["local_rotation"].GetObject(), t.local_rotation);
			Serializer::deserialize(v["local_scale"].GetObject(), t.local_scale);
			Serializer::deserialize(v["world_trs"].GetObject(), t.world_trs);
			Serializer::deserialize(v["rotation_axis"].GetObject(), t.rotation_axis);
			t.rotation_angle = v["rotation_angle"].GetFloat();
			const rapidjson::Value& children_objs = v["children"].GetArray();
			for (rapidjson::SizeType idx = 0; idx < children_objs.Size(); idx++)
			{
				const rapidjson::Value& child_obj = children_objs[idx];
				Transform* child = new Transform();
				deserialize(child_obj, *child);
				child->set_parent(&t);
			}
		}
		// Transform
		//====================================================================================


		//====================================================================================
		// Camera
		static rapidjson::Value serialize(rapidjson::Document& doc, const Camera& cam)
		{
			rapidjson::Value v;
			v.SetObject();
			v.AddMember("fov", cam.fov, doc.GetAllocator());
			v.AddMember("aspect", cam.aspect, doc.GetAllocator());
			v.AddMember("near", cam.near, doc.GetAllocator());
			v.AddMember("far", cam.far, doc.GetAllocator());
			v.AddMember("enable_msaa", cam.enable_msaa, doc.GetAllocator());
			v.AddMember("transform", Serializer::serialize(doc, *cam.transform), doc.GetAllocator());
			v.AddMember("projection", (int32_t)cam.projection, doc.GetAllocator());
			v.AddMember("proj_matrix", Serializer::serialize(doc, cam.proj_matrix), doc.GetAllocator()); // ??????????
			return v;
		}

		static void deserialize(const rapidjson::Value& v, Camera& cam)
		{
			cam.fov = v["fov"].GetFloat();
			cam.aspect = v["aspect"].GetFloat();
			cam.near = v["near"].GetFloat();
			cam.far = v["far"].GetFloat();
			cam.enable_msaa = v["enable_msaa"].GetBool();
			Serializer::deserialize(v["proj_matrix"].GetObject(), cam.proj_matrix);
			Transform* deserialized_transform = new Transform();
			Serializer::deserialize(v["transform"].GetObject(), *deserialized_transform);
			cam.transform = std::unique_ptr<Transform>(deserialized_transform);
			cam.projection = (Projection)v["projection"].GetInt();
		}
		// Camera
		//====================================================================================


		//====================================================================================
		// Texture
		static void serialize(const Texture& tex, const std::string& path)
		{
			rapidjson::Document doc;
			doc.SetObject();

			rapidjson::Value raw_path;
			raw_path.SetString(tex.raw_path.c_str(), doc.GetAllocator());
			rapidjson::Value meta_path;
			meta_path.SetString(path.c_str(), doc.GetAllocator());
			doc.AddMember("raw_path", raw_path, doc.GetAllocator());
			doc.AddMember("meta_path", meta_path, doc.GetAllocator());
			doc.AddMember("filtering", (int32_t)tex.filtering, doc.GetAllocator());
			doc.AddMember("wrap_mode", (int32_t)tex.wrap_mode, doc.GetAllocator());
			doc.AddMember("format", (int32_t)tex.format, doc.GetAllocator());
			doc.AddMember("width", (uint32_t)tex.width, doc.GetAllocator());
			doc.AddMember("height", (uint32_t)tex.height, doc.GetAllocator());
			doc.AddMember("mip_count", (uint32_t)tex.mip_count, doc.GetAllocator());
			doc.AddMember("mip_filtering", (int32_t)tex.mip_filtering, doc.GetAllocator());
			doc.AddMember("enable_mip", (int32_t)tex.enable_mip, doc.GetAllocator());

			std::filesystem::path abs_path(ASSETS_PATH + path);
			if (!std::filesystem::exists(abs_path.parent_path()))
			{
				std::filesystem::create_directories(abs_path.parent_path());
			}
			std::FILE* fd = fopen(abs_path.string().c_str(), "w+");
			if (fd != nullptr)
			{
				char write_buffer[256];
				rapidjson::FileWriteStream fs(fd, write_buffer, sizeof(write_buffer));
				rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(fs);
				doc.Accept(writer);
				fclose(fd);

				LOG("save texture: {}", path.c_str());
			}
			else
			{
				ERROR("path does not exist: {}", abs_path.string().c_str());
			}
		}

		static void deserialize(std::string path, Texture& tex)
		{
			std::string abs_path = ASSETS_PATH + path;
			std::FILE* fd = fopen(abs_path.c_str(), "r");
			if (fd != nullptr)
			{
				LOG("deserialize: {}", abs_path.c_str());
				char read_buffer[256];
				rapidjson::FileReadStream fs(fd, read_buffer, sizeof(read_buffer));
				rapidjson::Document doc;
				doc.ParseStream(fs);
				fclose(fd);

				const char* raw_path = doc["raw_path"].GetString();
				tex.raw_path = raw_path;
				tex.meta_path = doc["meta_path"].GetString();
				tex.filtering = (Filtering)doc["filtering"].GetInt();
				tex.wrap_mode = (WrapMode)doc["wrap_mode"].GetInt();
				tex.format = (TextureFormat)doc["format"].GetInt();
				tex.width = doc["width"].GetUint();
				tex.height = doc["height"].GetUint();
				tex.mip_count = doc["mip_count"].GetUint();
				tex.mip_filtering = (Filtering)doc["mip_filtering"].GetInt();
				//tex.enable_mip = doc["enable_mip"].GetBool();
				tex.enable_mip = true; // force enable mip, todo: serialize it
				tex.reload(tex.raw_path.c_str());
			
				LOG("read textures: {}", path.c_str());
			}
			else
			{
				ERROR("path does not exist: {}", abs_path.c_str());
			}
		}
		// Texture
		//====================================================================================


		//====================================================================================
		// CubeMap
		static void serialize(const CubeMap& cube_map, std::string path)
		{
			rapidjson::Document doc;
			doc.SetObject();

			rapidjson::Value name;
			name.SetString(cube_map.name.c_str(), doc.GetAllocator());
			doc.AddMember("name", name, doc.GetAllocator());

			rapidjson::Value cube_map_path;
			cube_map_path.SetString(path.c_str(), doc.GetAllocator());
			doc.AddMember("meta_path", cube_map_path, doc.GetAllocator());

			rapidjson::Value right;
			right.SetString(cube_map.texture_path.c_str(), doc.GetAllocator());
			doc.AddMember("hdr_texture", right, doc.GetAllocator());

			doc.AddMember("wrap_mode", (int32_t)cube_map.wrap_mode, doc.GetAllocator());

			doc.AddMember("filtering", (int32_t)cube_map.filtering, doc.GetAllocator());

			std::filesystem::path abs_path(ASSETS_PATH + path);
			if (!std::filesystem::exists(abs_path.parent_path()))
			{
				std::filesystem::create_directories(abs_path.parent_path());
			}
			std::FILE* fd = fopen(abs_path.string().c_str(), "w+");
			if (fd != nullptr)
			{
				char write_buffer[256];
				rapidjson::FileWriteStream fs(fd, write_buffer, sizeof(write_buffer));
				rapidjson::PrettyWriter<rapidjson::FileWriteStream> material_writer(fs);
				doc.Accept(material_writer);
				fclose(fd);
				LOG("save cubemap: {}", path.c_str());
			}
			else
			{
				ERROR("path does not exist: {}", (ASSETS_PATH + path).c_str());
			}
		}


		static void deserialize(std::string path, CubeMap& cubemap)
		{
			std::FILE* fd = fopen((ASSETS_PATH + path).c_str(), "r");
			if (fd != nullptr)
			{
				LOG("deserialize: {}", (ASSETS_PATH + path).c_str());
				char read_buffer[256];
				rapidjson::FileReadStream fs(fd, read_buffer, sizeof(read_buffer));
				rapidjson::Document doc;
				doc.ParseStream(fs);
				const char* texture_path = doc["texture_path"].GetString();
				fclose(fd);
				cubemap.name = doc["name"].GetString();
				cubemap.meta_path = doc["meta_path"].GetString();
				cubemap.wrap_mode = (WrapMode)doc["wrap_mode"].GetInt();
				cubemap.filtering = (Filtering)doc["filtering"].GetInt();
				cubemap.reload(texture_path);
			}
			else
			{
				ERROR("path does not exist: {}", ASSETS_PATH + path);
			}
		}
		// CubeMap
		//====================================================================================

		//====================================================================================
		// ShaderPropertyMap
		static void serialize(const ShaderPropertyMap& properties, rapidjson::Document& doc)
		{
			rapidjson::Value name2float;
			name2float.SetArray();
			for (auto& kv : properties.name2float)
			{
				rapidjson::Value pair;
				pair.SetArray();
				pair.PushBack(kv.first, doc.GetAllocator());
				pair.PushBack(kv.second, doc.GetAllocator());
				name2float.PushBack(pair, doc.GetAllocator());
			}
			doc.AddMember("name2float", name2float, doc.GetAllocator());

			rapidjson::Value name2float4;
			name2float4.SetArray();
			for (auto& kv : properties.name2float4)
			{
				rapidjson::Value pair;
				pair.SetArray();
				pair.PushBack(kv.first, doc.GetAllocator());
				pair.PushBack(Serializer::serialize(doc, kv.second), doc.GetAllocator());
				name2float4.PushBack(pair, doc.GetAllocator());
			}
			doc.AddMember("name2float4", name2float4, doc.GetAllocator());

			rapidjson::Value name2int;
			name2int.SetArray();
			for (auto& kv : properties.name2int)
			{
				rapidjson::Value pair;
				pair.SetArray();
				pair.PushBack(kv.first, doc.GetAllocator());
				pair.PushBack(kv.second, doc.GetAllocator());
				name2int.PushBack(pair, doc.GetAllocator());
			}
			doc.AddMember("name2int", name2int, doc.GetAllocator());

			rapidjson::Value name2tex;
			name2tex.SetArray();
			for (auto& kv : properties.name2tex)
			{
				if (kv.second == nullptr || kv.second->meta_path == "") continue;
				rapidjson::Value pair;
				pair.SetArray();
				pair.PushBack(kv.first, doc.GetAllocator());
				rapidjson::Value tex_path;
				tex_path.SetString(kv.second->meta_path.c_str(), doc.GetAllocator());
				pair.PushBack(tex_path, doc.GetAllocator());
				name2tex.PushBack(pair, doc.GetAllocator());
			}
			doc.AddMember("name2tex", name2tex, doc.GetAllocator());
		}

		static void deserialize(rapidjson::Document& doc, ShaderPropertyMap& properties)
		{
			rapidjson::Value name2int, name2float, name2float4, name2tex;
			name2int = doc["name2int"].GetArray();
			name2float = doc["name2float"].GetArray();
			name2float4 = doc["name2float4"].GetArray();
			name2tex = doc["name2tex"].GetArray();

			for (rapidjson::SizeType idx = 0; idx < name2int.Size(); idx++)
			{
				const rapidjson::Value& pair = name2int[idx].GetArray();
				properties.name2int[pair[0].GetUint()] = pair[1].GetInt();
			}

			for (rapidjson::SizeType idx = 0; idx < name2float.Size(); idx++)
			{
				const rapidjson::Value& pair = name2float[idx].GetArray();
				properties.name2float[pair[0].GetUint()] = pair[1].GetFloat();
			}

			for (rapidjson::SizeType idx = 0; idx < name2float4.Size(); idx++)
			{
				const rapidjson::Value& pair = name2float4[idx].GetArray();
				Serializer::deserialize(pair[1].GetObject(), properties.name2float4[pair[0].GetUint()]);
			}

			for (rapidjson::SizeType idx = 0; idx < name2tex.Size(); idx++)
			{
				const rapidjson::Value& pair = name2tex[idx].GetArray();
				const char* tex_path = pair[1].GetString();
				properties.name2tex[pair[0].GetUint()] = Texture::load_asset(tex_path);
			}
		}
		// ShaderPropertyMap
		//====================================================================================


		//====================================================================================
		// Material
		static void serialize(const Material& material, std::string path)
		{
			rapidjson::Document doc;
			doc.SetObject();
			rapidjson::Value mat_name;
			mat_name.SetString(material.material_name.c_str(), doc.GetAllocator());
			rapidjson::Value shader_name;
			shader_name.SetString(material.target_shader->name.c_str(), doc.GetAllocator());
			rapidjson::Value meta_path;
			meta_path.SetString(path.c_str(), doc.GetAllocator());
			doc.AddMember("material_name", mat_name, doc.GetAllocator());
			doc.AddMember("target_shader", shader_name, doc.GetAllocator());
			doc.AddMember("meta_path", meta_path, doc.GetAllocator());
			doc.AddMember("color_mask", (int32_t)material.color_mask, doc.GetAllocator());
			doc.AddMember("blend_op", (int32_t)material.blend_op, doc.GetAllocator());
			doc.AddMember("src_factor", (int32_t)material.src_factor, doc.GetAllocator());
			doc.AddMember("dst_factor", (int32_t)material.dst_factor, doc.GetAllocator());
			doc.AddMember("ztest_func", (int32_t)material.ztest_func, doc.GetAllocator());
			doc.AddMember("zwrite_mode", (int32_t)material.zwrite_mode, doc.GetAllocator());
			doc.AddMember("stencil_fail_op", (int32_t)material.stencil_fail_op, doc.GetAllocator());
			doc.AddMember("stencil_func", (int32_t)material.stencil_func, doc.GetAllocator());
			doc.AddMember("stencil_pass_op", (int32_t)material.stencil_pass_op, doc.GetAllocator());
			doc.AddMember("stencil_read_mask", (int32_t)material.stencil_read_mask, doc.GetAllocator());
			doc.AddMember("stencil_ref_val", (int32_t)material.stencil_ref_val, doc.GetAllocator());
			doc.AddMember("stencil_write_mask", (int32_t)material.stencil_write_mask, doc.GetAllocator());
			doc.AddMember("stencil_zfail_op", (int32_t)material.stencil_zfail_op, doc.GetAllocator());
			doc.AddMember("double_face", material.double_face, doc.GetAllocator());
			doc.AddMember("transparent", material.transparent, doc.GetAllocator());
			doc.AddMember("cast_shadow", material.cast_shadow, doc.GetAllocator());

			serialize(material.local_properties, doc);

			std::filesystem::path abs_path(ASSETS_PATH + path);
			if (!std::filesystem::exists(abs_path.parent_path()))
			{
				std::filesystem::create_directories(abs_path.parent_path());
			}
			std::FILE* fd = fopen(abs_path.string().c_str(), "w+");
			if (fd != nullptr)
			{
				char write_buffer[256];
				rapidjson::FileWriteStream fs(fd, write_buffer, sizeof(write_buffer));
				rapidjson::PrettyWriter<rapidjson::FileWriteStream> material_writer(fs);
				doc.Accept(material_writer);
				fclose(fd);
			}
			else
			{
				ERROR("serialize material failed, path does not exist: {}", abs_path.string().c_str());
			}
		}

		static void deserialize(std::string path, Material& material)
		{
			std::filesystem::path abs_path(ASSETS_PATH + path);
			std::FILE* fd = fopen(abs_path.string().c_str(), "r");
			if (fd != nullptr)
			{
				LOG("deserialize: {}", abs_path.string().c_str());
				char read_buffer[256];
				rapidjson::FileReadStream fs(fd, read_buffer, sizeof(read_buffer));
				rapidjson::Document doc;
				doc.ParseStream(fs);
				material.color_mask = (ColorMask)doc["color_mask"].GetInt();
				material.blend_op = (BlendOp)doc["blend_op"].GetInt();
				material.src_factor = (BlendFactor)doc["src_factor"].GetInt();
				material.dst_factor = (BlendFactor)doc["dst_factor"].GetInt();
				material.ztest_func = (CompareFunc)doc["ztest_func"].GetInt();
				material.zwrite_mode = (ZWrite)doc["zwrite_mode"].GetInt();
				material.stencil_fail_op = (StencilOp)doc["stencil_fail_op"].GetInt();
				material.stencil_func = (CompareFunc)doc["stencil_func"].GetInt();
				material.stencil_pass_op = (StencilOp)doc["stencil_pass_op"].GetInt();
				material.stencil_zfail_op = (StencilOp)doc["stencil_zfail_op"].GetInt();
				material.stencil_read_mask = (uint8_t)doc["stencil_read_mask"].GetInt();
				material.stencil_ref_val = (uint8_t)doc["stencil_ref_val"].GetInt();
				material.stencil_write_mask = (uint8_t)doc["stencil_write_mask"].GetInt();
				material.double_face = doc["double_face"].GetBool();
				material.transparent = doc["transparent"].GetBool();
				material.cast_shadow = doc["cast_shadow"].GetBool();
				material.material_name = doc["material_name"].GetString();
				material.meta_path = doc["meta_path"].GetString();
				material.target_shader = ShaderLab::get_shader(doc["target_shader"].GetString());

				deserialize(doc, material.local_properties);

				fclose(fd);
			}
			else
			{
				ERROR("path does not exist: {}", abs_path.string().c_str());
			}
		}
		// Material
		//====================================================================================


		//====================================================================================
		// Vertex
		static rapidjson::Value serialize(rapidjson::Document& doc, const Vertex& vertex)
		{
			rapidjson::Value v;
			v.SetObject();
			v.AddMember("position", Serializer::serialize(doc, vertex.position), doc.GetAllocator());
			v.AddMember("color", Serializer::serialize(doc, vertex.color), doc.GetAllocator());
			v.AddMember("uv", Serializer::serialize(doc, vertex.uv), doc.GetAllocator());
			v.AddMember("tangent", Serializer::serialize(doc, vertex.tangent), doc.GetAllocator());
			v.AddMember("normal", Serializer::serialize(doc, vertex.normal), doc.GetAllocator());
			return v;
		}

		static void deserialize(const rapidjson::Value& v, Vertex& vertex)
		{
			Serializer::deserialize(v["position"], vertex.position);
			Serializer::deserialize(v["color"], vertex.color);
			Serializer::deserialize(v["uv"], vertex.uv);
			Serializer::deserialize(v["tangent"], vertex.tangent);
			Serializer::deserialize(v["normal"], vertex.normal);
		}
		// Vertex
		//====================================================================================
	

		//====================================================================================
		// Mesh
		static rapidjson::Value serialize(rapidjson::Document& doc, const Mesh& mesh)
		{
			rapidjson::Value v;
			v.SetObject();

			rapidjson::Value indices;
			indices.SetArray();
			for (auto& i : mesh.indices)
			{
				indices.PushBack(i, doc.GetAllocator());
			}

			rapidjson::Value vertices;
			vertices.SetArray();
			for (auto& vert : mesh.vertices)
			{
				vertices.PushBack(Serializer::serialize(doc, vert), doc.GetAllocator());
			}

			v.AddMember("indices", indices, doc.GetAllocator());
			v.AddMember("vertices", vertices, doc.GetAllocator());

			return v;
		}

		static void deserialize(const rapidjson::Value& v, Mesh& mesh)
		{
			auto indices_array = v["indices"].GetArray();
			auto vertices_array = v["vertices"].GetArray();

			mesh.indices.clear();
			mesh.vertices.clear();
			mesh.indices.reserve(indices_array.Size());
			mesh.vertices.reserve(vertices_array.Size());

			for (rapidjson::SizeType idx = 0; idx < indices_array.Size(); idx++)
			{
				mesh.indices.emplace_back(indices_array[idx].GetUint());
			}

			for (rapidjson::SizeType idx = 0; idx < vertices_array.Size(); idx++)
			{
				Vertex vertex;
				Serializer::deserialize(vertices_array[idx].GetObject(), vertex);
				mesh.vertices.emplace_back(vertex);
			}
		}
		// Mesh
		//====================================================================================


		//====================================================================================
		// Model
		static void serialize(const Model& model, std::string path)
		{
			rapidjson::Document doc;
			doc.SetObject();

			rapidjson::Value name;
			name.SetString(model.name.c_str(), doc.GetAllocator());
			doc.AddMember("name", name, doc.GetAllocator());

			rapidjson::Value raw_path;
			raw_path.SetString(model.raw_path.c_str(), doc.GetAllocator());
			doc.AddMember("raw_path", raw_path, doc.GetAllocator());

			if (model.raw_path == "" && model.meshes.size() > 0)
			{
				rapidjson::Value meshes;
				meshes.SetArray();
				for (auto& m : model.meshes)
				{
					meshes.PushBack(Serializer::serialize(doc, m), doc.GetAllocator());
				}
				doc.AddMember("meshes", meshes, doc.GetAllocator());
			}

			rapidjson::Value meta_path;
			meta_path.SetString(path.c_str(), doc.GetAllocator());
			doc.AddMember("meta_path", meta_path, doc.GetAllocator());

			rapidjson::Value material;
			if (model.material != nullptr)
			{
				material.SetString(model.material->meta_path.c_str(), doc.GetAllocator());
			}
			else
			{
				material.SetString("", doc.GetAllocator());
			}
			doc.AddMember("material", material, doc.GetAllocator());

			doc.AddMember("transform", Serializer::serialize(doc, *model.transform), doc.GetAllocator());

			doc.AddMember("flip_uv", model.flip_uv, doc.GetAllocator());

			std::filesystem::path abs_path(ASSETS_PATH + path);
			if (!std::filesystem::exists(abs_path.parent_path()))
			{
				std::filesystem::create_directories(abs_path.parent_path());
			}
			std::FILE* fd = fopen(abs_path.string().c_str(), "w+");
			if (fd != nullptr)
			{
				char write_buffer[256];
				rapidjson::FileWriteStream fs(fd, write_buffer, sizeof(write_buffer));
				rapidjson::PrettyWriter<rapidjson::FileWriteStream> material_writer(fs);
				doc.Accept(material_writer);
				fclose(fd);
				LOG("save model: {}", path);
			}
			else
			{
				ERROR("path does not exist: {}", ASSETS_PATH + path);
			}
		}

		static void deserialize(std::string path, Model& model)
		{
			static std::vector<int> free_names;
			static int current_name;
			std::FILE* fd = fopen((ASSETS_PATH + path).c_str(), "r");
			if (fd != nullptr)
			{
				char read_buffer[256];
				rapidjson::FileReadStream fs(fd, read_buffer, sizeof(read_buffer));
				rapidjson::Document doc;
				doc.ParseStream(fs);
				fclose(fd);

				model.name = doc["name"].GetString();
				if (model.name == "")
				{
					if (free_names.size() > 0)
					{
						int num = free_names.back();
						free_names.pop_back();
						model.name = "Model(" + std::to_string(num) + ")";
					}
					else
					{
						model.name = "Model(" + std::to_string(current_name++) + ")";
					}
				}
				model.raw_path = doc["raw_path"].GetString();
				model.meta_path = doc["meta_path"].GetString();
				model.flip_uv = doc["flip_uv"].GetBool();
				std::string material_path = doc["material"].GetString();

				if (model.raw_path != "")
				{
					model.load_raw(model.raw_path, model.flip_uv);
				}
				else
				{
					rapidjson::Value meshes = doc["meshes"].GetArray();
					for (rapidjson::SizeType idx = 0; idx < meshes.Size(); idx++)
					{
						Mesh mesh;
						Serializer::deserialize(meshes[idx].GetObject(), mesh);
						model.meshes.emplace_back(mesh);
					}
				}

				if (material_path != "")
				{
					Material* mat = new Material();
					Serializer::deserialize(material_path, *mat);
					model.material = std::shared_ptr<Material>(mat);
				}
				else
				{
					model.material = std::make_shared<Material>();
				}

				Transform* deserialized_transform = new Transform();
				Serializer::deserialize(doc["transform"].GetObject(), *deserialized_transform);
				model.transform = std::unique_ptr<Transform>(deserialized_transform);
				model.transform->name = model.name;

				model.transform->set_model(&model);
			}
			else
			{
				ERROR("path does not exist: {}", ASSETS_PATH + path);
			}
		}
		// Model
		//====================================================================================


		//====================================================================================
		// Light
		static rapidjson::Value serialize(rapidjson::Document& doc, const DirectionalLight& light)
		{
			rapidjson::Value serialized_light;
			serialized_light.SetObject();
			rapidjson::Value ambient = Serializer::serialize(doc, light.ambient);
			rapidjson::Value diffuse = Serializer::serialize(doc, light.diffuse);
			rapidjson::Value specular = Serializer::serialize(doc, light.specular);
			rapidjson::Value intensity; intensity.SetFloat(light.intensity);
			rapidjson::Value yaw; yaw.SetFloat(light.yaw);
			rapidjson::Value pitch; pitch.SetFloat(light.pitch);
			rapidjson::Value distance; distance.SetFloat(light.distance);
			serialized_light.AddMember("ambient", ambient, doc.GetAllocator());
			serialized_light.AddMember("diffuse", diffuse, doc.GetAllocator());
			serialized_light.AddMember("specular", specular, doc.GetAllocator());
			serialized_light.AddMember("intensity", intensity, doc.GetAllocator());
			serialized_light.AddMember("yaw", yaw, doc.GetAllocator());
			serialized_light.AddMember("pitch", pitch, doc.GetAllocator());
			serialized_light.AddMember("position", distance, doc.GetAllocator());
			return serialized_light;
		}

		static void deserialize(const rapidjson::Value& v, DirectionalLight& light)
		{
			Serializer::deserialize(v["ambient"].GetObject(), light.ambient);
			Serializer::deserialize(v["diffuse"].GetObject(), light.diffuse);
			Serializer::deserialize(v["specular"].GetObject(), light.specular);
			light.intensity = v["intensity"].GetFloat();
			light.yaw = v["yaw"].GetFloat();
			light.pitch = v["pitch"].GetFloat(); 
			light.distance = v["distance"].GetFloat();
		}

		static rapidjson::Value serialize(rapidjson::Document& doc, const PointLight& light)
		{
			rapidjson::Value serialized_light;
			serialized_light.SetObject();
			rapidjson::Value ambient = Serializer::serialize(doc, light.ambient);
			rapidjson::Value diffuse = Serializer::serialize(doc, light.diffuse);
			rapidjson::Value specular = Serializer::serialize(doc, light.specular);
			rapidjson::Value intensity; intensity.SetFloat(light.intensity);
			rapidjson::Value constant; constant.SetFloat(light.constant);
			rapidjson::Value linear; linear.SetFloat(light.linear);
			rapidjson::Value quadratic; quadratic.SetFloat(light.quadratic);
			rapidjson::Value position = Serializer::serialize(doc, light.position);
			serialized_light.AddMember("ambient", ambient, doc.GetAllocator());
			serialized_light.AddMember("diffuse", diffuse, doc.GetAllocator());
			serialized_light.AddMember("specular", specular, doc.GetAllocator());
			serialized_light.AddMember("intensity", intensity, doc.GetAllocator());
			serialized_light.AddMember("constant", constant, doc.GetAllocator());
			serialized_light.AddMember("linear", linear, doc.GetAllocator());
			serialized_light.AddMember("quadratic", quadratic, doc.GetAllocator());
			serialized_light.AddMember("position", position, doc.GetAllocator());
			return serialized_light;
		}

		static void deserialize(const rapidjson::Value& v, PointLight& light)
		{
			Serializer::deserialize(v["ambient"].GetObject(), light.ambient);
			Serializer::deserialize(v["diffuse"].GetObject(), light.diffuse);
			Serializer::deserialize(v["specular"].GetObject(), light.specular);
			light.intensity = v["intensity"].GetFloat();
			light.constant = v["constant"].GetFloat();
			light.linear = v["linear"].GetFloat();
			light.quadratic = v["quadratic"].GetFloat();
			Serializer::deserialize(v["position"].GetObject(), light.position);
		}
		// Light
		//====================================================================================



		//====================================================================================
		// Scene
		static void serialize(const Scene& scene, const std::string& path)
		{
			rapidjson::Document doc;
			doc.SetObject();

			rapidjson::Value name;
			name.SetString(scene.name.c_str(), doc.GetAllocator());
			doc.AddMember("name", name, doc.GetAllocator());
			doc.AddMember("main_light", Serializer::serialize(doc, scene.main_light), doc.GetAllocator());

			rapidjson::Value point_lights;
			point_lights.SetArray();
			for (auto& pl : scene.point_lights)
			{
				point_lights.PushBack(Serializer::serialize(doc, pl), doc.GetAllocator());
			}
			doc.AddMember("point_lights", point_lights, doc.GetAllocator());

			rapidjson::Value models;
			models.SetArray();
			for (auto& obj : scene.objects)
			{
				rapidjson::Value meta_path;
				meta_path.SetString(obj->target->meta_path.c_str(), doc.GetAllocator());
				models.PushBack(meta_path, doc.GetAllocator());
			}
			for (auto& obj : scene.transparent_objects)
			{
				rapidjson::Value meta_path;
				meta_path.SetString(obj->target->meta_path.c_str(), doc.GetAllocator());
				models.PushBack(meta_path, doc.GetAllocator());
			}
			doc.AddMember("models", models, doc.GetAllocator());
			doc.AddMember("enable_ibl", scene.enable_skybox, doc.GetAllocator());
			doc.AddMember("enable_shadow", scene.enable_shadow, doc.GetAllocator());
			doc.AddMember("pcf_on", scene.pcf_on, doc.GetAllocator());
			doc.AddMember("shadow_bias", scene.shadow_bias, doc.GetAllocator());
			doc.AddMember("color_space", (int32_t)scene.color_space, doc.GetAllocator());
			doc.AddMember("work_flow", (int32_t)scene.work_flow, doc.GetAllocator());

			rapidjson::Value cubemap_path;
			cubemap_path.SetString(scene.cubemap->meta_path.c_str(), doc.GetAllocator());
			doc.AddMember("cubemap_path", cubemap_path, doc.GetAllocator());

			rapidjson::Value main_cam = Serializer::serialize(doc, *scene.main_cam).GetObject();
			doc.AddMember("main_cam", main_cam, doc.GetAllocator());

			rapidjson::StringBuffer sb;
			sb.Clear();
			rapidjson::PrettyWriter<rapidjson::StringBuffer> ww(sb);
			doc.Accept(ww);

			std::filesystem::path abs_path(ASSETS_PATH + path);
			if (!std::filesystem::exists(abs_path.parent_path()))
			{
				std::filesystem::create_directories(abs_path.parent_path());
			}
			std::FILE* fd = fopen(abs_path.string().c_str(), "w+");
			if (fd != nullptr)
			{
				char write_buffer[256];
				rapidjson::FileWriteStream fs(fd, write_buffer, sizeof(write_buffer));
				rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(fs);
				doc.Accept(writer);
				fclose(fd);
				LOG("save scene: {}", path);
			}
			else
			{
				ERROR("path does not exist: {}", ASSETS_PATH + path);
			}
		}

		static void deserialize(const std::string& path, Scene& scene)
		{
			scene.asset_path = path;
			std::filesystem::path abs_path(ASSETS_PATH + path);
			std::FILE* fd = fopen(abs_path.string().c_str(), "r");
			if (fd != nullptr)
			{
				char read_buffer[256];
				rapidjson::FileReadStream fs(fd, read_buffer, sizeof(read_buffer));
				rapidjson::Document doc;
				doc.ParseStream(fs);

				const char* name = doc["name"].GetString();
				scene.name = name;
				if (scene.name == "")
				{
					size_t last_slash = path.find_last_of("/\\");
					std::string nice_name = path;
					nice_name = nice_name.replace(nice_name.begin(), nice_name.begin() + last_slash + 1, "");
					scene.name = nice_name;
				}
				Serializer::deserialize(doc["main_light"].GetObject(), scene.main_light);
				auto point_lights = doc["point_lights"].GetArray();
				for (rapidjson::SizeType idx = 0; idx < point_lights.Size(); idx++)
				{
					PointLight pl;
					Serializer::deserialize(point_lights[idx].GetObject(), pl);
					scene.point_lights.push_back(pl);
				}

				auto models = doc["models"].GetArray();
				for (rapidjson::SizeType idx = 0; idx < models.Size(); idx++)
				{
					const char* model_path = models[idx].GetString();
					Model* model = new Model();
					Serializer::deserialize(model_path, *model);
					scene.add(std::shared_ptr<Model>(model));
				}

				const char* cubemap_path = doc["cubemap_path"].GetString();
				if (cubemap_path != nullptr)
				{
					scene.cubemap = std::shared_ptr<CubeMap>(new CubeMap(cubemap_path));
				}

				scene.enable_skybox = doc["enable_ibl"].GetBool();
				scene.enable_shadow = doc["enable_shadow"].GetBool();
				scene.pcf_on = doc["pcf_on"].GetBool();
				scene.shadow_bias = doc["shadow_bias"].GetFloat();
				scene.color_space = (ColorSpace)doc["color_space"].GetInt();
				scene.work_flow = (PBRWorkFlow)doc["work_flow"].GetInt();

				Camera* camera = new Camera();
				Serializer::deserialize(doc["main_cam"].GetObject(), *camera);
				scene.main_cam = std::unique_ptr<Camera>(camera);
				Camera::set_main_camera(scene.main_cam.get());

				fclose(fd);
			}
			else
			{
				ERROR("path does not exist: {}", ASSETS_PATH + path);
			}
		}
		// Scene
		//====================================================================================
	};
}
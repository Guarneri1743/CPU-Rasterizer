#include "Material.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "Marcos.h"
#include "ShaderLab.hpp"
#include "Singleton.hpp"
#include "Utility.hpp"

namespace Guarneri
{
	Material::Material()
	{
		this->material_name = "default_material";
		this->target_shader = std::make_shared<Shader>("pbr_shader");
		this->shadow_caster = std::dynamic_pointer_cast<Shader>(std::make_shared<ShadowShader>());
		this->color_mask = (ColorMask::R | ColorMask::G | ColorMask::B | ColorMask::A);
		this->stencil_func = CompareFunc::ALWAYS;
		this->stencil_pass_op = StencilOp::KEEP;
		this->stencil_fail_op = StencilOp::KEEP;
		this->stencil_zfail_op = StencilOp::KEEP;
		this->stencil_read_mask = 0xFF;
		this->stencil_write_mask = 0xFF;
		this->stencil_ref_val = 0;
		this->ztest_func = CompareFunc::LESS;
		this->zwrite_mode = ZWrite::ON;
		this->src_factor = BlendFactor::SRC_ALPHA;
		this->dst_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;
		this->blend_op = BlendOp::ADD;
		this->double_face = false;
		this->transparent = false;
		this->cast_shadow = false;
	}

	Material::Material(std::string name) : Material()
	{
		this->material_name = name;
	}

	Material::Material(std::string name, std::shared_ptr<Shader> shader) : Material(name)
	{
		this->target_shader = shader;
	}

	Material::Material(const Material& other)
	{
		copy(other);
	}

	Material::~Material()
	{}

	std::shared_ptr<Material> Material::create()
	{
		std::shared_ptr<Material> mat = std::shared_ptr<Material>(new Material());
		return mat;
	}

	std::shared_ptr<Material> Material::create(std::string path)
	{
		std::shared_ptr<Material> mat = std::shared_ptr<Material>(new Material());
		Material::deserialize(path, *mat);
		return mat;
	}

	std::shared_ptr<Material> Material::create(std::string name, std::shared_ptr<Shader> shader)
	{
		return std::shared_ptr<Material>(new Material(name, shader));
	}

	std::shared_ptr<Material> Material::create(const Material& other)
	{
		return std::shared_ptr<Material>(new Material(other));
	}

	Shader* Material::get_shader(const RenderPass& pass) const
	{
		if (pass == RenderPass::SHADOW)
		{
			return shadow_caster.get();
		}
		return target_shader.get();
	}

	void Material::set_shadowmap(RawBuffer<float>* shadowmap)
	{
		this->target_shader->shadowmap = shadowmap;
	}

	void Material::sync(Shader* shader, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p)
	{
		shader->model = m;
		shader->view = v;
		shader->projection = p;
		shader->ztest_func = ztest_func;
		shader->zwrite_mode = zwrite_mode;
		shader->src_factor = src_factor;
		shader->dst_factor = dst_factor;
		shader->blend_op = blend_op;
		shader->transparent = transparent;
		shader->stencil_func = stencil_func;
		shader->stencil_pass_op = stencil_pass_op;
		shader->stencil_fail_op = stencil_fail_op;
		shader->stencil_zfail_op = stencil_zfail_op;
		shader->stencil_read_mask = stencil_read_mask;
		shader->stencil_write_mask = stencil_write_mask;
		shader->stencil_ref_val = stencil_ref_val;
		shader->color_mask = color_mask;
		shader->lighting_param = lighting_param;
		shader->double_face = double_face;
		shader->name2float = name2float;
		shader->name2float4 = name2float4;
		shader->name2tex = name2tex;
		shader->name2int = name2int;
		shader->name2cubemap = name2cubemap;
		if (name2tex.count(normal_prop) > 0)
		{
			shader->normal_map = true;
		}
		else
		{
			shader->normal_map = false;
		}
	}

	void Material::sync(const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p)
	{
		sync(target_shader.get(), m, v, p);
		if (shadow_caster != nullptr)
		{
			sync(shadow_caster.get(), m, v, p);
		}
	}

	void Material::set_int(const property_name& name, const int& val)
	{
		name2int[name] = val;
	}

	void Material::set_float4(const property_name& name, const Vector4& val)
	{
		name2float4[name] = val;
	}

	void Material::set_float(const property_name& name, const float& val)
	{
		name2float[name] = val;
	}

	void Material::set_texture(const property_name& name, std::shared_ptr<Texture> tex)
	{
		if (tex == nullptr)
		{
			return;
		}
		name2tex[name] = tex;
	}

	void Material::set_cubemap(const property_name& name, std::shared_ptr<CubeMap> cubemap)
	{
		if (cubemap == nullptr)
		{
			return;
		}
		name2cubemap[name] = cubemap;
	}

	int Material::get_int(const property_name& name) const
	{
		if (name2int.count(name) > 0)
		{
			return name2int.at(name);
		}
		return 0;
	}

	Vector4 Material::get_float4(const property_name& name) const
	{
		if (name2float4.count(name) > 0)
		{
			return name2float4.at(name);
		}
		return 0;
	}

	float Material::get_float(const property_name& name) const
	{
		if (name2float.count(name) > 0)
		{
			return name2float.at(name);
		}
		return 0;
	}

	std::shared_ptr<Texture> Material::get_texture(const property_name& name) const
	{
		if (name2tex.count(name) > 0)
		{
			return name2tex.at(name);
		}
		return nullptr;
	}

	std::shared_ptr<CubeMap> Material::get_cubemap(const property_name& name) const
	{
		if (name2cubemap.count(name) > 0)
		{
			return name2cubemap.at(name);
		}
		return nullptr;
	}

	Material& Material::operator =(const Material& other)
	{
		copy(other);
		return *this;
	}

	void Material::copy(const Material& other)
	{
		this->material_name = other.material_name;
		this->target_shader = other.target_shader;
		this->ztest_func = other.ztest_func;
		this->zwrite_mode = other.zwrite_mode;
		this->stencil_func = other.stencil_func;
		this->stencil_pass_op = other.stencil_pass_op;
		this->stencil_fail_op = other.stencil_fail_op;
		this->stencil_zfail_op = other.stencil_zfail_op;
		this->stencil_read_mask = other.stencil_read_mask;
		this->stencil_write_mask = other.stencil_write_mask;
		this->stencil_ref_val = other.stencil_ref_val;
		this->src_factor = other.src_factor;
		this->dst_factor = other.dst_factor;
		this->blend_op = other.blend_op;
		this->double_face = other.double_face;
		this->transparent = other.transparent;
		this->name2float = other.name2float;
		this->name2float4 = other.name2float4;
		this->name2int = other.name2int;
		this->name2tex = other.name2tex;
		this->name2cubemap = other.name2cubemap;
	}

	std::string Material::str() const
	{
		std::stringstream ss;
		ss << "Material[" << this->id << " Shader: " << this->target_shader << "]";
		return ss.str();
	}

	void Material::serialize(const Material& material, std::string path)
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
		doc.AddMember("double_face", material.double_face, doc.GetAllocator());
		doc.AddMember("transparent", material.transparent, doc.GetAllocator());
		doc.AddMember("cast_shadow", material.cast_shadow, doc.GetAllocator());

		rapidjson::Value name2float;
		name2float.SetArray();
		for (auto& kv : material.name2float)
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
		for (auto& kv : material.name2float4)
		{
			rapidjson::Value pair;
			pair.SetArray();
			pair.PushBack(kv.first, doc.GetAllocator());
			pair.PushBack(Vector4::serialize(doc, kv.second), doc.GetAllocator());
			name2float4.PushBack(pair, doc.GetAllocator());
		}
		doc.AddMember("name2float4", name2float4, doc.GetAllocator());

		rapidjson::Value name2int;
		name2int.SetArray();
		for (auto& kv : material.name2int)
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
		for (auto& kv : material.name2tex)
		{
			if (kv.second->meta_path == "") continue;
			rapidjson::Value pair;
			pair.SetArray();
			pair.PushBack(kv.first, doc.GetAllocator());
			rapidjson::Value tex_path;
			tex_path.SetString(kv.second->meta_path.c_str(), doc.GetAllocator());
			pair.PushBack(tex_path, doc.GetAllocator());
			name2tex.PushBack(pair, doc.GetAllocator());
		}
		doc.AddMember("name2tex", name2tex, doc.GetAllocator());

		rapidjson::Value name2cubemap;
		name2cubemap.SetArray();
		for (auto& kv : material.name2cubemap)
		{
			if (kv.second->meta_path == "") continue;
			rapidjson::Value pair;
			pair.SetArray();
			pair.PushBack(kv.first, doc.GetAllocator());
			rapidjson::Value tex_path;
			tex_path.SetString(kv.second->meta_path.c_str(), doc.GetAllocator());
			pair.PushBack(tex_path, doc.GetAllocator());
			name2cubemap.PushBack(pair, doc.GetAllocator());
		}
		doc.AddMember("name2cubemap", name2cubemap, doc.GetAllocator());

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
			std::cout << "path does not exist: " << ASSETS_PATH + path << std::endl;
		}
	}

	void Material::deserialize(std::string path, Material& material)
	{
		std::FILE* fd = fopen((ASSETS_PATH + path).c_str(), "r");
		if (fd != nullptr)
		{
			std::cout << "deserialize: " << ASSETS_PATH + path << std::endl;
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
			material.stencil_read_mask = (uint8_t)doc["stencil_read_mask"].GetInt();
			material.stencil_ref_val = (uint8_t)doc["stencil_ref_val"].GetInt();
			material.stencil_write_mask = (uint8_t)doc["stencil_write_mask"].GetInt();
			material.double_face = doc["double_face"].GetBool();
			material.transparent = doc["transparent"].GetBool();
			material.cast_shadow = doc["cast_shadow"].GetBool();
			material.material_name = doc["material_name"].GetString();
			material.meta_path = doc["meta_path"].GetString();
			material.target_shader = ShaderLab::get_shader(doc["target_shader"].GetString());

			rapidjson::Value name2int, name2float, name2float4, name2tex, name2cubemap;
			name2int = doc["name2int"].GetArray();
			name2float = doc["name2float"].GetArray();
			name2float4 = doc["name2float4"].GetArray();
			name2tex = doc["name2tex"].GetArray();
			name2cubemap = doc["name2cubemap"].GetArray();

			for (rapidjson::SizeType idx = 0; idx < name2int.Size(); idx++)
			{
				const rapidjson::Value& pair = name2int.PopBack();
				material.name2int[pair[0].GetUint()] = pair[1].GetInt();
			}

			for (rapidjson::SizeType idx = 0; idx < name2float.Size(); idx++)
			{
				const rapidjson::Value& pair = name2float.PopBack();
				material.name2float[pair[0].GetUint()] = pair[1].GetFloat();
			}

			for (rapidjson::SizeType idx = 0; idx < name2float4.Size(); idx++)
			{
				const rapidjson::Value& pair = name2float4.PopBack();
				material.name2float4[pair[0].GetUint()] = Vector4::deserialize(pair[1].GetObject());
			}

			for (rapidjson::SizeType idx = 0; idx < name2tex.Size(); idx++)
			{
				const rapidjson::Value& pair = name2tex[idx].GetArray();
				const char* tex_path = pair[1].GetString();
				material.name2tex[pair.GetArray()[0].GetUint()] = Texture::create(tex_path);
			}

			for (rapidjson::SizeType idx = 0; idx < name2cubemap.Size(); idx++)
			{
				const rapidjson::Value& pair = name2cubemap[idx].GetArray();
				const char* tex_path = pair[1].GetString();
				material.name2cubemap[pair.GetArray()[0].GetUint()] = CubeMap::create(tex_path);
			}

			fclose(fd);
		}
		else
		{
			std::cout << "path does not exist: " << ASSETS_PATH + path << std::endl;
		}
	}
}
#include "Material.hpp"
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
#include "Logger.hpp"

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
		this->cast_shadow = true;
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

	void Material::sync(Shader* shader, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p)
	{
		shader->model = m;
		shader->view = v;
		shader->projection = p;
		shader->vp_matrix = p * v;
		shader->mvp_matrix = p * v * m;
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

	void Material::sync(const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p)
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

	void Material::set_float4(const property_name& name, const tinymath::vec4f& val)
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

	tinymath::vec4f Material::get_float4(const property_name& name) const
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
}
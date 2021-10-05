#include "Material.hpp"
#include "Define.hpp"
#include "ShaderLab.hpp"
#include "Singleton.hpp"
#include "Utility.hpp"
#include "Logger.hpp"

namespace CpuRasterizor
{
	Material::Material()
	{
		this->material_name = "default_material";
		this->target_shader = std::make_shared<Shader>("pbr_shader");
		this->shadow_caster = std::dynamic_pointer_cast<Shader>(std::make_shared<ShadowShader>());
		this->color_mask = (ColorMask::kRed | ColorMask::kGreen | ColorMask::kBlue | ColorMask::kAlpha);
		this->stencil_func = CompareFunc::kAlways;
		this->stencil_pass_op = StencilOp::kKeep;
		this->stencil_fail_op = StencilOp::kKeep;
		this->stencil_zfail_op = StencilOp::kKeep;
		this->stencil_read_mask = 0xFF;
		this->stencil_write_mask = 0xFF;
		this->stencil_ref_val = 0;
		this->ztest_func = CompareFunc::kLess;
		this->zwrite_mode = ZWrite::kOn;
		this->src_factor = BlendFactor::kSrcAlpha;
		this->dst_factor = BlendFactor::kOneMinusSrcAlpha;
		this->blend_op = BlendOp::kAdd;
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
		if (pass == RenderPass::kShadow)
		{
			return shadow_caster.get();
		}
		return target_shader.get();
	}

	void Material::sync(Shader* shader)
	{
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
		shader->double_face = double_face;
		shader->local_properties = local_properties;
	}

	void Material::sync()
	{
		sync(target_shader.get());
		if (shadow_caster != nullptr)
		{
			sync(shadow_caster.get());
		}
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
		this->local_properties = other.local_properties;
	}
}
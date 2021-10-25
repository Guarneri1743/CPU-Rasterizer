#include "Material.hpp"
#include "Define.hpp"
#include "ShaderLab.hpp"
#include "Singleton.hpp"
#include "Utility.hpp"
#include "Logger.hpp"
#include "Shader.hpp"
#include "CGL.h"

namespace CpuRasterizer
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
		this->zwrite_on = true;
		this->src_factor = BlendFactor::kSrcAlpha;
		this->dst_factor = BlendFactor::kOneMinusSrcAlpha;
		this->blend_op = BlendFunc::kAdd;
		this->double_face = false;
		this->transparent = false;
		this->cast_shadow = true;
		initialize();
	}

	Material::Material(std::string name) : Material()
	{
		this->material_name = name;
	}

	Material::Material(std::string name, std::shared_ptr<Shader> shader) : Material(name)
	{
		this->target_shader = shader;
		initialize();
	}

	Material::Material(const Material& other)
	{
		copy(other);
	}

	Material::~Material()
	{}

	void Material::initialize()
	{
		if (target_shader != nullptr)
		{
			cglCreateProgram(target_shader.get(), target_shader_id);
		}

		if (shadow_caster != nullptr)
		{
			cglCreateProgram(shadow_caster.get(), shadow_caster_id);
		}
	}

	uint32_t Material::get_shader(RenderPass pass) const
	{
		if (pass == RenderPass::kShadow)
		{
			return shadow_caster_id;
		}
		return target_shader_id;
	}

	void Material::use(RenderPass pass)
	{
		cglDepthFunc(ztest_func);
		cglSetBlendFactor(src_factor, dst_factor);
		cglSetBlendFunc(blend_op);

		if (zwrite_on)
		{
			cglEnable(PipelineFeature::kZWrite);
		}
		else
		{
			cglDisable(PipelineFeature::kZWrite);
		}

		if (transparent)
		{
			cglEnable(PipelineFeature::kBlending);
		}
		else
		{
			cglDisable(PipelineFeature::kBlending);
		}

		cglSetStencilFunc(stencil_func);
		cglStencilMask(stencil_ref_val, stencil_write_mask, stencil_read_mask);
		cglSetStencilOp(stencil_pass_op, stencil_fail_op, stencil_zfail_op);
		cglSetColorMask(color_mask);
		if (!double_face)
		{
			cglEnable(PipelineFeature::kFaceCulling);
		}
		else
		{
			cglDisable(PipelineFeature::kFaceCulling);
		}

		cglCullFace(double_face ? FaceCulling::None : FaceCulling::Back);

		cglUseProgram(get_shader(pass));

		// todo
		if (pass == RenderPass::kShadow)
		{
			if (shadow_caster != nullptr)
			{
				shadow_caster->local_properties = local_properties;
			}
		}
		else
		{
			if (target_shader != nullptr)
			{
				target_shader->local_properties = local_properties;
			}
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
		this->zwrite_on = other.zwrite_on;
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
		this->target_shader_id = other.target_shader_id;
		this->shadow_caster_id = other.shadow_caster_id;
	}
}
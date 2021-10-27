#include "Material.hpp"
#include "Define.hpp"
#include "ShaderLab.hpp"
#include "Singleton.hpp"
#include "Utility.hpp"
#include "Logger.hpp"
#include "ShaderProgram.hpp"
#include "CGL.h"

namespace CpuRasterizer
{
	Material::Material():
		material_name("default_material"),
		target_shader(std::dynamic_pointer_cast<ShaderProgram>(std::make_shared<PBRShader>())),
		shadow_caster(std::dynamic_pointer_cast<ShaderProgram>(std::make_shared<ShadowShader>())),
		color_mask((ColorMask::kRed | ColorMask::kGreen | ColorMask::kBlue | ColorMask::kAlpha)),
		stencil_func(CompareFunc::kAlways),
		stencil_pass_op(StencilOp::kKeep),
		stencil_fail_op(StencilOp::kKeep),
		stencil_zfail_op(StencilOp::kKeep),
		stencil_read_mask(0xFF),
		stencil_write_mask(0xFF),
		stencil_ref_val(0),
		ztest_func(CompareFunc::kLess),
		zwrite_on(true),
		src_factor(BlendFactor::kSrcAlpha),
		dst_factor(BlendFactor::kOneMinusSrcAlpha),
		blend_op(BlendFunc::kAdd),
		double_face(false),
		transparent(false),
		cast_shadow(true),
		render_queue(0),
		stencil_on(false)
	{
		initialize();
	}

	Material::Material(std::string name) : Material()
	{
		this->material_name = name;
	}

	Material::Material(std::string name, std::shared_ptr<ShaderProgram> shader) : Material(name)
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
			target_shader_id = cglCreateProgram(target_shader.get());
		}

		if (shadow_caster != nullptr)
		{
			shadow_caster_id = cglCreateProgram(shadow_caster.get());
		}
	}

	resource_id Material::get_shader(RenderPass pass) const
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

		if (stencil_on)
		{
			cglEnable(PipelineFeature::kStencilTest);
		}
		else
		{
			cglDisable(PipelineFeature::kStencilTest);
		}

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
		this->render_queue = other.render_queue;
		this->stencil_on = other.stencil_on;
	}
}
#include "Material.hpp"
#include "Define.hpp"
#include "ShaderLab.hpp"
#include "Singleton.hpp"
#include "Utility.hpp"
#include "Logger.hpp"
#include "Shader.hpp"

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

	Shader* Material::get_shader(RenderPass pass) const
	{
		if (pass == RenderPass::kShadow)
		{
			return shadow_caster.get();
		}
		return target_shader.get();
	}

	void Material::sync(Shader* shader)
	{
		CpuRasterSharedData.ztest_func = ztest_func;
		CpuRasterSharedData.src_factor = src_factor;
		CpuRasterSharedData.dst_factor = dst_factor;
		CpuRasterSharedData.blend_op = blend_op;
		if (zwrite_on)
		{
			CpuRasterSharedData.raster_flag |= RasterFlag::kZWrite;
		}
		else
		{
			CpuRasterSharedData.raster_flag &= ~RasterFlag::kZWrite;
		}

		if (transparent)
		{
			CpuRasterSharedData.raster_flag |= RasterFlag::kBlending;
		}
		else
		{
			CpuRasterSharedData.raster_flag &= ~RasterFlag::kBlending;
		}
		
		CpuRasterSharedData.stencil_func = stencil_func;
		CpuRasterSharedData.stencil_pass_op = stencil_pass_op;
		CpuRasterSharedData.stencil_fail_op = stencil_fail_op;
		CpuRasterSharedData.stencil_zfail_op = stencil_zfail_op;
		CpuRasterSharedData.stencil_read_mask = stencil_read_mask;
		CpuRasterSharedData.stencil_write_mask = stencil_write_mask;
		CpuRasterSharedData.stencil_ref_val = stencil_ref_val;
		CpuRasterSharedData.color_mask = color_mask;
		if (!double_face)
		{
			CpuRasterSharedData.raster_flag |= RasterFlag::kFaceCulling;
		}
		else
		{
			CpuRasterSharedData.raster_flag &= ~RasterFlag::kFaceCulling;
		}
		CpuRasterSharedData.face_culling = double_face ? FaceCulling::None : FaceCulling::Back;

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
	}
}
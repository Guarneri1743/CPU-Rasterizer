#include "SkyboxShader.hpp"
#include "tinymath.h"
#include "GlobalShaderParams.hpp"
#include "Singleton.hpp"
#include "Sampling.hpp"

namespace Guarneri
{
	SkyboxShader::SkyboxShader() : Shader("skybox_shader")
	{
		this->skybox = true;
		this->ztest_func = CompareFunc::ALWAYS;
	}

	SkyboxShader::~SkyboxShader()
	{}

	v2f SkyboxShader::vertex_shader(const a2v& input) const
	{
		v2f o;
		auto clip_pos = projection * view * tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
		o.position = tinymath::vec4f(clip_pos.x, clip_pos.y, clip_pos.w, clip_pos.w);
		o.world_pos = (model * tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f)).xyz;
		o.shadow_coord = input.position;
		return o;
	}

	tinymath::Color SkyboxShader::fragment_shader(const v2f& input) const
	{
		tinymath::Color ret;

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::UV) != RenderFlag::DISABLE)
		{
			tinymath::vec2f uv = spherical_coord_to_uv(tinymath::vec3f(input.shadow_coord.xyz));
			return tinymath::Color(uv.x, uv.y, 0.0f, 1.0f);
		}
		else if ((INST(GlobalShaderParams).debug_flag & RenderFlag::IRRADIANCE_MAP) != RenderFlag::DISABLE)
		{
			if (global_shader_properties.has_cubemap(cubemap_prop) && global_shader_properties.get_cubemap(cubemap_prop)->sample_irradiance_map(input.shadow_coord.xyz, ret))
			{
				if (INST(GlobalShaderParams).color_space == ColorSpace::Linear)
				{
					ret = ret / (ret + tinymath::kColorWhite);
					ret = tinymath::pow(ret, 1.0f / 2.2f);
				}
				return ret;
			}
		}

		if (global_shader_properties.has_cubemap(cubemap_prop) && global_shader_properties.get_cubemap(cubemap_prop)->sample(input.shadow_coord.xyz, ret))
		{
			if (INST(GlobalShaderParams).color_space == ColorSpace::Linear)
			{
				ret = ret / (ret + tinymath::kColorWhite);
				ret = tinymath::pow(ret, 1.0f / 2.2f);
			}

			ret.a = 1.0f;

			return ret;
		}

		return tinymath::kColorBlack;
	}
}
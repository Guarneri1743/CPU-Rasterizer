#pragma once
#include "Shader.hpp"
#include "Sampling.hpp"
#include "CubeMap.hpp"

namespace CpuRasterizer
{
	class SkyboxShader : public Shader
	{
	public:
		SkyboxShader::SkyboxShader() : Shader("skybox_shader")
		{
			this->double_face = true;
			this->ztest_func = CompareFunc::kAlways;
		}

		SkyboxShader::~SkyboxShader()
		{}

		v2f SkyboxShader::vertex_shader(const a2v& input) const
		{
			v2f o;
			auto clip_pos = vp_matrix() * tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
			o.position = tinymath::vec4f(clip_pos.x, clip_pos.y, clip_pos.w, clip_pos.w);
			o.world_pos = (model() * tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f)).xyz;
			o.texcoord0 = input.position;
			return o;
		}

		tinymath::Color SkyboxShader::fragment_shader(const v2f& input) const
		{
			tinymath::Color ret;

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kUV) != RenderFlag::kNone)
			{
				tinymath::vec2f uv = spherical_coord_to_uv(tinymath::vec3f(input.texcoord0.xyz));
				return tinymath::Color(uv.x, uv.y, 0.0f, 1.0f);
			}
			else if ((CpuRasterSharedData.debug_flag & RenderFlag::kIrradianceMap) != RenderFlag::kNone)
			{
				if (ShaderPropertyMap::global_shader_properties.has_cubemap(cubemap_prop) && ShaderPropertyMap::global_shader_properties.get_cubemap(cubemap_prop)->sample_irradiance_map(input.texcoord0.xyz, ret))
				{
					if (CpuRasterSharedData.color_space == ColorSpace::kLinear)
					{
						ret = ret / (ret + tinymath::kColorWhite);
						ret = tinymath::pow(ret, 1.0f / 2.2f);
					}
					return ret;
				}
			}

			if (ShaderPropertyMap::global_shader_properties.has_cubemap(cubemap_prop) && ShaderPropertyMap::global_shader_properties.get_cubemap(cubemap_prop)->sample(input.texcoord0.xyz, ret))
			{
				if (CpuRasterSharedData.color_space == ColorSpace::kLinear)
				{
					ret = ret / (ret + tinymath::kColorWhite);
					ret = tinymath::pow(ret, 1.0f / 2.2f);
				}

				ret.a = 1.0f;

				return ret;
			}

			return tinymath::kColorBlack;
		}
	};
}
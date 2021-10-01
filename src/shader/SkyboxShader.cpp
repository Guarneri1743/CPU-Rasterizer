#include "SkyboxShader.hpp"
#include "TinyMath.h"
#include "GlobalShaderParams.hpp"
#include "Singleton.hpp"
#include "Sampling.hpp"

namespace Guarneri
{
	SkyboxShader::SkyboxShader() : Shader("skybox_shader")
	{
		this->skybox = true;
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

	Color SkyboxShader::fragment_shader(const v2f& input) const
	{
		Color ret;

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::UV) != RenderFlag::DISABLE)
		{
			tinymath::vec2f uv = spherical_coord_to_uv(tinymath::vec3f(input.shadow_coord.xyz));
			return Color(uv.x, uv.y, 0.0f, 1.0f);
		}
		else if ((INST(GlobalShaderParams).render_flag & RenderFlag::IRRADIANCE_MAP) != RenderFlag::DISABLE)
		{
			name2cubemap.at(cubemap_prop)->sample_irradiance_map(input.shadow_coord.xyz, ret);
			ret = ret / (ret + Color::WHITE);
			ret = Color::pow(ret, 1.0f / 2.2f);
			return ret;
		}

		if (name2cubemap.count(cubemap_prop) > 0 && name2cubemap.at(cubemap_prop)->sample(input.shadow_coord.xyz, ret))
		{
			ret = ret / (ret + Color::WHITE);
			ret = Color::pow(ret, 1.0f / 2.2f);
			return ret;
		}

		return Color::BLACK;
	}
}
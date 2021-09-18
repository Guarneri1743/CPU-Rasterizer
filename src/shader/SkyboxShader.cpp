#include "SkyboxShader.hpp"
#include <sstream>
#include "Vector4.hpp"
#include "GlobalShaderParams.hpp"
#include "Singleton.hpp"

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
		auto clip_pos = projection * view * Vector4(input.position.xyz(), 1.0f);
		o.position = Vector4(clip_pos.xy(), clip_pos.ww());
		o.world_pos = (model * Vector4(input.position.xyz(), 1.0f)).xyz();
		o.shadow_coord = input.position;
		return o;
	}

	Color SkyboxShader::fragment_shader(const v2f& input) const
	{
		Color ret;

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::UV) != RenderFlag::DISABLE)
		{
			return name2cubemap.at(cubemap_prop)->spherical_coord_to_uv(input.shadow_coord.xyz());
		}
		else if ((INST(GlobalShaderParams).render_flag & RenderFlag::IRRADIANCE_MAP) != RenderFlag::DISABLE)
		{
			name2cubemap.at(cubemap_prop)->sample_irradiance_map(input.shadow_coord.xyz(), ret);
			ret = ret / (ret + Color::WHITE);
			ret = Color::pow(ret, 1.0f / 2.2f);
			return ret;
		}

		if (name2cubemap.count(cubemap_prop) > 0 && name2cubemap.at(cubemap_prop)->sample(input.shadow_coord.xyz(), ret))
		{
			ret = ret / (ret + Color::WHITE);
			ret = Color::pow(ret, 1.0f / 2.2f);
			return ret;
		}

		return Color::BLACK;
	}

	std::string SkyboxShader::str() const
	{
		std::stringstream ss;
		ss << "SkyboxShader[" << this->id << "]" << std::endl;
		return ss.str();
	}
}
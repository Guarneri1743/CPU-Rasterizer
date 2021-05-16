#include "SkyboxShader.hpp"
#include <sstream>
#include <ostream>
#include "Vector4.hpp"
#include "Misc.hpp"
#include "Singleton.hpp"

namespace Guarneri
{
	SkyboxShader::SkyboxShader()
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
		Color sky_color;

		if ((INST(MiscParameter).render_flag & RenderFlag::UV) != RenderFlag::DISABLE)
		{
			int index;
			return name2cubemap.at(cubemap_prop)->sample(input.shadow_coord.xyz(), index);
		}

		if (name2cubemap.count(cubemap_prop) > 0 && name2cubemap.at(cubemap_prop)->sample(input.shadow_coord.xyz(), sky_color))
		{
			return sky_color;
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
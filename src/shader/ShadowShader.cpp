#include "ShadowShader.hpp"
#include <sstream>
#include "GlobalShaderParams.hpp"
#include "Singleton.hpp"

namespace Guarneri
{
	ShadowShader::ShadowShader() : Shader("shadow_shader")
	{
		this->shadow = true;
	}
	ShadowShader::~ShadowShader()
	{}

	v2f ShadowShader::vertex_shader(const a2v& input) const
	{
		v2f o;
		o.position = INST(GlobalShaderParams).main_light.light_space() * model * Vector4(input.position.xyz(), 1.0f);
		o.shadow_coord = o.position;
		return o;
	}

	Color ShadowShader::fragment_shader(const v2f& input) const
	{
		//auto shadow_coord = input.shadow_coord * 0.5f + 0.5f;
		//printf("shadow: %f, %f, %f %f\npos: %f, %f, %f %f\n", shadow_coord.x, shadow_coord.y, shadow_coord.z, shadow_coord.w, input.position.x, input.position.y, input.position.z, input.position.w);
		UNUSED(input);
		return Color::BLACK;
	}

	std::string ShadowShader::str() const
	{
		std::stringstream ss;
		ss << "ShadowShader[" << this->id << "]" << std::endl;
		return ss.str();
	}
}
#include "LightShader.hpp"
#include <iostream>
#include <sstream>

namespace Guarneri
{
	LightShader::LightShader() : Shader("light_shader")
	{}

	LightShader::~LightShader()
	{}

	v2f LightShader::vertex_shader(const a2v& input) const
	{
		v2f o;
		auto opos = Vector4(input.position.xyz(), 1.0f);
		auto wpos = model * opos;
		auto cpos = projection * view * wpos;
		o.position = cpos;
		return o;
	}

	Color LightShader::fragment_shader(const v2f& input) const
	{
		UNUSED(input);
		return Color::WHITE;
	}

	std::string LightShader::str() const
	{
		std::stringstream ss;
		ss << "LightShader[" << this->id << "]" << std::endl;
		return ss.str();
	}
}
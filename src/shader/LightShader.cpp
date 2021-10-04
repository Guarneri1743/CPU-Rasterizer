#include "LightShader.hpp"
#include <iostream>

namespace Guarneri
{
	LightShader::LightShader() : Shader("light_shader")
	{}

	LightShader::~LightShader()
	{}

	v2f LightShader::vertex_shader(const a2v& input) const
	{
		v2f o;
		auto opos = tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
		auto wpos = model * opos;
		auto cpos = projection * view * wpos;
		o.position = cpos;
		return o;
	}

	tinymath::Color LightShader::fragment_shader(const v2f& input, const Vertex& ddx, const Vertex& ddy) const
	{
		UNUSED(input);
		UNUSED(ddx);
		UNUSED(ddy);
		return tinymath::kColorWhite;
	}
}
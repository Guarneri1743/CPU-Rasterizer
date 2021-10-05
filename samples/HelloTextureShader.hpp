#pragma once
#include "Shader.hpp"

using namespace CpuRasterizor;

class HelloTextureShader : public Shader
{
public:
	HelloTextureShader() : Shader("sample_shader") {}

	v2f vertex_shader(const a2v& input) const
	{
		v2f o;
		auto opos = tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
		auto m = model();
		auto vp = vp_matrix();
		auto wpos = m * opos;
		auto cpos = vp * wpos;
		o.position = cpos;
		return o;
	}

	tinymath::Color fragment_shader(const v2f& input) const
	{
		UNUSED(input);
		return tinymath::kColorRed;
	}
};
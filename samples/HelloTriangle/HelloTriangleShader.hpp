#pragma once
#include "ShaderProgram.hpp"

using namespace CpuRasterizer;
using namespace tinymath;

class HelloTriangleShader : public ShaderProgram
{
public:
	HelloTriangleShader() : ShaderProgram("sample_shader") {}

	v2f vertex_shader(const a2v& input) const
	{
		v2f o;
		auto opos = vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
		auto m = model();
		auto vp = vp_matrix();
		auto wpos = m * opos;
		auto cpos = vp * wpos;
		o.position = cpos;
		return o;
	}

	Color fragment_shader(const v2f& input) const
	{
		UNUSED(input);

		// draw a white triangle
		return kColorWhite;
	}
};
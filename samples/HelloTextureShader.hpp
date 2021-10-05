#pragma once
#include "Shader.hpp"

using namespace CpuRasterizor;
using namespace tinymath;

class HelloTextureShader : public Shader
{
public:
	HelloTextureShader() : Shader("sample_shader") {}

	v2f vertex_shader(const a2v& input) const
	{
		v2f o;
		auto opos = vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
		auto m = model();
		auto vp = vp_matrix();
		auto wpos = m * opos;
		auto cpos = vp * wpos;
		o.position = cpos;
		o.uv = input.uv;
		return o;
	}

	Color fragment_shader(const v2f& input) const
	{
		UNUSED(input);

		// visualize uv
		//return input.uv;

		// sample texture
		Color c;
		if (local_properties.has_texture(123))
		{
			local_properties.get_texture(123)->sample(input.uv.x, input.uv.y, c);
		}

		return c;
	}
};
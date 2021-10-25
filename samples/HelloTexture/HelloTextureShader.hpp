#pragma once
#include "ShaderProgram.hpp"

using namespace CpuRasterizer;
using namespace tinymath;

class HelloTextureShader : public ShaderProgram
{
public:
	HelloTextureShader() : ShaderProgram("sample_shader") {}

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

		// sample texture
		Color c;
		property_name tex_prop = 123;
		if (local_properties.has_texture(tex_prop))
		{
			local_properties.get_texture(tex_prop)->sample(input.uv.x, input.uv.y, c);
		}

		return c;
	}
};
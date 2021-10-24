#pragma once
#include "Shader.hpp"
#include "Sampling.hpp"

using namespace CpuRasterizer;
using namespace tinymath;

class HelloTexture3DShader : public Shader
{
public:
	HelloTexture3DShader() : Shader("sample_shader") {}

	v2f vertex_shader(const a2v& input) const
	{
		v2f o;
		auto opos = vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
		auto wpos = model() * opos;
		auto cpos = mvp_matrix() * opos;
		o.position = cpos;
		o.texcoord0 = opos;
		o.uv = input.uv;
		return o;
	}

	Color fragment_shader(const v2f& input) const
	{
		UNUSED(input);

		// sample texture
		Color c;
		if (local_properties.has_texture(123))
		{
			auto uvw = input.texcoord0;
			uvw = uvw  * 0.5f + 0.5f;
			local_properties.get_texture(123)->sample(uvw.x, uvw.y, uvw.x, c);
		}

		return c;
	}
};
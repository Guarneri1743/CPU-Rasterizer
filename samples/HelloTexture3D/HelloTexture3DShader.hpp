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
		
		// todo: strinify the key
		property_name tex_prop = 123;
		if (local_properties.has_int(tex_prop))
		{
			auto tex_id = local_properties.get_int(tex_prop);
			auto uvw = input.texcoord0;
			uvw = uvw  * 0.5f + 0.5f;
			local_properties.get_texture(tex_id)->sample(uvw.x, uvw.y, uvw.x, c);
		}

		return c;
	}
};
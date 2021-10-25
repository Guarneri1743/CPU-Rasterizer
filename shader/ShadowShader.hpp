#pragma once
#include "Shader.hpp"

namespace CpuRasterizer
{
	class ShadowShader : public Shader
	{
	public:
		ShadowShader() : Shader("shadow_shader")
		{}

		~ShadowShader()
		{}

		v2f vertex_shader(const a2v& input) const
		{
			v2f o;
			o.position = CpuRasterSharedData.main_light.light_space() * model() * tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
			o.texcoord0 = o.position;
			return o;
		}

		tinymath::Color fragment_shader(const v2f& input) const
		{
			UNUSED(input);
			return tinymath::kColorBlack;
		}
	};
}
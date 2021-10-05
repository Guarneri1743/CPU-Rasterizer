#pragma once
#include "Shader.hpp"

namespace CpuRasterizor
{
	class ShadowShader : public Shader
	{
	public:
		ShadowShader() : Shader("shadow_shader")
		{
			this->shadow = true;
		}
		~ShadowShader()
		{}

		v2f vertex_shader(const a2v& input) const
		{
			v2f o;
			o.position = CpuRasterSharedData.main_light.light_space() * model() * tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
			o.shadow_coord = o.position;
			return o;
		}

		tinymath::Color fragment_shader(const v2f& input) const
		{
			//auto shadow_coord = input.shadow_coord * 0.5f + 0.5f;
			//printf("shadow: %f, %f, %f %f\npos: %f, %f, %f %f\n", shadow_coord.x, shadow_coord.y, shadow_coord.z, shadow_coord.w, input.position.x, input.position.y, input.position.z, input.position.w);
			UNUSED(input);
			return tinymath::kColorBlack;
		}
	};
}
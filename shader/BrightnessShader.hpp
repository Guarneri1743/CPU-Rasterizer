#pragma once
#include "ShaderProgram.hpp"
using namespace tinymath;

namespace CpuRasterizer
{
	class BrightnessShader : public ShaderProgram
	{
	public:
		BrightnessShader() : ShaderProgram("bright_extraction_shader") {}

		v2f BrightnessShader::vertex_shader(const a2v& input) const
		{
			v2f o;
			o.position = input.position;
			o.uv = input.uv;
			return o;
		}

		tinymath::Color BrightnessShader::fragment_shader(const v2f& input) const
		{
			Color scene_color;
			local_properties.get_texture(scene_color_prop)->sample(input.uv.x, input.uv.y, scene_color);
			float brightness = dot(vec3f(scene_color.x, scene_color.y, scene_color.z), vec3f(0.2126f, 0.7152f, 0.0722f));
			if (brightness > 1.0)
				return scene_color;
			else
				return kColorBlack;
		}
	};
}
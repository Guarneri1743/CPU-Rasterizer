#pragma once
#include "ShaderProgram.hpp"
using namespace tinymath;

namespace CpuRasterizer
{
	class BloomShader : public ShaderProgram
	{
	public:
		BloomShader() : ShaderProgram("bloom_shader") {}

		v2f BloomShader::vertex_shader(const a2v& input) const
		{
			v2f o;
			o.position = input.position;
			o.uv = input.uv;
			return o;
		}

		tinymath::Color BloomShader::fragment_shader(const v2f& input) const
		{
			const float gamma = 2.2f;
			Color scene_color, bloom_color;
			local_properties.get_texture(scene_color_prop)->sample(input.uv.x, input.uv.y, scene_color);
			local_properties.get_texture(bloom_bright_color_prop)->sample(input.uv.x, input.uv.y, bloom_color);
			float exposure = local_properties.get_float(exposure_prop);
			vec4f ret = vec4f(1.0f) - tinymath::exp(-scene_color * exposure);   
			ret = ret / (ret + tinymath::kColorWhite);
			ret = tinymath::pow(ret, 1.0f / 2.2f);
			return Color(ret.x, ret.y, ret.z, 1.0f);
		}
	};
}
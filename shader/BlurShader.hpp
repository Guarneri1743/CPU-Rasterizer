#pragma once
#include "ShaderProgram.hpp"
using namespace tinymath;

namespace CpuRasterizer
{
	constexpr float gaussian_weights[5] = { 0.227f, 0.195f, 0.122f, 0.054f, 0.016f };

	class BlurShader : public ShaderProgram
	{
	public:
		BlurShader() : ShaderProgram("blur_shader") {}

		v2f BlurShader::vertex_shader(const a2v& input) const
		{
			v2f o;
			o.position = input.position;
			o.uv = input.uv;
			return o;
		}

		tinymath::Color BlurShader::fragment_shader(const v2f& input) const
		{
			vec4f screen_param = local_properties.get_float4(screen_param_prop);
			vec2f tex_offset = 1.0f / vec2f(screen_param.x, screen_param.y);
			Color ret;
			local_properties.get_texture(bloom_bright_color_prop)->sample(input.uv.x, input.uv.y, ret);
			ret *= gaussian_weights[0];
			
			int horizontal = local_properties.get_int(124);

			if (horizontal == 1)
			{
				for (int i = 1; i < 5; ++i)
				{
					Color c1, c2;
					local_properties.get_texture(bloom_bright_color_prop)->sample(input.uv.x + tex_offset.x * i, input.uv.y, c1);
					local_properties.get_texture(bloom_bright_color_prop)->sample(input.uv.x - tex_offset.x * i, input.uv.y, c2);
					ret += c1 * gaussian_weights[i]; 
					ret += c2 * gaussian_weights[i];
				}
			}
			else
			{
				for (int i = 1; i < 5; ++i)
				{
					Color c1, c2;
					local_properties.get_texture(bloom_bright_color_prop)->sample(input.uv.x, input.uv.y + tex_offset.y * i, c1);
					local_properties.get_texture(bloom_bright_color_prop)->sample(input.uv.x, input.uv.y - tex_offset.y * i, c2);
					ret += c1 * gaussian_weights[i];
					ret += c2 * gaussian_weights[i];
				}
			}

			return ret;
		}
	};
}
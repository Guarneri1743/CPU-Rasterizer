#pragma once
#include "ShaderProgram.hpp"

using namespace CpuRasterizer;
using namespace tinymath;

class HelloLightingShader : public ShaderProgram
{
public:
	HelloLightingShader() : ShaderProgram("sample_shader") {}

	v2f vertex_shader(const a2v& input) const
	{
		v2f o;
		auto opos = vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
		auto m = model();
		auto vp = vp_matrix();
		auto wpos = m * opos;
		auto v = view();
		auto p = projection();
		auto cpos = vp * wpos;
		o.position = cpos;
		o.world_pos = wpos.xyz;
		o.uv = input.uv;
		mat3x3 normal_matrix = mat4x4_to_mat3x3(transpose(inverse(m)));
		o.normal = normal_matrix * input.normal;
		o.color = input.color;
		return o;
	}

	Color fragment_shader(const v2f& input) const
	{
		vec4f albedo = local_properties.get_float4(albedo_prop);
		vec4f light_color = local_properties.get_float4(light_color_prop);
		float intensity = local_properties.get_float(light_intensity_prop);

		vec3f normal = normalize(input.normal);
		vec3f light_dir = normalize(local_properties.get_float4(light_direction_prop)).xyz;
		float ndl = max(dot(normal, light_dir), 0.0f);

		vec3f cam_pos = local_properties.get_float4(cam_position_prop).xyz;
		vec3f view_dir = normalize(cam_pos - input.world_pos).xyz;
		vec3f half_dir = normalize(view_dir + light_dir);
		float spec = tinymath::pow(max(dot(normal, half_dir), 0.0f), 2048.0f);
		spec = min(spec, 1.0f);
		spec = max(spec, 0.0f);

		vec4f ret = ((albedo * ndl + spec) * light_color * intensity).xyzw;

		ret = ret / (ret + kColorWhite);
		ret = pow(ret, 1.0f / 2.2f);

		return ret;
	}
};
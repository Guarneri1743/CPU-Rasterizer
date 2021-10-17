#pragma once
#include "Shader.hpp"

using namespace CpuRasterizor;
using namespace tinymath;

class HelloLightingShader : public Shader
{
public:
	HelloLightingShader() : Shader("sample_shader") {}

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
		o.uv = input.uv;
		mat3x3 normal_matrix = mat4x4_to_mat3x3(transpose(inverse(m)));
		o.normal = normal_matrix * input.normal;
		return o;
	}

	Color fragment_shader(const v2f& input) const
	{
		vec4f albedo = local_properties.get_float4(albedo_prop);
		vec4f diffuse = local_properties.get_float4(light_diffuse);
		float intensity = local_properties.get_float(light_intensity);

		vec3f normal = input.normal;
		vec3f light_dir = local_properties.get_float4(light_direction).xyz;
		float ndl = dot(normal, light_dir);

		return albedo * diffuse * ndl * intensity;
	}
};
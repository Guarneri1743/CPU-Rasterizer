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
		tinymath::mat3x3 normal_matrix = tinymath::mat4x4_to_mat3x3(tinymath::transpose(tinymath::inverse(m)));
		o.normal = normal_matrix * input.normal;
		return o;
	}

	Color fragment_shader(const v2f& input) const
	{
		tinymath::vec4f albedo;
		tinymath::vec4f direction;
		tinymath::vec4f diffuse;
		float intensity;

		albedo = local_properties.get_float4(albedo_prop);
		direction = local_properties.get_float4(light_direction);
		diffuse = local_properties.get_float4(light_diffuse);
		intensity = local_properties.get_float(light_intensity);

		vec3f normal = input.normal;
		vec3f light_dir = direction.xyz;
		float ndl = dot(normal, light_dir);

		return albedo * diffuse * ndl * intensity;
	}
};
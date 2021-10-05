#include "Shader.hpp"
#include <algorithm>
#include <iostream>
#include "Marcos.h"
#include "Singleton.hpp"

namespace Guarneri
{
	ShaderPropertyMap Shader::global_shader_properties;

	Shader* Shader::error_shader = new Shader("error_shader", true);

	Shader::Shader(std::string name) : Shader(name, false) {}

	Shader::Shader(std::string name, bool is_error_shader)
	{
		this->color_mask = (ColorMask::kRed | ColorMask::kGreen | ColorMask::kBlue | ColorMask::kAlpha);
		this->stencil_func = CompareFunc::kAlways;
		this->stencil_pass_op = StencilOp::kKeep;
		this->stencil_fail_op = StencilOp::kKeep;
		this->stencil_zfail_op = StencilOp::kKeep;
		this->stencil_read_mask = 0xFF;
		this->stencil_write_mask = 0xFF;
		this->stencil_ref_val = 0;
		this->ztest_func = CompareFunc::kLEqual;
		this->zwrite_mode = ZWrite::kOn;
		this->src_factor = BlendFactor::kSrcAlpha;
		this->dst_factor = BlendFactor::kOneMinusSrcAlpha;
		this->blend_op = BlendOp::kAdd;
		this->transparent = false;
		this->double_face = false;
		this->skybox = false;
		this->shadow = false;
		this->name = name;
		this->is_error_shader = is_error_shader;
	}

	Shader::~Shader()
	{}

	v2f Shader::vertex_shader(const a2v& input) const
	{
		v2f o;
		auto opos = tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
		auto wpos = model * opos;
		auto cpos = vp_matrix * wpos;
		o.position = cpos;
		o.world_pos = wpos.xyz;
		o.color = input.color;
		tinymath::mat3x3 normal_matrix = tinymath::transpose(tinymath::inverse(tinymath::mat4x4_to_mat3x3(model)));
		o.normal = normal_matrix * input.normal;
		o.uv = input.uv;
		return o;
	}

	tinymath::Color Shader::fragment_shader(const v2f& input) const
	{
		if (is_error_shader) { return kErrorColor; }

		auto main_light = INST(GlobalShaderParams).main_light;
		auto point_lights = INST(GlobalShaderParams).point_lights;

		tinymath::vec3f cam_pos = INST(GlobalShaderParams).camera_pos;
		tinymath::vec3f wpos = input.world_pos;
		tinymath::vec4f screen_pos = input.position;

		tinymath::vec3f normal = tinymath::normalize(input.normal);
		tinymath::vec3f view_dir = tinymath::normalize(cam_pos - wpos);

		tinymath::Color ret = tinymath::kColorBlack;
		tinymath::Color albedo = tinymath::kColorWhite;
		if (local_properties.has_texture(albedo_prop) && local_properties.get_texture(albedo_prop)->sample(input.uv.x, input.uv.y, albedo))
		{
			if (INST(GlobalShaderParams).color_space == ColorSpace::kLinear)
			{
				albedo = tinymath::pow(albedo, 2.2f);
			}
		}

		if (local_properties.has_float4(tint_color_prop))
		{
			albedo *= tinymath::Color(local_properties.get_float4(tint_color_prop));
		}

		float ndl = tinymath::dot(normal, -tinymath::normalize(main_light.forward));

		ret += ndl * albedo;

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::kUV) != RenderFlag::kNone)
		{
			return tinymath::Color(input.uv.x, input.uv.y, 0.0f, 1.0f);
		}

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::kVertexColor) != RenderFlag::kNone)
		{
			return input.color;
		}

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::kNormal) != RenderFlag::kNone)
		{
			return normal;
		}

		if (INST(GlobalShaderParams).color_space == ColorSpace::kLinear)
		{
			ret = ret / (ret + tinymath::kColorWhite);
			ret = tinymath::pow(ret, 1.0f / 2.2f);
		}

		return tinymath::Color(ret.r, ret.g, ret.b, 1.0f);
	}

	float Shader::linearize_01depth(const float& depth, const float& near, const float& far)
	{
		return (linearize_depth(depth, near, far) - near) / (far - near);
	}

	float Shader::linearize_depth(const float& depth, const float& near, const float& far)
	{
	#ifdef GL_LIKE
		float ndc_z = depth * 2.0f - 1.0f;  // [0, 1] -> [-1, 1] (GL)
	#else
		float ndc_z = depth; // [0, 1] (DX)
	#endif

	#ifdef LEFT_HANDED 
	#ifdef GL_LIKE
		return (2.0f * near * far) / (far + near - ndc_z * (far - near));
	#else
		return (far * near) / (far - (far - near) * ndc_z);
	#endif
	#else
	#ifdef GL_LIKE
		return (2.0f * near * far) / (-(far + near) - ndc_z * (far - near));
	#else
		return (far * near) / (-far - (far - near) * ndc_z);
	#endif
	#endif
	}
}
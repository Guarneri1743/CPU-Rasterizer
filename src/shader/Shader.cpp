#include "Shader.hpp"
#include <algorithm>
#include <iostream>
#include "Marcos.h"
#include "Singleton.hpp"

namespace Guarneri
{
	Shader* Shader::error_shader = new Shader("error_shader", true);

	Shader::Shader(std::string name) : Shader(name, false) {}

	Shader::Shader(std::string name, bool is_error_shader)
	{
		this->color_mask = (ColorMask::R | ColorMask::G | ColorMask::B | ColorMask::A);
		this->stencil_func = CompareFunc::ALWAYS;
		this->stencil_pass_op = StencilOp::KEEP;
		this->stencil_fail_op = StencilOp::KEEP;
		this->stencil_zfail_op = StencilOp::KEEP;
		this->stencil_read_mask = 0xFF;
		this->stencil_write_mask = 0xFF;
		this->stencil_ref_val = 0;
		this->ztest_func = CompareFunc::LEQUAL;
		this->zwrite_mode = ZWrite::ON;
		this->src_factor = BlendFactor::SRC_ALPHA;
		this->dst_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;
		this->blend_op = BlendOp::ADD;
		this->lighting_param = LightingData();
		this->transparent = false;
		this->double_face = false;
		this->skybox = false;
		this->shadow = false;
		this->shadowmap = nullptr;
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
		if (name2tex.count(albedo_prop) > 0 && name2tex.at(albedo_prop)->sample(input.uv.x, input.uv.y, albedo))
		{
			if (INST(GlobalShaderParams).color_space == ColorSpace::Linear)
			{
				albedo = tinymath::pow(albedo, 2.2f);
			}
		}

		if (name2float4.count(tint_color_prop) > 0)
		{
			albedo *= tinymath::Color(name2float4.at(tint_color_prop));
		}

		float ndl = tinymath::dot(normal, -tinymath::normalize(main_light.forward));

		ret += ndl * albedo;

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::UV) != RenderFlag::DISABLE)
		{
			return tinymath::Color(input.uv.x, input.uv.y, 0.0f, 1.0f);
		}

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::VERTEX_COLOR) != RenderFlag::DISABLE)
		{
			return input.color;
		}

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::NORMAL) != RenderFlag::DISABLE)
		{
			return normal;
		}

		if (INST(GlobalShaderParams).color_space == ColorSpace::Linear)
		{
			ret = ret / (ret + tinymath::kColorWhite);
			ret = tinymath::pow(ret, 1.0f / 2.2f);
		}

		return tinymath::Color(ret.r, ret.g, ret.b, 1.0f);
	}
}
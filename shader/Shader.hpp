#pragma once
#include <unordered_map>
#include "tinymath.h"
#include "Singleton.hpp"
#include "Light.hpp"
#include "GlobalShaderParams.hpp"
#include "ShaderPropertyMap.hpp"
#include "RasterAttributes.hpp"

#undef near
#undef far

namespace CpuRasterizor
{
	static tinymath::Color mip_colors[kMaxMip] =
	{
		tinymath::Color(1.f, 0.f, 0.f, 1.f),
		tinymath::Color(0.f, 0.f, 1.f, 1.f),
		tinymath::Color(1.f, 0.5f, 0.f, 1.f),
		tinymath::Color(1.f, 0.f, 0.5f, 1.f),
		tinymath::Color(0.f, 0.5f, 0.5f, 1.f),
		tinymath::Color(0.f, 0.25f, 0.5f, 1.f),
		tinymath::Color(0.25f, 0.5f, 0.f, 1.f),
		tinymath::Color(0.5f, 0.f, 1.f, 1.f),
	};

	struct a2v
	{
		tinymath::vec4f position;
		tinymath::vec2f uv;
		tinymath::vec4f color;
		tinymath::vec3f normal;
		tinymath::vec3f tangent;
	};

	struct v2f
	{
		tinymath::vec4f position;
		tinymath::vec3f world_pos;
		tinymath::vec2f uv;
		tinymath::vec4f color;
		tinymath::vec3f tangent;
		tinymath::vec3f bitangent;
		tinymath::vec3f normal;
		tinymath::vec4f shadow_coord;

		Fragment ddx;
		Fragment ddy;
	};

	class Shader
	{
	public:
		ColorMask color_mask;
		CompareFunc stencil_func;
		StencilOp stencil_pass_op;
		StencilOp stencil_fail_op;
		StencilOp stencil_zfail_op;
		uint8_t stencil_ref_val;
		uint8_t stencil_write_mask;
		uint8_t stencil_read_mask;
		CompareFunc ztest_func;
		ZWrite zwrite_mode;
		BlendFactor src_factor;
		BlendFactor dst_factor;
		BlendOp blend_op;
		bool transparent;
		bool double_face;
		bool skybox;
		bool shadow;
		bool discarded = false;
		std::string name;
		ShaderPropertyMap local_properties;

	public:
		virtual ~Shader() {}

		Shader(std::string name) {
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
		}

		virtual v2f vertex_shader(const a2v& input) const
		{
			v2f o;
			auto opos = tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
			auto wpos = model() * opos;
			auto cpos = vp_matrix() * wpos;
			o.position = cpos;
			o.world_pos = wpos.xyz;
			o.color = input.color;
			tinymath::mat3x3 normal_matrix = tinymath::transpose(tinymath::inverse(tinymath::mat4x4_to_mat3x3(model())));
			o.normal = normal_matrix * input.normal;
			o.uv = input.uv;
			return o;
		}

		virtual tinymath::Color fragment_shader(const v2f& input) const
		{
			auto main_light = CpuRasterSharedData.main_light;
			auto point_lights = CpuRasterSharedData.point_lights;

			tinymath::vec3f cam_pos = CpuRasterSharedData.camera_pos;
			tinymath::vec3f wpos = input.world_pos;
			tinymath::vec4f screen_pos = input.position;

			tinymath::vec3f normal = tinymath::normalize(input.normal);
			tinymath::vec3f view_dir = tinymath::normalize(cam_pos - wpos);

			tinymath::Color ret = tinymath::kColorBlack;
			tinymath::Color albedo = tinymath::kColorWhite;
			if (local_properties.has_texture(albedo_prop) && local_properties.get_texture(albedo_prop)->sample(input.uv.x, input.uv.y, input.ddx.uv, input.ddy.uv, albedo))
			{
				if (CpuRasterSharedData.color_space == ColorSpace::kLinear)
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

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kUV) != RenderFlag::kNone)
			{
				return tinymath::Color(input.uv.x, input.uv.y, 0.0f, 1.0f);
			}

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kVertexColor) != RenderFlag::kNone)
			{
				return input.color;
			}

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kNormal) != RenderFlag::kNone)
			{
				return normal;
			}

			if (CpuRasterSharedData.color_space == ColorSpace::kLinear)
			{
				ret = ret / (ret + tinymath::kColorWhite);
				ret = tinymath::pow(ret, 1.0f / 2.2f);
			}

			return tinymath::Color(ret.r, ret.g, ret.b, 1.0f);
		}

		static inline float linearize_depth(const float& depth, const float& near, const float& far)
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

		static inline float linearize_01depth(const float& depth, const float& near, const float& far)
		{
			return (linearize_depth(depth, near, far) - near) / (far - near);
		}

		inline tinymath::mat4x4 model() const { return local_properties.get_mat4x4(mat_model); }
		inline tinymath::mat4x4 view() const { return local_properties.get_mat4x4(mat_view); }
		inline tinymath::mat4x4 projection() const { return local_properties.get_mat4x4(mat_projection); }
		inline tinymath::mat4x4 vp_matrix() const { return local_properties.get_mat4x4(mat_vp); }
		inline tinymath::mat4x4 mvp_matrix() const { return local_properties.get_mat4x4(mat_mvp); }
	};
}
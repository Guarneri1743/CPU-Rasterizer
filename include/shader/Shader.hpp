#pragma once
#include <unordered_map>
#include "Object.hpp"
#include "Light.hpp"
#include "GlobalShaderParams.hpp"
#include "tinymath.h"
#include "ShaderPropertyMap.hpp"

namespace Guarneri
{
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
	};

	struct LightingData
	{
		tinymath::vec3f f0;
		float roughness;
		float metallic;
		float glossiness;

		LightingData()
		{
			glossiness = 32.0f;
			f0 = tinymath::vec3f(0.04f);
			metallic = 0.0f;
			roughness = 0.5f;
		}
	};

	constexpr tinymath::Color kErrorColor = tinymath::Color(199.0f, 0.0f, 106.0f, 1.0f);

	class Shader : public Object
	{
	public:
		tinymath::mat4x4 model, view, projection;
		tinymath::mat4x4 vp_matrix;
		tinymath::mat4x4 mvp_matrix;
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
		LightingData lighting_param;
		bool discarded = false;
		std::string name;
		ShaderPropertyMap local_properties;
		static ShaderPropertyMap global_shader_properties;

	public:
		Shader(std::string name);
		virtual ~Shader();
		virtual v2f vertex_shader(const a2v& input) const;
		virtual tinymath::Color fragment_shader(const v2f& input) const;
		static float linearize_depth(const float& depth, const float& near, const float& far);
		static float linearize_01depth(const float& depth, const float& near, const float& far);
		static Shader*  get_error_shader() { return error_shader; }

	protected:
		Shader(std::string name, bool is_error);
		bool is_error_shader;
		static Shader* error_shader;
	};
}
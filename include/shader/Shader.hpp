#pragma once
#include <unordered_map>
#include "Object.hpp"
#include "Light.hpp"
#include "GlobalShaderParams.hpp"
#include "tinymath.h"
#include "Texture.hpp"
#include "CubeMap.hpp"

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
		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, tinymath::vec4f> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, std::shared_ptr<Texture>> name2tex;
		std::unordered_map<property_name, std::shared_ptr<CubeMap>> name2cubemap;
		std::unordered_map<property_name, std::string> keywords;
		RawBuffer<float>* shadowmap;
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
		bool normal_map = false;
		std::string name;

	public:
		Shader(std::string name);
		virtual ~Shader();
		virtual v2f vertex_shader(const a2v& input) const;
		virtual tinymath::Color fragment_shader(const v2f& input) const;

		static Shader*  get_error_shader() { return error_shader; }

	protected:
		Shader(std::string name, bool is_error);
		bool is_error_shader;
		static Shader* error_shader;
	};
}
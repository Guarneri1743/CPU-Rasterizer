#pragma once
#include <memory>
#include <string>
#include "tinymath.h"
#include "Object.hpp"
#include "Shader.hpp"
#include "ShadowShader.hpp"
#include "SkyboxShader.hpp"

namespace Guarneri
{
	class Material : public Object
	{
	public:
		std::string material_name;
		std::string meta_path;
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
		bool double_face;
		bool transparent;
		bool cast_shadow;
		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, tinymath::vec4f> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, std::shared_ptr<Texture>> name2tex;
		std::unordered_map<property_name, std::shared_ptr<CubeMap>> name2cubemap;
		LightingData lighting_param;
		std::shared_ptr<Shader> target_shader;
		std::shared_ptr<Shader> shadow_caster;

	public:
		Material();
		Material(std::string name);
		Material(std::string name, std::shared_ptr<Shader> shader);
		Material(const Material& other);
		~Material();

		Shader* get_shader(const RenderPass& pass) const;
		void set_shadowmap(RawBuffer<float>* shadowmap);
		void sync(Shader* shader, const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void sync(const tinymath::mat4x4& m, const tinymath::mat4x4& v, const tinymath::mat4x4& p);
		void set_int(const property_name& name, const int& val);
		void set_float4(const property_name& name, const tinymath::vec4f& val);
		void set_float(const property_name& name, const float& val);
		void set_texture(const property_name& name, std::shared_ptr<Texture> tex);
		void set_cubemap(const property_name& name, std::shared_ptr<CubeMap> cubemap);
		int get_int(const property_name& name) const;
		tinymath::vec4f get_float4(const property_name& name) const;
		float get_float(const property_name& name) const;
		std::shared_ptr<Texture> get_texture(const property_name& name) const;
		std::shared_ptr<CubeMap> get_cubemap(const property_name& name) const;
		Material& operator =(const Material& other);
		void copy(const Material& other);
	};
}
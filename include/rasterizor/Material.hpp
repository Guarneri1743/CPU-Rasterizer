#pragma once
#include <memory>
#include <string>
#include "tinymath.h"
#include "Define.hpp"
#include "Object.hpp"
#include "ShaderPropertyMap.hpp"

namespace CpuRasterizor
{
	class Shader;

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
		std::shared_ptr<Shader> target_shader;
		std::shared_ptr<Shader> shadow_caster;
		ShaderPropertyMap local_properties;

	public:
		Material();
		Material(std::string name);
		Material(std::string name, std::shared_ptr<Shader> shader);
		Material(const Material& other);
		~Material();

		Shader* get_shader(RenderPass pass) const;
		void sync(Shader* shader);
		void sync();
		
		Material& operator =(const Material& other);
		void copy(const Material& other);
	};
}
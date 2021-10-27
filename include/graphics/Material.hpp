#pragma once
#include <memory>
#include <string>
#include "tinymath.h"
#include "Define.hpp"
#include "Object.hpp"
#include "ShaderPropertyMap.hpp"

namespace CpuRasterizer
{
	class ShaderProgram;

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
		stencil_t stencil_ref_val;
		stencil_t stencil_write_mask;
		stencil_t stencil_read_mask;
		CompareFunc ztest_func;
		bool zwrite_on;
		bool stencil_on;
		BlendFactor src_factor;
		BlendFactor dst_factor;
		BlendFunc blend_op;
		bool double_face;
		bool transparent;
		bool cast_shadow;
		int32_t render_queue;
		std::shared_ptr<ShaderProgram> target_shader;
		std::shared_ptr<ShaderProgram> shadow_caster;
		resource_id target_shader_id;
		resource_id shadow_caster_id;
		ShaderPropertyMap local_properties;

	public:
		Material();
		Material(std::string name);
		Material(std::string name, std::shared_ptr<ShaderProgram> shader);
		Material(const Material& other);
		~Material();
		
		void initialize();
		resource_id get_shader(RenderPass pass) const;
		void use(RenderPass pass);
		
		Material& operator =(const Material& other);
		void copy(const Material& other);
	};
}
#ifndef _MATERIAL_
#define _MATERIAL_
#include <Guarneri.hpp>

namespace Guarneri {
	class Material : public Object {
	public:
		Material() {
			this->target_shader = std::make_unique<Shader>();
			this->shadow_caster = std::make_unique<ShadowShader>();
			this->color_mask = (ColorMask::R | ColorMask::G | ColorMask::B | ColorMask::A);
			this->stencil_func = CompareFunc::ALWAYS;
			this->stencil_pass_op = StencilOp::KEEP;
			this->stencil_fail_op = StencilOp::KEEP;
			this->stencil_zfail_op = StencilOp::KEEP;
			this->stencil_read_mask = 0xFF;
			this->stencil_write_mask = 0xFF;
			this->stencil_ref_val = 0;
			this->ztest_func = CompareFunc::LESS;
			this->zwrite_mode = ZWrite::ON;
			this->src_factor = BlendFactor::SRC_ALPHA;
			this->dst_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;
			this->blend_op = BlendOp::ADD;
			this->double_face = false;
			this->transparent = false;
			this->cast_shadow = false;
		}

		Material(std::unique_ptr<Shader>& shader) {
			this->target_shader = std::move(shader);
			this->shadow_caster = std::make_unique<ShadowShader>();
			this->color_mask = (ColorMask::R | ColorMask::G | ColorMask::B | ColorMask::A);
			this->stencil_func = CompareFunc::ALWAYS;
			this->stencil_pass_op = StencilOp::KEEP;
			this->stencil_fail_op = StencilOp::KEEP;
			this->stencil_zfail_op = StencilOp::KEEP;
			this->stencil_read_mask = 0xFF;
			this->stencil_write_mask = 0xFF;
			this->stencil_ref_val = 0;
			this->ztest_func = CompareFunc::LESS;
			this->zwrite_mode = ZWrite::ON;
			this->src_factor = BlendFactor::SRC_ALPHA;
			this->dst_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;
			this->blend_op = BlendOp::ADD;
			this->double_face = false;
			this->transparent = false;
			this->cast_shadow = false;
		}

		Material(std::unique_ptr<SkyboxShader>& shader) {
			auto shader_ptr = shader.release();
			this->target_shader = std::unique_ptr <Shader>((Shader*)(shader_ptr));
			this->shadow_caster = std::make_unique<ShadowShader>();
			this->color_mask = (ColorMask::R | ColorMask::G | ColorMask::B | ColorMask::A);
			this->stencil_func = CompareFunc::ALWAYS;
			this->stencil_pass_op = StencilOp::KEEP;
			this->stencil_fail_op = StencilOp::KEEP;
			this->stencil_zfail_op = StencilOp::KEEP;
			this->stencil_read_mask = 0xFF;
			this->stencil_write_mask = 0xFF;
			this->stencil_ref_val = 0;
			this->ztest_func = CompareFunc::LESS;
			this->zwrite_mode = ZWrite::ON;
			this->src_factor = BlendFactor::SRC_ALPHA;
			this->dst_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;
			this->blend_op = BlendOp::ADD;
			this->double_face = false;
			this->transparent = false;
			this->cast_shadow = false;
		}

		Material(const Material& other) {
			copy(other);
		}

		~Material() { }

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
		bool double_face;
		bool transparent;
		bool cast_shadow;
		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, Vector4> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, std::shared_ptr<Texture>> name2tex;
		std::unordered_map<property_name, std::shared_ptr<CubeMap>> name2cubemap;
		LightingData lighting_param;
		std::unique_ptr<Shader> target_shader;
		std::unique_ptr<ShadowShader> shadow_caster;

	public:
		static std::unique_ptr<Material> create() {
			return std::make_unique<Material>();
		}

		static std::unique_ptr<Material> create(std::unique_ptr<Shader>& Shader) {
			return std::make_unique<Material>(Shader);
		}

		static std::unique_ptr<Material> create(const Material& other) {
			return std::make_unique<Material>(other);
		}

		Shader* get_shader(const RenderPass& pass) const {
			if (pass == RenderPass::SHADOW) {
				return shadow_caster.get();
			}
			return target_shader.get();
		}

		void set_shadowmap(RawBuffer<float>* shadowmap) {
			this->target_shader->shadowmap = shadowmap;
		}

		void sync(Shader* shader, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p) {
			shader->m = m;
			shader->v = v;
			shader->p = p;
			shader->ztest_func = ztest_func;
			shader->zwrite_mode = zwrite_mode;
			shader->src_factor = src_factor;
			shader->dst_factor = dst_factor;
			shader->blend_op = blend_op;
			shader->transparent = transparent;
			shader->name2float = name2float;
			shader->name2float4 = name2float4;
			shader->name2tex = name2tex;
			shader->name2int = name2int;
			shader->name2cubemap = name2cubemap;
			shader->stencil_func = stencil_func;
			shader->stencil_pass_op = stencil_pass_op;
			shader->stencil_fail_op = stencil_fail_op;
			shader->stencil_zfail_op = stencil_zfail_op;
			shader->stencil_read_mask = stencil_read_mask;
			shader->stencil_write_mask = stencil_write_mask;
			shader->stencil_ref_val = stencil_ref_val;
			shader->color_mask = color_mask;
			shader->lighting_param = lighting_param;
			shader->double_face = double_face;
			if (name2tex.count(normal_prop) > 0) {
				shader->normal_map = true;
			}
			else {
				shader->normal_map = false;
			}
		}

		void sync(const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p) {
			sync(target_shader.get(), m, v, p);
			if (shadow_caster != nullptr) {
				sync(shadow_caster.get(), m, v, p);
			}
		}

		void set_int(const property_name& name, const int& val) {
			name2int[name] = val;
		}

		void set_float4(const property_name& name, const Vector4& val) {
			name2float4[name] = val;
		}

		void set_float(const property_name& name, const float& val) {
			name2float[name] = val;
		}
		
		void set_texture(const property_name& name, std::shared_ptr<Texture> tex) {
			if (tex == nullptr) {
				return;
			}
			name2tex[name] = tex;
		}

		void set_cubemap(const property_name& name, std::shared_ptr<CubeMap> cubemap) {
			if (cubemap == nullptr) {
				return;
			}
			name2cubemap[name] = cubemap;
		}

		int get_int(const property_name& name) const {
			if (name2int.count(name) > 0) {
				return name2int.at(name);
			}
			return 0;
		}

		Vector4 get_float4(const property_name& name) const {
			if (name2float4.count(name) > 0) {
				return name2float4.at(name);
			}
			return 0;
		}

		float get_float(const property_name& name) const {
			if (name2float.count(name) > 0) {
				return name2float.at(name);
			}
			return 0;
		}

		std::shared_ptr<Texture> get_texture(const property_name& name) const {
			if (name2tex.count(name) > 0) {
				return name2tex.at(name);
			}
			return nullptr;
		}

		std::shared_ptr<CubeMap> get_cubemap(const property_name& name) const {
			if (name2cubemap.count(name) > 0) {
				return name2cubemap.at(name);
			}
			return nullptr;
		}

		Material& operator =(const Material& other) {
			copy(other);
			return *this;
		}

		void copy(const Material& other) {
			this->target_shader = std::make_unique<Shader>(Shader(*other.target_shader.get()));
			this->ztest_func = other.ztest_func;
			this->zwrite_mode = other.zwrite_mode;
			this->stencil_func = other.stencil_func;
			this->stencil_pass_op = other.stencil_pass_op;
			this->stencil_fail_op = other.stencil_fail_op;
			this->stencil_zfail_op = other.stencil_zfail_op;
			this->stencil_read_mask = other.stencil_read_mask;
			this->stencil_write_mask = other.stencil_write_mask;
			this->stencil_ref_val = other.stencil_ref_val;
			this->src_factor = other.src_factor;
			this->dst_factor = other.dst_factor;
			this->blend_op = other.blend_op;
			this->double_face = other.double_face;
			this->transparent = other.transparent;
			this->name2float = other.name2float;
			this->name2float4 = other.name2float4;
			this->name2int = other.name2int;
			this->name2tex = other.name2tex;
			this->name2cubemap = other.name2cubemap;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "Material[" << this->id << " Shader: " << this->target_shader << "]";
			return ss.str();
		}
	};
}
#endif
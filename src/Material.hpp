#ifndef _MATERIAL_
#define _MATERIAL_
#include <Guarneri.hpp>

namespace Guarneri {
	class Material : public Object {
	public:
		Material() {
			this->target_shader = std::make_shared<Shader>();
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
			this->skybox = false;
		}

		Material(std::unique_ptr<Shader>& shader) {
			this->target_shader = std::move(shader);
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
			this->skybox = false;
		}

		Material(std::unique_ptr<SkyboxShader>& shader) {
			auto shader_ptr = shader.release();
			this->target_shader = std::shared_ptr<Shader>((Shader*)(shader_ptr));
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
			this->skybox = true;
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
		bool skybox;
		bool transparent;
		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, Vector4> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, std::shared_ptr<Texture>> name2tex;
		std::unordered_map<property_name, std::shared_ptr<CubeMap>> name2cubemap;
		LightingData lighting_param;
		std::shared_ptr<Shader> target_shader;

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

		void sync(const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p) {
			target_shader->m = m;
			target_shader->v = v;
			target_shader->p = p;
			target_shader->ztest_func = ztest_func;
			target_shader->zwrite_mode = zwrite_mode;
			target_shader->src_factor = src_factor;
			target_shader->dst_factor = dst_factor;
			target_shader->blend_op = blend_op;
			target_shader->transparent = transparent;
			target_shader->name2float = name2float;
			target_shader->name2float4 = name2float4;
			target_shader->name2tex = name2tex;
			target_shader->name2int = name2int;
			target_shader->name2cubemap = name2cubemap;
			target_shader->stencil_func = stencil_func;
			target_shader->stencil_pass_op = stencil_pass_op;
			target_shader->stencil_fail_op = stencil_fail_op;
			target_shader->stencil_zfail_op = stencil_zfail_op;
			target_shader->stencil_read_mask = stencil_read_mask;
			target_shader->stencil_write_mask = stencil_write_mask;
			target_shader->stencil_ref_val = stencil_ref_val;
			target_shader->color_mask = color_mask;
			target_shader->lighting_param = lighting_param;
			target_shader->double_face = double_face;
			target_shader->skybox = skybox;
			if (name2tex.count(normal_prop) > 0) {
				target_shader->normal_map = true;
			}
			else {
				target_shader->normal_map = false;
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
			this->target_shader = other.target_shader;
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
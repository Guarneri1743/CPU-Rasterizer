#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	class Material : public Object {
	public:
		Material() {
			this->target_shader = std::make_unique<Shader>("default");
			this->ztest_mode = ztest::less_equal;
			this->zwrite_mode = zwrite::on;
			this->src_factor = blend_factor::src_alpha;
			this->dst_factor = blend_factor::one_minus_src_alpha;
			this->blend_op = blend_operator::add;
			this->transparent = false;
		}

		Material(std::unique_ptr<Shader>& Shader) {
			this->target_shader = std::move(Shader);
			this->ztest_mode = ztest::less_equal;
			this->zwrite_mode = zwrite::on;
			this->src_factor = blend_factor::src_alpha;
			this->dst_factor = blend_factor::one_minus_src_alpha;
			this->blend_op = blend_operator::add;
			this->transparent = false;
		}

		Material(const Material& other) {
			copy(other);
		}

		~Material() { }

	public:
		std::shared_ptr<Shader> target_shader;
		ztest ztest_mode;
		zwrite zwrite_mode;
		blend_factor src_factor;
		blend_factor dst_factor;
		blend_operator blend_op;
		bool transparent;
		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, Vector4> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, std::shared_ptr<Texture>> name2tex;
		lighting_data lighting_param;

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

		void set_shader(std::shared_ptr<Shader> Shader) {
			this->target_shader = Shader;
		}

		std::shared_ptr<Shader> get_shader() {
			return target_shader;
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

		Material& operator =(const Material& other) {
			copy(other);
			return *this;
		}

		void copy(const Material& other) {
			this->target_shader = other.target_shader;
			this->ztest_mode = other.ztest_mode;
			this->zwrite_mode = other.zwrite_mode;
			this->src_factor = other.src_factor;
			this->dst_factor = other.dst_factor;
			this->blend_op = other.blend_op;
			this->transparent = other.transparent;
			this->name2float = other.name2float;
			this->name2float4 = other.name2float4;
			this->name2int = other.name2int;
			this->name2tex = other.name2tex;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "Material[" << this->id << " Shader: " << this->target_shader << "]";
			return ss.str();
		}
	};
}
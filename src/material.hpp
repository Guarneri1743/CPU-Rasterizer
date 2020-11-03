#pragma once
#include <guarneri.hpp>

namespace guarneri {
	class material {
	public:
		material() {
			this->target_shader = shader::default_shader;
			this->ztest_mode = ztest::less_equal;
			this->zwrite_mode = zwrite::on;
			this->src_factor = blend_factor::src_alpha;
			this->dst_factor = blend_factor::one_minus_src_alpha;
			this->blend_op = blend_operator::add;
			this->transparent = false;
		}

		material(const std::shared_ptr<shader>& shader) {
			this->target_shader = shader;
			this->ztest_mode = ztest::less_equal;
			this->zwrite_mode = zwrite::on;
			this->src_factor = blend_factor::src_alpha;
			this->dst_factor = blend_factor::one_minus_src_alpha;
			this->blend_op = blend_operator::add;
			this->transparent = false;
		}

		material(const material& other) {
			deep_copy(other);
		}

		~material() {
			target_shader.reset();
			for (auto& kv : name2tex) {
				kv.second.reset();
			}
		}

	public:
		std::shared_ptr<shader> target_shader;
		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, float4> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, std::shared_ptr<texture>> name2tex;
		ztest ztest_mode;
		zwrite zwrite_mode;
		blend_factor src_factor;
		blend_factor dst_factor;
		blend_operator blend_op;
		bool transparent;

	public:
		static std::unique_ptr<material> create() {
			return std::make_unique<material>();
		}

		static std::unique_ptr<material> create(const std::shared_ptr<shader>& shader) {
			return std::make_unique<material>(shader);
		}

		static std::unique_ptr<material> create(const material& other) {
			return std::make_unique<material>(other);
		}

		void set_shader(std::shared_ptr<shader> shader) {
			this->target_shader = shader;
		}

		std::shared_ptr<shader> get_shader() {
			return target_shader;
		}

		void set_int(const property_name& name, const int& val) {
			name2int[name] = val;
		}

		void set_float4(const property_name& name, const float4& val) {
			name2float4[name] = val;
		}

		void set_float(const property_name& name, const float& val) {
			name2float[name] = val;
		}
		
		void set_texture(const property_name& name, std::shared_ptr<texture> tex) {
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

		float4 get_float4(const property_name& name) const {
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

		std::shared_ptr<texture> get_texture(const property_name& name) const {
			if (name2tex.count(name) > 0) {
				return name2tex.at(name);
			}
			return nullptr;
		}

		void operator =(const material& other) {
			deep_copy(other);
		}

		void deep_copy(const material& other) {
			this->target_shader = other.target_shader;
			this->ztest_mode = other.ztest_mode;
			this->zwrite_mode = other.zwrite_mode;
			this->src_factor = other.src_factor;
			this->dst_factor = other.dst_factor;
			this->blend_op = other.blend_op;
			this->transparent = other.transparent;
		}
	};
}
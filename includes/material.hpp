#pragma once
#include <common.hpp>
#include <shader.hpp>
#include <float4.hpp>
#include <texture.hpp>
#include <unordered_map>
#include <shader_manager.hpp>
#include <texture_manager.hpp>

namespace guarneri {
	class material {
	public:
		material(shader* shader) {
			this->target_shader = shader;
			this->ztest_mode = ztest::less_equal;
			this->zwrite_mode = zwrite::on;
			this->src_factor = blend_factor::src_alpha;
			this->dst_factor = blend_factor::one_minus_src_alpha;
			this->blend_op = blend_operator::add;
			this->transparent = false;
		}

	public:
		shader* target_shader;
		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, float4> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, texture<color>*> name2tex;
		ztest ztest_mode;
		zwrite zwrite_mode;
		blend_factor src_factor;
		blend_factor dst_factor;
		blend_operator blend_op;
		bool transparent;

	public:
		void set_shader(shader* shader) {
			this->target_shader = shader;
		}

		shader* get_shader() {
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
		
		void set_texture(const property_name& name, texture<color>* tex) {
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

		texture<color>* get_texture(const property_name& name) const {
			if (name2tex.count(name) > 0) {
				return name2tex.at(name);
			}
			return nullptr;
		}
	};
}
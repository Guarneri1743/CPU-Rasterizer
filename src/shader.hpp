#pragma once
#include <guarneri.hpp>

namespace guarneri {
	enum class ztest {
		always,
		less,
		less_equal,
		equal,
		greater_equal,
		greater,
		not_equal
	};

	enum class zwrite {
		on,
		off,
		fragment_z,
		early_z
	};

	struct a2v {
		float4 position;
		float2 uv;
		float4 color;
		float3 normal;
	};

	struct v2f {
		float4 position;
		float2 uv;
		float4 color;
		float3 normal;
		float3 tangent;
		float3 bitangent;
		// todo: add userdata feature to vertex
		float3 custom_data;
	};

	class shader {
	public:
		shader(const shader_id& id) {
			this->id = id;
			this->ztest_mode = ztest::less_equal;
			this->zwrite_mode = zwrite::on;
			this->src_factor = blend_factor::src_alpha;
			this->dst_factor = blend_factor::one_minus_src_alpha;
			this->blend_op = blend_operator::add;
			this->transparent = false;
		}

	private:
		mat4 v, p;
		mat4 m;

		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, float4> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, texture*> name2tex;
		std::unordered_map<property_name, std::string> keywords;

	public:
		shader_id id;
		ztest ztest_mode;
		zwrite zwrite_mode;
		blend_factor src_factor;
		blend_factor dst_factor;
		blend_operator blend_op;
		bool transparent;
		static shader default_shader;

	public:
		void sync(ztest ztest, zwrite zwrite) {
			this->ztest_mode = ztest;
			this->zwrite_mode = zwrite;
		}

		void sync(bool semi_trans, blend_factor src, blend_factor dst, blend_operator op) {
			this->transparent = semi_trans;
			this->src_factor = src;
			this->dst_factor = dst;
			this->blend_op = op;
		}

		void sync(
			const std::unordered_map<property_name, float>& float_uniforms,
			const std::unordered_map<property_name, float4>& float4_uniforms,
			const std::unordered_map<property_name, int>& int_uniforms,
			const std::unordered_map<property_name, texture*>& tex_uniforms) {
			this->name2float = float_uniforms;
			this->name2float4 = float4_uniforms;
			this->name2int = int_uniforms;
			this->name2tex = tex_uniforms;
		}

		void set_mvp_matrix(const mat4& model, const mat4& view, const mat4& proj) {
			this->m = model;
			this->v = view;
			this->p = proj;
		}

		v2f vertex_shader(const a2v& input) {
			v2f o;
			o.position = p * v * m * input.position;
			o.color = input.color;
			o.normal = input.normal;
			o.uv = input.uv;
			return o;
		}

		color fragment_shader(const v2f& input) {
			color main_tex;
			if (name2tex[albedo_prop] != nullptr && name2tex[albedo_prop]->sample(input.uv.x, input.uv.y, main_tex)) {
				
			}

			if (name2tex[normal_prop] != nullptr && name2tex[normal_prop]->sample(input.uv.x, input.uv.y, main_tex)) {

			}

			if (name2tex[specular_prop] != nullptr && name2tex[specular_prop]->sample(input.uv.x, input.uv.y, main_tex)) {

			}

			if (name2tex[height_prop] != nullptr && name2tex[height_prop]->sample(input.uv.x, input.uv.y, main_tex)) {

			}

			// fallback
			return color(207.0f / 255.0f, 0.0f, 112.0f / 255.0f);
		}
	};

	shader shader::default_shader("default");
}
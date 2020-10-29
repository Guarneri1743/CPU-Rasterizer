#pragma once
#include <common.hpp>
#include <float2.hpp>
#include <float3.hpp>
#include <float4.hpp>
#include <mat4.hpp>
#include <unordered_map>
#include <texture.hpp>

namespace guarneri {
	enum class ztest {
		always,
		less,
		less_equal,
		equal,
		greater_equal,
		greater,
		not_equal,
		off
	};

	enum class zwrite {
		on,
		off
	};

	struct v_input {
		float4 position;
		float2 uv;
		float4 color;
		float3 normal;
	};

	struct v_output {
		float4 position;
		float2 uv;
		float4 color;
		float3 normal;
		float3 tangent;
		// todo: add userdata feature to vertex
		float3 custom_data;
	};

	class shader {
	public:
		shader(const shader_id& id) {
			this->id = id;
			this->ztest_mode = ztest::less_equal;
			this->zwrite_mode = zwrite::on;
		}

	private:
		mat4 v, p;
		mat4 m;

		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, float4> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, texture<float>*> name2tex;

	public:
		shader_id id;
		ztest ztest_mode;
		zwrite zwrite_mode;

	public:
		void sync_ztest(ztest ztest, zwrite zwrite) {
			this->ztest_mode = ztest;
			this->zwrite_mode = zwrite;
		}

		void sync_uniforms(
			const std::unordered_map<property_name, float>& float_uniforms,
			const std::unordered_map<property_name, float4>& float4_uniforms,
			const std::unordered_map<property_name, int>& int_uniforms,
			const std::unordered_map<property_name, texture<float>*>& tex_uniforms) {
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

		v_output vertex_shader(const v_input& input) {
			v_output o;
			o.position = p * v * m * input.position;
			o.color = input.color;
			o.normal = input.normal;
			o.uv = input.uv;
			return o;
		}

		float4 fragment_shader(const v_output& input) {
			float noise;
			if (name2tex["noise"]->sample(input.uv.x, input.uv.y, noise)) {
				return float4(noise, noise, noise, 1);
			}
			else {
				//sample failed
				return float4(0, 1, 0, 1);
			}

			// return vertex color;
			return float4(input.uv, 0, 1);
		}
	};
}
#pragma once
#include <common.hpp>
#include <float2.hpp>
#include <float3.hpp>
#include <float4.hpp>
#include <mat4.hpp>

namespace guarneri {
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
	};

	struct shader {
	private:
		mat4 v, p;
		mat4 m;

	public:
		void set_vp_matrix(const mat4& view, const mat4& proj) {
			this->v = view;
			this->p = proj;
		}

		void set_model_matrix(const mat4& model) {
			this->m = model;
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
			// return vertex color;
			return input.color;
		}
	};
}
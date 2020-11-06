#pragma once
#include <guarneri.hpp>

namespace guarneri {
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

	class shader : public object{
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

		shader(const shader& other) {
			copy(other);
		}

		~shader() { }

	private:
		mat4 m, v, p;
		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, float4> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, std::shared_ptr<texture>> name2tex;
		std::unordered_map<property_name, std::string> keywords;

	public:
		shader_id id;
		ztest ztest_mode;
		zwrite zwrite_mode;
		blend_factor src_factor;
		blend_factor dst_factor;
		blend_operator blend_op;
		bool transparent;

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
			const std::unordered_map<property_name, std::shared_ptr<texture>>& tex_uniforms) {
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
			auto oo = p * v * m * input.position;
			o.position = oo;
			o.color = input.color;
			o.normal = input.normal;
			o.uv = input.uv;
			return o;
		}

		color fragment_shader(const v2f& input) {
			color main_tex;
			if (name2tex.count(albedo_prop) > 0 && name2tex[albedo_prop]->sample(input.uv.x, input.uv.y, main_tex)) {
				return main_tex;
			}

			if (name2tex.count(normal_prop) > 0 && name2tex[normal_prop]->sample(input.uv.x, input.uv.y, main_tex)) {

			}

			if (name2tex.count(specular_prop) > 0 && name2tex[specular_prop]->sample(input.uv.x, input.uv.y, main_tex)) {

			}

			if (name2tex.count(height_prop) > 0 && name2tex[height_prop]->sample(input.uv.x, input.uv.y, main_tex)) {

			}

			// fallback
			return color(207.0f / 255.0f, 0.0f, 112.0f / 255.0f, 0.3f);
		}

		shader& operator =(const shader& other) {
			copy(other);
			return *this;
		}

		void copy(const shader& other) {
			this->id = other.id;
			this->ztest_mode = other.ztest_mode;
			this->zwrite_mode = other.zwrite_mode;
			this->src_factor = other.src_factor;
			this->dst_factor = other.dst_factor;
			this->blend_op = other.blend_op;
			this->transparent = other.transparent;
			this->name2float = other.name2float;
			this->name2float4 = other.name2float4;
			this->name2tex = other.name2tex;
			this->name2int = other.name2int;
			this->keywords = other.keywords;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "Shader[" << this->id << "]" << std::endl;
			return ss.str();
		}
	};
}
#ifndef _SHADER_
#define _SHADER_
#include <Guarneri.hpp>

namespace Guarneri {
	struct a2v {
		Vector4 position;
		Vector2 uv;
		Vector4 color;
		Vector3 normal;
	};

	struct v2f {
		Vector4 position;
		Vector3 world_pos;
		Vector2 uv;
		Vector4 color;
		Vector3 normal;
		Vector3 tangent;
		Vector3 bitangent;
		Vector4 shadow_coord;
	};

	class Shader : public Object{
	public:
		Shader() {
			this->ztest_mode = ZTest::LEQUAL;
			this->zwrite_mode = ZWrite::ON;
			this->src_factor = BlendFactor::SRC_ALPHA;
			this->dst_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;
			this->blend_op = BlendOp::ADD;
			this->transparent = false;
		}

		Shader(const Shader& other) {
			copy(other);
		}

		virtual ~Shader() { }

	protected:
		Matrix4x4 m, v, p;
		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, Vector4> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, std::shared_ptr<Texture>> name2tex;
		std::unordered_map<property_name, std::string> keywords;

	public:
		ZTest ztest_mode;
		ZWrite zwrite_mode;
		BlendFactor src_factor;
		BlendFactor dst_factor;
		BlendOp blend_op;
		bool transparent;
		LightingData lighting_param;

	public:
		void sync(ZTest ztest, ZWrite zwrite) {
			this->ztest_mode = ztest;
			this->zwrite_mode = zwrite;
		}

		void sync(bool semi_trans, BlendFactor src, BlendFactor dst, BlendOp op) {
			this->transparent = semi_trans;
			this->src_factor = src;
			this->dst_factor = dst;
			this->blend_op = op;
		}

		void sync(
			const std::unordered_map<property_name, float>& float_uniforms,
			const std::unordered_map<property_name, Vector4>& float4_uniforms,
			const std::unordered_map<property_name, int>& int_uniforms,
			const std::unordered_map<property_name, std::shared_ptr<Texture>>& tex_uniforms) {
			this->name2float = float_uniforms;
			this->name2float4 = float4_uniforms;
			this->name2int = int_uniforms;
			this->name2tex = tex_uniforms;
		}

		void sync(const LightingData& data) {
			this->lighting_param = data;
		}

		void set_mvp_matrix(const Matrix4x4& model, const Matrix4x4& view, const Matrix4x4& proj) {
			this->m = model;
			this->v = view;
			this->p = proj;
		}

		virtual v2f vertex_shader(const a2v& input) {
			v2f o;
			auto oo = p * v * m * Vector4(input.position.x, input.position.y, input.position.z, 1.0f);
			o.position = oo;
			o.world_pos = (m * input.position).xyz();
			o.color = input.color;
			o.normal = Matrix3x3(m).inverse().transpose() * input.normal;
			o.uv = input.uv;
			return o;
		}

		virtual Color fragment_shader(const v2f& input) {
			Color ambient = misc_param.main_light.ambient;
			Color specular = misc_param.main_light.specular;
			Color diffuse = misc_param.main_light.diffuse;
			float intensity = misc_param.main_light.intensity;
			REF(intensity);

			Vector3 light_dir = misc_param.main_light.direction.normalized();
			Vector3 cam_pos = misc_param.camera_pos;
			Vector3 frag_pos = input.world_pos;
			Vector4 screen_pos = input.position;
			float glossiness = std::clamp(lighting_param.glossiness, 0.0f, 256.0f);

			Vector3 normal = input.normal.normalized();
			float ndl = std::max(Vector3::dot(normal, light_dir), 0.0f);
			Vector3 view_dir = (cam_pos - frag_pos).normalized();
			Vector3 reflect_dir = light_dir - 2.0f * normal * ndl;
			float spec = std::pow(std::max(Vector3::dot(view_dir, reflect_dir), 0.0f), glossiness);

			Color ret = ambient;
			Color main_tex;
			if (name2tex.count(albedo_prop) > 0 && name2tex[albedo_prop]->sample(input.uv.x, input.uv.y, main_tex)) {
				ret += diffuse * ndl * main_tex;
			}

			Color normal_tex;
			if (name2tex.count(normal_prop) > 0 && name2tex[normal_prop]->sample(input.uv.x, input.uv.y, normal_tex)) {

			}

			Color spec_tex;
			if (name2tex.count(specular_prop) > 0 && name2tex[specular_prop]->sample(input.uv.x, input.uv.y, spec_tex)) {
				ret += specular * spec * spec_tex;
			}

			Color height_tex;
			if (name2tex.count(height_prop) > 0 && name2tex[height_prop]->sample(input.uv.x, input.uv.y, height_tex)) {

			}

			if (((int)misc_param.flag & (int)RenderFlag::UV) != 0) {
				return input.uv;
			}

			if (((int)misc_param.flag & (int)RenderFlag::VERTEX_COLOR) != 0) {
				return input.color;
			}

			if (((int)misc_param.flag & (int)RenderFlag::NORMAL) != 0) {
				return input.normal;
			}

			return Color(ret.r, ret.g, ret.b, 1.0f);
		}

		Shader& operator =(const Shader& other) {
			copy(other);
			return *this;
		}

		void copy(const Shader& other) {
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
#endif
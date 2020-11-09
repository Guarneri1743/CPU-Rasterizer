#ifndef _SHADER_
#define _SHADER_
#include <Guarneri.hpp>

namespace Guarneri {
	struct a2v {
		Vector4 position;
		Vector2 uv;
		Vector4 color;
		Vector3 normal;
		Vector3 tangent;
	};

	struct v2f {
		Vector4 position;
		Vector3 world_pos;
		Vector2 uv;
		Vector4 color;
		Vector3 tangent;
		Vector3 bitangent;
		Vector3 normal;
		Vector4 shadow_coord;
	};

	struct LightingData {
		LightingData() {
			glossiness = 32.0f;
		}

		float glossiness;
	};

	class Shader : public Object{
	public:
		Shader() {
			this->color_mask = (ColorMask::R | ColorMask::G | ColorMask::B | ColorMask::A);
			this->stencil_func = CompareFunc::ALWAYS;
			this->stencil_pass_op = StencilOp::KEEP;
			this->stencil_fail_op = StencilOp::KEEP;
			this->stencil_zfail_op = StencilOp::KEEP;
			this->stencil_read_mask = 0xFF;
			this->stencil_write_mask = 0xFF;
			this->stencil_ref_val = 0;
			this->ztest_func = CompareFunc::LEQUAL;
			this->zwrite_mode = ZWrite::ON;
			this->src_factor = BlendFactor::SRC_ALPHA;
			this->dst_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;
			this->blend_op = BlendOp::ADD;
			this->lighting_param = LightingData();
			this->transparent = false;
		}

		Shader(const Shader& other) {
			copy(other);
		}

		virtual ~Shader() { }

	public:		
		Matrix4x4 m, v, p;
		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, Vector4> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, std::shared_ptr<Texture>> name2tex;
		std::unordered_map<property_name, std::shared_ptr<CubeMap>> name2cubemap;
		std::unordered_map<property_name, std::string> keywords;
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
		bool transparent;
		LightingData lighting_param;
		bool discarded = false;
		bool normal_map = false;

	public:
		void discard() {
			discarded = true;
		}

		virtual v2f vertex_shader(const a2v& input) {
			v2f o;
			auto oo = p * v * m * Vector4(input.position.x, input.position.y, input.position.z, 1.0f);
			o.position = oo;
			o.world_pos = (m * input.position).xyz();
			o.shadow_coord = Vector4(input.uv, 0.0f, 1.0f);
			o.color = input.color;
			Matrix3x3 normal_matrix = Matrix3x3(m).inverse().transpose();
			if (normal_map) {
				Vector3 t = (normal_matrix * input.tangent).normalized();
				Vector3 n = (normal_matrix * input.normal).normalized();
				t = (t - Vector3::dot(t, n) * n).normalized();
				Vector3 b = Vector3::cross(n, t);
				o.tangent = t;
				o.bitangent = b;
				o.normal = n;
			}
			else {
				o.normal = normal_matrix * input.normal;
			}
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
			float ndl = 0.0f;
			Vector3 view_dir = (cam_pos - frag_pos).normalized();
		
			Color normal_tex;
			float spec;
			if (name2tex.count(normal_prop) > 0 && name2tex[normal_prop]->sample(input.uv.x, input.uv.y, normal_tex)) {
				Matrix3x3 tbn = Matrix3x3(input.tangent, input.bitangent, input.normal);
				light_dir = tbn * light_dir;
				view_dir = tbn * view_dir;
				auto packed_normal = Vector3(normal_tex.r, normal_tex.g, normal_tex.b);
				normal = (packed_normal * 2.0f - 1.0f).normalized();
			}

			ndl = std::max(Vector3::dot(normal, light_dir), 0.0f);
			Vector3 reflect_dir = 2.0f * normal * ndl - light_dir;
			Vector3 half_dir = (light_dir + view_dir).normalized();
			
			Color ret = ambient;
			Color main_tex;
			if (name2tex.count(albedo_prop) > 0 && name2tex[albedo_prop]->sample(input.uv.x, input.uv.y, main_tex)) {
				ret += Color::saturate(diffuse * ndl * main_tex);
			}

			Color spec_tex;
			if (name2tex.count(specular_prop) > 0 && name2tex[specular_prop]->sample(input.uv.x, input.uv.y, spec_tex)) {
				spec = std::pow(std::max(Vector3::dot(normal, half_dir), 0.0f), glossiness), 0.0f, 1.0f;
				ret += Color::saturate(specular * spec * spec_tex);
			}

			Color ao;
			if (name2tex.count(ao_prop) >0&&name2tex[ao_prop]->sample(input.uv.x, input.uv.y, ao)) {
				ret *= ao;
			}

			if ((misc_param.render_flag & RenderFlag::UV) != RenderFlag::DISABLE) {
				return input.uv;
			}

			if ((misc_param.render_flag & RenderFlag::VERTEX_COLOR) != RenderFlag::DISABLE) {
				return input.color;
			}

			if ((misc_param.render_flag & RenderFlag::NORMAL) != RenderFlag::DISABLE) {
				return spec;
			}
			
			ret = Color::saturate(ret);
			return Color(ret.r, ret.g, ret.b, 1.0f);
		}

		Shader& operator =(const Shader& other) {
			copy(other);
			return *this;
		}

		void copy(const Shader& other) {
			this->id = other.id;
			this->ztest_func = other.ztest_func;
			this->zwrite_mode = other.zwrite_mode;
			this->src_factor = other.src_factor;
			this->dst_factor = other.dst_factor;
			this->blend_op = other.blend_op;
			this->transparent = other.transparent;
			this->name2float = other.name2float;
			this->name2float4 = other.name2float4;
			this->name2tex = other.name2tex;
			this->name2cubemap = other.name2cubemap;
			this->name2int = other.name2int;
			this->keywords = other.keywords;
			this->stencil_func = other.stencil_func;
			this->stencil_pass_op = other.stencil_pass_op;
			this->stencil_fail_op = other.stencil_fail_op;
			this->stencil_zfail_op = other.stencil_zfail_op;
			this->stencil_read_mask = other.stencil_read_mask;
			this->stencil_write_mask = other.stencil_write_mask;
			this->stencil_ref_val = other.stencil_ref_val;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "Shader[" << this->id << "]" << std::endl;
			return ss.str();
		}
	};
}
#endif
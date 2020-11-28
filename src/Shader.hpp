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
			this->double_face = false;
			this->skybox = false;
			this->shadow = false;
			this->shadowmap = nullptr;
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
		RawBuffer<float>* shadowmap;
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
		bool double_face;
		bool skybox;
		bool shadow;
		LightingData lighting_param;
		bool discarded = false;
		bool normal_map = false;

	public:
		virtual v2f vertex_shader(const a2v& input) const {
			v2f o;
			auto opos = Vector4(input.position.xyz(), 1.0f);
			auto wpos = m * opos;
			auto cpos = p * v * wpos;
			auto light_space_pos = misc_param.main_light.light_space() * wpos;
			o.position = cpos;
			o.world_pos = wpos.xyz();
			o.shadow_coord = light_space_pos;
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

		float get_shadow_atten(const Vector4& light_space_pos) const {
			if (shadowmap == nullptr) {
				return 0.0f;
			}
			Vector3 proj_shadow_coord = light_space_pos.xyz();
			proj_shadow_coord = proj_shadow_coord * 0.5f + 0.5f;
			float depth;
			if (shadowmap->read(proj_shadow_coord.x, proj_shadow_coord.y, depth)) {
				//printf("shadowmap: %f depth: %f\n", depth, proj_shadow_coord.z);
				float shadow_atten = (proj_shadow_coord.z) > depth ? 1.0f : 0.0f;
				return shadow_atten * 0.5f;
			}
			return 0.0f;
		}

		virtual Color fragment_shader(const v2f& input) const {
			Color ambient = misc_param.main_light.ambient;
			Color specular = misc_param.main_light.specular;
			Color diffuse = misc_param.main_light.diffuse;
			float intensity = misc_param.main_light.intensity;
			REF(intensity);

			Vector3 cam_pos = misc_param.camera_pos;
			Vector3 frag_pos = input.world_pos;
			Vector4 screen_pos = input.position;

			Vector3 light_dir = misc_param.main_light.forward.normalized();

			float glossiness = std::clamp(lighting_param.glossiness, 0.0f, 256.0f);

			Vector3 normal = input.normal.normalized();
			float ndl = 0.0f;
			Vector3 view_dir = (cam_pos - frag_pos).normalized();
		
			Color normal_tex;
			float spec;
			if (name2tex.count(normal_prop) > 0 && name2tex.at(normal_prop)->sample(input.uv.x, input.uv.y, normal_tex)) {
				Matrix3x3 tbn = Matrix3x3(input.tangent, input.bitangent, input.normal);
				light_dir = tbn * light_dir;
				view_dir = tbn * view_dir;
				auto packed_normal = Vector3(normal_tex.r, normal_tex.g, normal_tex.b);
				normal = (packed_normal * 2.0f - 1.0f).normalized();
			}

			ndl = std::max(Vector3::dot(normal, light_dir), 0.0f);
			Vector3 reflect_dir = 2.0f * normal * ndl - light_dir;
			Vector3 half_dir = (light_dir + view_dir).normalized();
			spec = std::pow(std::max(Vector3::dot(normal, half_dir), 0.0f), glossiness), 0.0f, 1.0f;

			//shadow
			float shadow_atten = 1.0 - get_shadow_atten(input.shadow_coord);

			Color ret = ambient;
			Color main_tex;
			if (name2tex.count(albedo_prop) > 0 && name2tex.at(albedo_prop)->sample(input.uv.x, input.uv.y, main_tex)) {
				ret += Color::saturate(diffuse * ndl * main_tex) * shadow_atten;
			}

			Color spec_tex;
			if (name2tex.count(specular_prop) > 0 && name2tex.at(specular_prop)->sample(input.uv.x, input.uv.y, spec_tex)) {
				ret += Color::saturate(specular * spec * spec_tex) * shadow_atten;
			}
			else {
				ret += Color::saturate(specular * spec) * shadow_atten;
			}

			Color ao;
			if (name2tex.count(ao_prop) >0&&name2tex.at(ao_prop)->sample(input.uv.x, input.uv.y, ao)) {
				ret *= ao * shadow_atten;
			}

			// todo: ddx ddy
			/*if ((misc_param.render_flag & RenderFlag::MIPMAP) != RenderFlag::DISABLE) {
				Vector2 ddx_uv = ddx.uv;
				Vector2 ddy_uv = ddy.uv;
				float rho = std::max(std::sqrt(Vector2::dot(ddx_uv, ddx_uv)), std::sqrt(Vector2::dot(ddy_uv, ddy_uv)));
				float logrho = std::log2(rho);
				int mip = std::max(int(logrho + 0.5f), 0);
				if (mip == 0) {
					return Color(1.0f, 0.0f, 0.0f, 1.0f);
				}
				else if (mip == 1) {
					return Color(0.0f, 1.0f, 0.0f, 1.0f);
				}
				else if (mip == 2) {
					return Color(0.0f, 0.0f, 1.0f, 1.0f);
				}
				else if (mip == 3) {
					return Color(1.0f, 0.0f, 1.0f, 1.0f);
				}
				else if (mip == 4) {
					return Color(0.0f, 1.0f, 1.0f, 1.0f);
				}
				else if (mip == 5) {
					return Color(1.0f, 1.0f, 1.0f, 1.0f);
				}
				else if (mip == 6) {
					return Color(0.5f, 0.0f, 0.5f, 1.0f);
				}
				else if (mip == 7) {
					return Color(0.0f, 0.5f, 0.5f, 1.0f);
				}
				else {
					return Color(0.5f, 0.5f, 0.5f, 1.0f);
				}
 			}*/

			if ((misc_param.render_flag & RenderFlag::UV) != RenderFlag::DISABLE) {
				return input.uv;
			}

			if ((misc_param.render_flag & RenderFlag::VERTEX_COLOR) != RenderFlag::DISABLE) {
				return input.color;
			}

			if ((misc_param.render_flag & RenderFlag::SPECULAR) != RenderFlag::DISABLE) {
				return spec;
			}

			if ((misc_param.render_flag & RenderFlag::NORMAL) != RenderFlag::DISABLE) {
				return  input.normal.normalized();
			}

			ret = Color::saturate(ret);
			return Color(ret.r, ret.g, ret.b, 1.0f);
		}

		std::string str() const {
			std::stringstream ss;
			ss << "Shader[" << this->id << "]" << std::endl;
			return ss.str();
		}
	};
}
#endif
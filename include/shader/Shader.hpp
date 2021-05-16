#ifndef _SHADER_
#define _SHADER_
#include <unordered_map>
#include "Object.hpp"
#include "Light.hpp"
#include "Misc.hpp"
#include "Color.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Matrix3x3.hpp"
#include "Matrix4x4.hpp"
#include "Texture.hpp"
#include "CubeMap.hpp"

namespace Guarneri
{
	struct a2v
	{
		Vector4 position;
		Vector2 uv;
		Vector4 color;
		Vector3 normal;
		Vector3 tangent;
	};

	struct v2f
	{
		Vector4 position;
		Vector3 world_pos;
		Vector2 uv;
		Vector4 color;
		Vector3 tangent;
		Vector3 bitangent;
		Vector3 normal;
		Vector4 shadow_coord;
	};

	struct LightingData
	{
		Vector3 f0;
		float roughness;
		float metallic;
		float glossiness;

		LightingData()
		{
			glossiness = 32.0f;
			f0 = Vector3(0.04f);
			metallic = 0.0f;
			roughness = 0.5f;
		}
	};

	class Shader : public Object
	{
	public:
		Matrix4x4 model, view, projection;
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
		std::string name;

	public:
		Shader(std::string name);
		virtual ~Shader();
		virtual v2f vertex_shader(const a2v& input) const;
		float get_shadow_atten(const Vector4& light_space_pos) const;
		Vector3 reflect(const Vector3& n, const Vector3& light_out_dir) const;
		Color calculate_main_light(const DirectionalLight& light, const LightingData& lighting_data, const Vector3& wpos, const Vector3& v, const Vector3& n, Color albedo, Color ao, const Vector2& uv, const Matrix3x3& tbn) const;
		Color calculate_point_light(const PointLight& light, const LightingData& lighting_data, const Vector3& wpos, const Vector3& v, const Vector3& n, Color albedo, Color ao, const Vector2& uv, const Matrix3x3& tbn) const;
		virtual Color fragment_shader(const v2f& input) const;
		std::string str() const;
	};
}
#endif
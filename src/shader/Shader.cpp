#include "Shader.hpp"
#include <algorithm>
#include <iostream>
#include "Marcos.h"
#include "Singleton.hpp"
#include "PBR.hpp"

namespace Guarneri
{
	constexpr Color kErrorColor = Color(199.0f, 0.0f, 106.0f, 1.0f);

	Shader* Shader::error_shader = new Shader("error_shader", true);

	Shader::Shader(std::string name) : Shader(name, false) {}

	Shader::Shader(std::string name, bool is_error_shader)
	{
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
		this->name = name;
		this->is_error_shader = is_error_shader;
	}

	Shader::~Shader()
	{}

	v2f Shader::vertex_shader(const a2v& input) const
	{
		v2f o;
		auto opos = Vector4(input.position.xyz(), 1.0f);
		auto wpos = model * opos;
		auto cpos = vp_matrix * wpos;
		o.position = cpos;
		o.world_pos = wpos.xyz();
		if (!is_error_shader)
		{
			auto light_space_pos = INST(GlobalShaderParams).main_light.light_space() * Vector4(wpos.xyz(), 1.0f);
			o.shadow_coord = light_space_pos;
		}
		o.color = input.color;
		Matrix3x3 normal_matrix = Matrix3x3(model).inverse().transpose();
		if (normal_map && !is_error_shader)
		{
			Vector3 t = (normal_matrix * input.tangent).normalized();
			Vector3 n = (normal_matrix * input.normal).normalized();
			t = (t - Vector3::dot(t, n) * n).normalized();
			Vector3 b = Vector3::cross(n, t);
			o.tangent = t;
			o.bitangent = b;
			o.normal = n;
		}
		else
		{
			o.normal = normal_matrix * input.normal;
		}
		o.uv = input.uv;
		return o;
	}

	float Shader::get_shadow_atten(const Vector4& light_space_pos) const
	{
		if (shadowmap == nullptr)
		{
			return 0.0f;
		}

		Vector3 proj_shadow_coord = light_space_pos.xyz();
		proj_shadow_coord = proj_shadow_coord * 0.5f + 0.5f;

		if (proj_shadow_coord.z > 1.0f)
		{
			return 0.0f;
		}

		float shadow_atten = 0.0f;

		Vector2 texel_size = 1.0f / Vector2(shadowmap->height, shadowmap->width);

		if (INST(GlobalShaderParams).pcf_on)
		{
			const int kernel_size = 3;
			// PCF
			for (int x = -kernel_size; x <= kernel_size; x++)
			{
				for (int y = -kernel_size; y <= kernel_size; y++)
				{
					float depth;
					if (shadowmap->read(proj_shadow_coord.x + (float)x * texel_size.x, proj_shadow_coord.y + (float)y * texel_size.y, depth))
					{
						//printf("shadowmap: %f depth: %f\n", depth, proj_shadow_coord.z);
						shadow_atten += (proj_shadow_coord.z - INST(GlobalShaderParams).shadow_bias) > depth ? 1.0f : 0.0f;
					}
				}
			}

			shadow_atten /= (float)((kernel_size + 1) * (kernel_size + 1) * 4);
		}
		else
		{
			float depth;
			if (shadowmap->read(proj_shadow_coord.x, proj_shadow_coord.y, depth))
			{
				//printf("shadowmap: %f depth: %f\n", depth, proj_shadow_coord.z);
				shadow_atten = (proj_shadow_coord.z - INST(GlobalShaderParams).shadow_bias) > depth ? 1.0f : 0.0f;
			}
		}

		return shadow_atten * 0.8f;
	}

	Vector3 Shader::reflect(const Vector3& n, const Vector3& light_out_dir) const
	{
		auto ndl = Vector3::dot(n, light_out_dir);
		return (2.0f * n * ndl - light_out_dir).normalized();
	}

	Vector3 metallic_workflow(const Vector3& f0, const Vector3& albedo, const float& intensity, const float& metallic, const float& roughness, const float& light_distance, const Vector3& halfway, const Vector3& light_dir, const Vector3& view_dir, const Vector3& normal)
	{
		float attenuation = 1.0f / (light_distance * light_distance);
		Vector3 radiance = Vector3(attenuation * intensity);

		float ndf = distribution_ggx(normal, halfway, roughness);
		float geo = geometry_smith(normal, view_dir, light_dir, roughness);
		Vector3 fresnel = fresnel_schlick(std::max(Vector3::dot(halfway, view_dir), 0.0f), f0);

		Vector3 nominator = ndf * geo * fresnel;
		float ndv = std::max(Vector3::dot(normal, view_dir), 0.0f);
		float ndl = std::max(Vector3::dot(normal, light_dir), 0.0f);
		float denominator = 4.0f * ndv * ndl;
		Vector3 specular = nominator / std::max(denominator, 0.001f);

		Vector3 spec_term = fresnel;
		Vector3 diffuse_term = Vector3(1.0f) - spec_term;
		diffuse_term *= 1.0f - metallic;

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::SPECULAR) != RenderFlag::DISABLE)
		{
			return specular;
		}

		return ((diffuse_term * albedo) / PI + specular) * radiance * ndl;
	}

	Color Shader::calculate_main_light(const DirectionalLight& light, const LightingData& lighting_data, const Vector3& wpos, const Vector3& view_dir, const Vector3& normal, Color albedo, Color ao, const Vector2& uv, const Matrix3x3& tbn) const
	{
		UNUSED(wpos);
		UNUSED(lighting_data);
		auto light_ambient = light.ambient;
		auto light_spec = light.specular;
		auto light_diffuse = light.diffuse;
		auto intensity = light.intensity;
		UNUSED(intensity);
		light_diffuse *= intensity;
		light_spec *= intensity;
		float light_distance = 1.0f;

		auto light_dir = -light.forward.normalized();
		if (this->normal_map)
		{
			light_dir = tbn * light_dir;
		}

		auto half_dir = (light_dir + view_dir).normalized();

		// sample textures
		Color metallic_color = Color::BLACK;
		float metallic = 0.0f;
		name2tex.count(metallic_prop) > 0 && name2tex.at(metallic_prop)->sample(uv.x, uv.y, metallic_color);
		metallic = metallic_color.r;
		Color roughness_color;
		float roughness = 0.0f;
		if (name2tex.count(roughness_prop) > 0)
		{
			name2tex.at(roughness_prop)->sample(uv.x, uv.y, roughness_color);
			roughness = roughness_color.r;
		}
		else if (name2tex.count(specular_prop))
		{
			Color spec;
			name2tex.at(specular_prop)->sample(uv.x, uv.y, spec);
			roughness = 1.0f - spec.r;
		}

		roughness = std::max(roughness, EPSILON); // brdf lut bug (baking error)

		if (name2float.count(roughness_multiplier_prop) > 0 && name2float.count(roughness_offset_prop) > 0 )
		{
			roughness = roughness * name2float.at(roughness_multiplier_prop) + name2float.at(roughness_offset_prop);
		}
		
		if (name2float.count(metallic_multiplier_prop) > 0 && name2float.count(metallic_offset_prop) > 0)
		{
			metallic = metallic * name2float.at(metallic_multiplier_prop) + name2float.at(metallic_offset_prop);
		}

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::ROUGHNESS) != RenderFlag::DISABLE)
		{
			return Color(roughness);
		}

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::METALLIC) != RenderFlag::DISABLE)
		{
			return Color(metallic_color);
		}

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::AO) != RenderFlag::DISABLE)
		{
			return ao;
		}

		if (INST(GlobalShaderParams).workflow == PBRWorkFlow::Specular)
		{
			//todo
			auto spec = std::pow(std::max(Vector3::dot(normal, half_dir), 0.0f), (roughness) * 32.0f);
			auto ndl = std::max(Vector3::dot(normal, light_dir), 0.0f);

			auto diffuse = Color::saturate(light_diffuse * ndl * albedo);
			Color spec_tex = Color::WHITE;
			name2tex.count(specular_prop) > 0 && name2tex.at(specular_prop)->sample(uv.x, uv.y, spec_tex);
			auto specular = Color::saturate(light_spec * spec * spec_tex);
			auto ambient = light_ambient;
			auto ret = ambient + diffuse + specular;
			return ret;
		}
		else
		{
			Vector3 f0 = 0.04f;
			f0 = Vector3::lerp(f0, Vector3(albedo.r, albedo.g, albedo.b), metallic);
			auto lo = metallic_workflow(f0, Vector3(albedo.r, albedo.g, albedo.b), intensity, metallic, roughness, light_distance, half_dir, light_dir, view_dir, normal);

			Vector3 fresnel = fresnel_schlick_roughness(std::max(Vector3::dot(normal, view_dir), 0.0f), f0, roughness);

			Vector3 specular_term = fresnel;
			Vector3 diffuse_term = 1.0f - specular_term;
			diffuse_term *= 1.0f - metallic;

			// IBL
			Color irradiance;
			if (INST(GlobalShaderParams).enable_skybox)
			{
				name2cubemap.count(cubemap_prop) > 0 && name2cubemap.at(cubemap_prop)->sample_irradiance_map(normal, irradiance);
			}

			Color prefiltered_color;
			if (INST(GlobalShaderParams).enable_skybox)
			{
				auto reflect_dir = reflect(normal, view_dir);
				name2cubemap.count(cubemap_prop) > 0 && name2cubemap.at(cubemap_prop)->sample_prefilter_map(reflect_dir, prefiltered_color);
			}

			Color brdf_lut;
			if (INST(GlobalShaderParams).enable_skybox)
			{
				float ndv = std::max(Vector3::dot(normal, view_dir), 0.0f);
				name2cubemap.count(cubemap_prop) > 0 && name2cubemap.at(cubemap_prop)->sample_brdf(Vector2(ndv, roughness), brdf_lut);
			}

			Vector3 indirect_diffuse = Vector3(irradiance.r, irradiance.g, irradiance.b) * Vector3(albedo.r, albedo.g, albedo.b) * diffuse_term;
			Vector3 indirect_specular = Vector3(prefiltered_color.r, prefiltered_color.g, prefiltered_color.b) * (specular_term * brdf_lut.r + brdf_lut.g);

			if ((INST(GlobalShaderParams).render_flag & RenderFlag::INDIRECT_DIFFUSE) != RenderFlag::DISABLE)
			{
				return Color(indirect_diffuse);
			}

			if ((INST(GlobalShaderParams).render_flag & RenderFlag::INDIRECT_SPECULAR) != RenderFlag::DISABLE)
			{
				return Color(indirect_specular);
			}

			auto ambient = (indirect_diffuse + indirect_specular) * ao.r;
			auto ret = Color(ambient) + lo;

			return ret;
		}
	}

	Color Shader::calculate_point_light(const PointLight& light, const LightingData& lighting_data, const Vector3& wpos, const Vector3& view_dir, const Vector3& normal, Color albedo, Color ao, const Vector2& uv, const Matrix3x3& tbn) const
	{
		// TODO
		return Color::BLACK;
	}

	Color Shader::fragment_shader(const v2f& input) const
	{
		if (is_error_shader) { return kErrorColor; }

		auto main_light = INST(GlobalShaderParams).main_light;
		auto point_lights = INST(GlobalShaderParams).point_lights;

		Vector3 cam_pos = INST(GlobalShaderParams).camera_pos;
		Vector3 wpos = input.world_pos;
		Vector4 screen_pos = input.position;

		Vector3 normal = input.normal.normalized();
		Vector3 view_dir = (cam_pos - wpos).normalized();

		Color normal_tex;
		Matrix3x3 tbn;
		if (name2tex.count(normal_prop) > 0 && name2tex.at(normal_prop)->sample(input.uv.x, input.uv.y, normal_tex))
		{
			tbn = Matrix3x3(input.tangent, input.bitangent, input.normal);
			view_dir = tbn * view_dir;
			auto packed_normal = Vector3(normal_tex.r, normal_tex.g, normal_tex.b);
			normal = (packed_normal * 2.0f - 1.0f).normalized();
		}

		//shadow
		float shadow_atten = 1.0f - get_shadow_atten(input.shadow_coord);

		Color ret = Color::BLACK;
		Color albedo = Color::WHITE;
		if (name2tex.count(albedo_prop) > 0 && name2tex.at(albedo_prop)->sample(input.uv.x, input.uv.y, albedo))
		{
			if (INST(GlobalShaderParams).color_space == ColorSpace::Linear)
			{
				albedo = Color::pow(albedo, 2.2f);
			}
		}

		if (name2float4.count(tint_color_prop) > 0)
		{
			albedo *= Color(name2float4.at(tint_color_prop));
		}

		Color ao = Color::WHITE;
		name2tex.count(ao_prop) > 0 && name2tex.at(ao_prop)->sample(input.uv.x, input.uv.y, ao);

		Color emmision = Color::BLACK;
		name2tex.count(emission_prop) > 0 && name2tex.at(emission_prop)->sample(input.uv.x, input.uv.y, emmision);

		ret += calculate_main_light(main_light, lighting_param, wpos, view_dir, normal, albedo, ao, input.uv, tbn);

		for (auto& light : point_lights)
		{
			ret += calculate_point_light(light, lighting_param, wpos, view_dir, normal, albedo, ao, input.uv, tbn);
		}

		ret += emmision;
		ret *= shadow_atten;

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::SPECULAR) != RenderFlag::DISABLE)
		{
			return Color(ao.r, ao.r, ao.r, 1.0f);
		}

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::UV) != RenderFlag::DISABLE)
		{
			return input.uv;
		}

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::VERTEX_COLOR) != RenderFlag::DISABLE)
		{
			return input.color;
		}

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::NORMAL) != RenderFlag::DISABLE)
		{
			return normal;
		}

		if (INST(GlobalShaderParams).color_space == ColorSpace::Linear)
		{
			ret = ret / (ret + Color::WHITE);
			ret = Color::pow(ret, 1.0f / 2.2f);
		}

		ret = Color::saturate(ret);

		return Color(ret.r, ret.g, ret.b, 1.0f);
	}

	std::string Shader::str() const
	{
		std::stringstream ss;
		ss << "Shader[" << this->id << "]" << std::endl;
		return ss.str();
	}
}
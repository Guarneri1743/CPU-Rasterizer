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
		auto opos = tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
		auto wpos = model * opos;
		auto cpos = vp_matrix * wpos;
		o.position = cpos;
		o.world_pos = wpos.xyz;
		if (!is_error_shader)
		{
			auto light_space_pos = INST(GlobalShaderParams).main_light.light_space() * tinymath::vec4f(wpos.x, wpos.y, wpos.z, 1.0f);
			o.shadow_coord = light_space_pos;
		}
		o.color = input.color;
		tinymath::mat3x3 normal_matrix = tinymath::transpose(tinymath::inverse(tinymath::mat4x4_to_mat3x3(model)));
		if (normal_map && !is_error_shader)
		{
			tinymath::vec3f t = tinymath::normalize(normal_matrix * input.tangent);
			tinymath::vec3f n = tinymath::normalize(normal_matrix * input.normal);
			t = tinymath::normalize(t - tinymath::dot(t, n) * n);
			tinymath::vec3f b = tinymath::cross(n, t);
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

	float Shader::get_shadow_atten(const tinymath::vec4f& light_space_pos) const
	{
		if (shadowmap == nullptr)
		{
			return 0.0f;
		}

		tinymath::vec3f proj_shadow_coord = light_space_pos.xyz;
		proj_shadow_coord = proj_shadow_coord * 0.5f + 0.5f;

		if (proj_shadow_coord.z > 1.0f)
		{
			return 0.0f;
		}

		float shadow_atten = 0.0f;

		tinymath::vec2f texel_size = 1.0f / tinymath::vec2f((float)shadowmap->height, (float)shadowmap->width);

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

	tinymath::vec3f Shader::reflect(const tinymath::vec3f& n, const tinymath::vec3f& light_out_dir) const
	{
		auto ndl = tinymath::dot(n, light_out_dir);
		return tinymath::normalize(2.0f * n * ndl - light_out_dir);
	}

	tinymath::vec3f metallic_workflow(const tinymath::vec3f& f0, const tinymath::vec3f& albedo, const float& intensity, const float& metallic, const float& roughness, const float& light_distance, const tinymath::vec3f& halfway, const tinymath::vec3f& light_dir, const tinymath::vec3f& view_dir, const tinymath::vec3f& normal)
	{
		float attenuation = 1.0f / (light_distance * light_distance);
		tinymath::vec3f radiance = tinymath::vec3f(attenuation * intensity);

		float ndf = distribution_ggx(normal, halfway, roughness);
		float geo = geometry_smith(normal, view_dir, light_dir, roughness);
		tinymath::vec3f fresnel = fresnel_schlick(tinymath::max(tinymath::dot(halfway, view_dir), 0.0f), f0);

		tinymath::vec3f nominator = ndf * geo * fresnel;
		float ndv = tinymath::max(tinymath::dot(normal, view_dir), 0.0f);
		float ndl = tinymath::max(tinymath::dot(normal, light_dir), 0.0f);
		float denominator = 4.0f * ndv * ndl;
		tinymath::vec3f specular = nominator / std::max(denominator, 0.001f);

		tinymath::vec3f spec_term = fresnel;
		tinymath::vec3f diffuse_term = tinymath::vec3f(1.0f) - spec_term;
		diffuse_term *= 1.0f - metallic;

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::SPECULAR) != RenderFlag::DISABLE)
		{
			return specular;
		}

		return ((diffuse_term * albedo) / PI + specular) * radiance * ndl;
	}

	Color Shader::calculate_main_light(const DirectionalLight& light, const LightingData& lighting_data, const tinymath::vec3f& wpos, const tinymath::vec3f& view_dir, const tinymath::vec3f& normal, Color albedo, Color ao, const tinymath::vec2f& uv, const tinymath::mat3x3& tbn) const
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

		auto light_dir = -tinymath::normalize(light.forward);
		if (this->normal_map)
		{
			light_dir = tbn * light_dir;
		}

		auto half_dir = tinymath::normalize(view_dir + light_dir);

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

		if (name2float.count(roughness_multiplier_prop) > 0 && name2float.count(roughness_offset_prop) > 0 )
		{
			roughness = roughness * name2float.at(roughness_multiplier_prop) + name2float.at(roughness_offset_prop);
		}
		
		if (name2float.count(metallic_multiplier_prop) > 0 && name2float.count(metallic_offset_prop) > 0)
		{
			metallic = metallic * name2float.at(metallic_multiplier_prop) + name2float.at(metallic_offset_prop);
		}

		roughness = tinymath::max(roughness, EPSILON); // brdf lut bug (baking error)

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
			auto spec = tinymath::pow(tinymath::max(tinymath::dot(normal, half_dir), 0.0f), (roughness) * 32.0f);
			auto ndl = tinymath::max(tinymath::dot(normal, light_dir), 0.0f);

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
			tinymath::vec3f f0 = 0.04f;
			f0 = tinymath::lerp(f0, tinymath::vec3f(albedo.r, albedo.g, albedo.b), metallic);
			auto lo = metallic_workflow(f0, tinymath::vec3f(albedo.r, albedo.g, albedo.b), intensity, metallic, roughness, light_distance, half_dir, light_dir, view_dir, normal);

			float ndv = tinymath::max(tinymath::dot(normal, view_dir), 0.0f);

			tinymath::vec3f fresnel = fresnel_schlick_roughness(ndv, f0, roughness);

			tinymath::vec3f specular_term = fresnel;
			tinymath::vec3f diffuse_term = 1.0f - specular_term;
			diffuse_term *= 1.0f - metallic;

			// IBL
			Color irradiance;
			if (INST(GlobalShaderParams).enable_ibl && name2cubemap.count(cubemap_prop) > 0)
			{
				name2cubemap.at(cubemap_prop)->sample_irradiance_map(normal, irradiance);
			}

			Color prefiltered_color;
			if (INST(GlobalShaderParams).enable_ibl && name2cubemap.count(cubemap_prop) > 0)
			{
				auto reflect_dir = reflect(normal, view_dir);
				name2cubemap.at(cubemap_prop)->sample_prefilter_map_lod(reflect_dir, roughness, prefiltered_color);
			}

			Color brdf_lut;
			if (INST(GlobalShaderParams).enable_ibl && name2cubemap.count(cubemap_prop) > 0)
			{
				name2cubemap.at(cubemap_prop)->sample_brdf(tinymath::vec2f(ndv, roughness), brdf_lut);

				/*if (INST(GlobalShaderParams).enable_ibl && brdf_lut.r <= 0.0f)
				{
					uint32_t row, col;
					uv2pixel(512, 512, ndv, roughness, row, col);
					printf("error, %f, %f  ndv: %f, roughness: %f\n", brdf_lut.r, brdf_lut.g, ndv, roughness);
				}*/
			}

			tinymath::vec3f indirect_diffuse = tinymath::vec3f(irradiance.r, irradiance.g, irradiance.b) * tinymath::vec3f(albedo.r, albedo.g, albedo.b) * diffuse_term;
			tinymath::vec3f env_brdf = (fresnel * std::clamp(brdf_lut.r, 0.0f, 1.0f) + brdf_lut.g);
			
			tinymath::vec3f indirect_specular = tinymath::vec3f(prefiltered_color.r, prefiltered_color.g, prefiltered_color.b) * env_brdf;

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

	Color Shader::calculate_point_light(const PointLight& light, const LightingData& lighting_data, const tinymath::vec3f& wpos, const tinymath::vec3f& view_dir, const tinymath::vec3f& normal, Color albedo, Color ao, const tinymath::vec2f& uv, const tinymath::mat3x3& tbn) const
	{
		// TODO
		return Color::BLACK;
	}

	Color Shader::fragment_shader(const v2f& input) const
	{
		if (is_error_shader) { return kErrorColor; }

		auto main_light = INST(GlobalShaderParams).main_light;
		auto point_lights = INST(GlobalShaderParams).point_lights;

		tinymath::vec3f cam_pos = INST(GlobalShaderParams).camera_pos;
		tinymath::vec3f wpos = input.world_pos;
		tinymath::vec4f screen_pos = input.position;

		tinymath::vec3f normal = tinymath::normalize(input.normal);
		tinymath::vec3f view_dir = tinymath::normalize(cam_pos - wpos);

		Color normal_tex;
		tinymath::mat3x3 tbn;
		if (name2tex.count(normal_prop) > 0 && name2tex.at(normal_prop)->sample(input.uv.x, input.uv.y, normal_tex))
		{
			tbn = tinymath::mat3x3(input.tangent, input.bitangent, input.normal);
			view_dir = tinymath::normalize(tbn * view_dir);
			auto packed_normal = tinymath::vec3f(normal_tex.r, normal_tex.g, normal_tex.b);
			normal = tinymath::normalize(packed_normal * 2.0f - 1.0f);
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
			return Color(input.uv.x, input.uv.y, 0.0f, 1.0f);
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

		return Color(ret.r, ret.g, ret.b, 1.0f);
	}
}
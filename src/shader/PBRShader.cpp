#include "PBRShader.hpp"
#include <algorithm>
#include <iostream>
#include "Marcos.h"
#include "Singleton.hpp"
#include "PBR.hpp"

namespace Guarneri
{
	PBRShader::PBRShader(std::string name) : Shader(name, false){}

	PBRShader::~PBRShader()
	{}

	v2f PBRShader::vertex_shader(const a2v& input) const
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
		if (local_properties.has_texture(normal_prop) && !is_error_shader)
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

	float PBRShader::get_shadow_atten(const tinymath::vec4f& light_space_pos) const
	{
		if (!global_shader_properties.has_framebuffer(shadowmap_prop))
		{
			return 0.0f;
		}

		auto shadowmap = global_shader_properties.get_framebuffer(shadowmap_prop);

		tinymath::vec3f proj_shadow_coord = light_space_pos.xyz;
		proj_shadow_coord = proj_shadow_coord * 0.5f + 0.5f;

		if (proj_shadow_coord.z > 1.0f)
		{
			return 0.0f;
		}

		float shadow_atten = 0.0f;

		tinymath::vec2f texel_size = 1.0f / tinymath::vec2f((float)shadowmap->get_height(), (float)shadowmap->get_width());

		if (INST(GlobalShaderParams).pcf_on)
		{
			const int kernel_size = 3;
			// PCF
			for (int x = -kernel_size; x <= kernel_size; x++)
			{
				for (int y = -kernel_size; y <= kernel_size; y++)
				{
					float depth;
					if (shadowmap->get_framebuffer()->read_depth(proj_shadow_coord.x + (float)x * texel_size.x, proj_shadow_coord.y + (float)y * texel_size.y, depth))
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
			if (shadowmap->get_framebuffer()->read_depth(proj_shadow_coord.x, proj_shadow_coord.y, depth))
			{
				shadow_atten = (proj_shadow_coord.z - INST(GlobalShaderParams).shadow_bias) > depth ? 1.0f : 0.0f;
			}
		}

		return shadow_atten * 0.8f;
	}

	tinymath::vec3f PBRShader::reflect(const tinymath::vec3f& n, const tinymath::vec3f& light_out_dir) const
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

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::SPECULAR) != RenderFlag::DISABLE)
		{
			return specular;
		}

		return ((diffuse_term * albedo) / PI + specular) * radiance * ndl;
	}

	tinymath::Color PBRShader::calculate_main_light(const DirectionalLight& light, 
													const LightingData& lighting_data, 
													const tinymath::vec3f& wpos, 
													const tinymath::vec3f& view_dir, 
													const tinymath::vec3f& normal, 
													const tinymath::Color& albedo, 
													const float& ao,
													const tinymath::vec2f& uv, 
													const tinymath::mat3x3& tbn) const
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
		if (local_properties.has_texture(normal_prop))
		{
			light_dir = tbn * light_dir;
		}

		auto half_dir = tinymath::normalize(view_dir + light_dir);

		// sample textures
		float metallic = 0.0f;
		tinymath::Color metallic_color = tinymath::kColorBlack;
		if (local_properties.has_texture(metallic_prop) && local_properties.get_texture(metallic_prop)->sample(uv.x, uv.y, metallic_color))
		{
			metallic = metallic_color.r;
		}

		tinymath::Color roughness_color;
		float roughness = 0.0f;
		if (local_properties.has_texture(roughness_prop) && local_properties.get_texture(roughness_prop)->sample(uv.x, uv.y, roughness_color))
		{
			roughness = roughness_color.r;
		}
		else if (local_properties.has_texture(specular_prop))
		{
			tinymath::Color spec;
			local_properties.get_texture(specular_prop)->sample(uv.x, uv.y, spec);
			roughness = 1.0f - spec.r;
		}

		if (local_properties.has_float(roughness_multiplier_prop) && local_properties.has_float(roughness_offset_prop))
		{
			roughness = roughness * local_properties.get_float(roughness_multiplier_prop) + local_properties.get_float(roughness_offset_prop);
		}

		if (local_properties.has_float(metallic_multiplier_prop) && local_properties.has_float(metallic_offset_prop))
		{
			metallic = metallic * local_properties.get_float(metallic_multiplier_prop) + local_properties.get_float(metallic_offset_prop);
		}

		roughness = tinymath::max(roughness, EPSILON); // brdf lut bug (baking error)

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::ROUGHNESS) != RenderFlag::DISABLE)
		{
			return tinymath::Color(roughness);
		}

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::METALLIC) != RenderFlag::DISABLE)
		{
			return tinymath::Color(metallic_color);
		}

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::AO) != RenderFlag::DISABLE)
		{
			return ao;
		}

		if (INST(GlobalShaderParams).workflow == PBRWorkFlow::Specular)
		{
			auto spec = tinymath::pow(tinymath::max(tinymath::dot(normal, half_dir), 0.0f), (roughness) * 32.0f);
			auto ndl = tinymath::max(tinymath::dot(normal, light_dir), 0.0f);

			auto diffuse = tinymath::saturate(light_diffuse * ndl * albedo);
			tinymath::Color spec_tex = tinymath::kColorWhite;
			tinymath::Color specular;
			if (local_properties.has_texture(specular_prop) && local_properties.get_texture(specular_prop)->sample(uv.x, uv.y, spec_tex))
			{
				specular = tinymath::saturate(light_spec * spec * spec_tex);
			}
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

			tinymath::Color ret = tinymath::Color(lo);
			ret.a = 1.0f;

			// IBL
			tinymath::Color irradiance;
			tinymath::Color prefiltered_color;
			tinymath::Color brdf_lut;
			tinymath::Color indirect_diffuse; 
			tinymath::Color indirect_specular;
			tinymath::Color env_brdf;
			if (INST(GlobalShaderParams).enable_ibl && global_shader_properties.has_cubemap(cubemap_prop))
			{
				auto cubemap = global_shader_properties.get_cubemap(cubemap_prop);
				cubemap->sample_irradiance_map(normal, irradiance);
				auto reflect_dir = reflect(normal, view_dir);
				cubemap->sample_prefilter_map_lod(reflect_dir, roughness, prefiltered_color);
				cubemap->sample_brdf(tinymath::vec2f(ndv, roughness), brdf_lut);

				indirect_diffuse = irradiance.xyz * albedo.xyz * diffuse_term;
				env_brdf = (fresnel * std::clamp(brdf_lut.r, 0.0f, 1.0f) + brdf_lut.g);
				indirect_specular = prefiltered_color * env_brdf;

				auto ambient = (indirect_diffuse + indirect_specular) * ao;
				ret += tinymath::Color(ambient.x, ambient.y, ambient.z, 0.0f);
			}

			if ((INST(GlobalShaderParams).debug_flag & RenderFlag::INDIRECT_DIFFUSE) != RenderFlag::DISABLE)
			{
				return tinymath::Color(indirect_diffuse.r, indirect_diffuse.g, indirect_diffuse.b, 1.0f);
			}

			if ((INST(GlobalShaderParams).debug_flag & RenderFlag::INDIRECT_SPECULAR) != RenderFlag::DISABLE)
			{
				return tinymath::Color(indirect_specular.r, indirect_specular.g, indirect_specular.b, 1.0f);
			}

			return ret;
		}
	}

	tinymath::Color PBRShader::calculate_point_light(const PointLight& light, 
													 const LightingData& lighting_data,
													 const tinymath::vec3f& wpos,
													 const tinymath::vec3f& view_dir,
													 const tinymath::vec3f& normal, 
													 const tinymath::Color& albedo, 
													 const float& ao, 
													 const tinymath::vec2f& uv,
													 const tinymath::mat3x3& tbn) const
	{
		UNUSED(light);
		UNUSED(lighting_data);
		UNUSED(wpos);
		UNUSED(view_dir);
		UNUSED(normal);
		UNUSED(albedo);
		UNUSED(ao);
		UNUSED(uv);
		UNUSED(tbn);
		// TODO
		return tinymath::kColorBlack;
	}

	tinymath::Color PBRShader::fragment_shader(const v2f& input, const Vertex& ddx, const Vertex& ddy) const
	{
		if (is_error_shader) { return kErrorColor; }

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::MIPMAP) != RenderFlag::DISABLE
			&& local_properties.has_texture(albedo_prop))
		{
			int mip = Shader::get_mip_level(ddx.uv, ddy.uv, local_properties.get_texture(albedo_prop)->width, local_properties.get_texture(albedo_prop)->height);
			return mip_colors[mip];
		}

		auto main_light = INST(GlobalShaderParams).main_light;
		auto point_lights = INST(GlobalShaderParams).point_lights;

		tinymath::vec3f cam_pos = INST(GlobalShaderParams).camera_pos;
		tinymath::vec3f wpos = input.world_pos;
		tinymath::vec4f screen_pos = input.position;

		tinymath::vec3f normal = tinymath::normalize(input.normal);
		tinymath::vec3f view_dir = tinymath::normalize(cam_pos - wpos);

		tinymath::Color normal_tex;
		tinymath::mat3x3 tbn;
		if (local_properties.has_texture(normal_prop) && local_properties.get_texture(normal_prop)->sample(input.uv.x, input.uv.y, normal_tex))
		{
			tbn = tinymath::mat3x3(input.tangent, input.bitangent, input.normal);
			view_dir = tinymath::normalize(tbn * view_dir);
			auto packed_normal = tinymath::vec3f(normal_tex.r, normal_tex.g, normal_tex.b);
			normal = tinymath::normalize(packed_normal * 2.0f - 1.0f);
		}

		//shadow
		float shadow_atten = 1.0f - get_shadow_atten(input.shadow_coord);

		tinymath::Color ret = tinymath::kColorBlack;
		tinymath::Color albedo = tinymath::kColorWhite;
		if (local_properties.has_texture(albedo_prop) && local_properties.get_texture(albedo_prop)->sample(input.uv.x, input.uv.y, albedo))
		{
			if (INST(GlobalShaderParams).color_space == ColorSpace::Linear)
			{
				albedo = tinymath::pow(albedo, 2.2f);
			}
		}

		if (local_properties.has_float4(tint_color_prop))
		{
			albedo *= tinymath::Color(local_properties.get_float4(tint_color_prop));
		}

		tinymath::Color ao_color = tinymath::kColorWhite;
		float ao = 1.0f;
		if (local_properties.has_texture(ao_prop) && local_properties.get_texture(ao_prop)->sample(input.uv.x, input.uv.y, ao_color))
		{
			ao = ao_color.r;
		}

		ret += calculate_main_light(main_light, lighting_param, wpos, view_dir, normal, albedo, ao, input.uv, tbn);

		for (auto& light : point_lights)
		{
			ret += calculate_point_light(light, lighting_param, wpos, view_dir, normal, albedo, ao, input.uv, tbn);
		}

		tinymath::Color emmision = tinymath::kColorBlack;
		if (local_properties.has_texture(emission_prop) && local_properties.get_texture(emission_prop)->sample(input.uv.x, input.uv.y, emmision))
		{
			ret += emmision;
		}

		ret *= shadow_atten;

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::SPECULAR) != RenderFlag::DISABLE)
		{
			return tinymath::Color(ao, ao, ao, 1.0f);
		}

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::UV) != RenderFlag::DISABLE)
		{
			return tinymath::Color(input.uv.x, input.uv.y, 0.0f, 1.0f);
		}

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::VERTEX_COLOR) != RenderFlag::DISABLE)
		{
			return input.color;
		}

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::NORMAL) != RenderFlag::DISABLE)
		{
			return normal;
		}

		if (INST(GlobalShaderParams).color_space == ColorSpace::Linear)
		{
			ret = ret / (ret + tinymath::kColorWhite);
			ret = tinymath::pow(ret, 1.0f / 2.2f);
		}

		return tinymath::Color(ret.r, ret.g, ret.b, 1.0f);
	}
}
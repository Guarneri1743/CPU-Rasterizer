#pragma once
#include "Shader.hpp"
#include "PBR.hpp"
#include "Sampling.hpp"

namespace CpuRasterizor
{
	struct MaterialData
	{
		tinymath::Color albedo_color;
		tinymath::vec3f unpacked_normal;
		tinymath::Color specular_color;
		tinymath::Color emission_color;
		float metallic;
		float roughness;
		float ao;
	};

	struct IblData
	{
		tinymath::Color irradiance;
		tinymath::Color prefiltered;
		tinymath::Color brdf_lut;
	};

	struct LightingData
	{
		tinymath::vec3f frag_pos;
		tinymath::vec3f normal;
		tinymath::vec3f light;
		tinymath::vec3f view_dir;
		float ndv;
		float ndl;
		float roughness;
		float metallic;
		float ao;
	};

	class PBRShader : public Shader
	{
	public:
		PBRShader(std::string name) : Shader(name) {}

		tinymath::vec3f reflect(const tinymath::vec3f& n, const tinymath::vec3f& light_out_dir) const
		{
			auto ndl = tinymath::dot(n, light_out_dir);
			return tinymath::normalize(2.0f * n * ndl - light_out_dir);
		}

		v2f vertex_shader(const a2v& input) const
		{
			v2f o;
			auto opos = tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
			auto wpos = model() * opos;
			auto cpos = vp_matrix() * wpos;
			o.position = cpos;
			o.world_pos = wpos.xyz;
			
			auto light_space_pos = CpuRasterSharedData.main_light.light_space() * tinymath::vec4f(wpos.x, wpos.y, wpos.z, 1.0f);
			o.shadow_coord = light_space_pos;
			
			o.color = input.color;
			tinymath::mat3x3 normal_matrix = tinymath::transpose(tinymath::inverse(tinymath::mat4x4_to_mat3x3(model())));
			if (local_properties.has_texture(normal_prop))
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

		void setup(const v2f& input, MaterialData& material_data) const
		{
			auto uv = input.uv;

			auto roughness_color = tinymath::kColorBlack;
			auto metallic_color = tinymath::kColorBlack;
			auto ao_color = tinymath::kColorBlack;
			material_data.emission_color = tinymath::kColorBlack;
			material_data.albedo_color = tinymath::kColorWhite;
			material_data.unpacked_normal = tinymath::kVec3fUp;
			material_data.roughness = 0.0f;
			material_data.metallic = 0.0f;
			material_data.ao = 1.0f;

			tinymath::Color normal_tex = tinymath::Color(0.0f, 1.0f, 0.0f, 1.0f);
			if (local_properties.has_texture(normal_prop))
			{
				if (CpuRasterSharedData.enable_mipmap)
				{
					local_properties.get_texture(normal_prop)->sample(input.uv.x, input.uv.y, input.ddx.uv, input.ddy.uv, normal_tex);
				}
				else
				{
					local_properties.get_texture(normal_prop)->sample(input.uv.x, input.uv.y, normal_tex);
				}

				auto packed_normal = tinymath::vec3f(normal_tex.r, normal_tex.g, normal_tex.b);
				material_data.unpacked_normal = tinymath::normalize(packed_normal * 2.0f - 1.0f);
			}

			if (local_properties.has_texture(albedo_prop))
			{
				if (CpuRasterSharedData.enable_mipmap)
				{
					local_properties.get_texture(albedo_prop)->sample(input.uv.x, input.uv.y, input.ddx.uv, input.ddy.uv, material_data.albedo_color);
				}
				else
				{
					local_properties.get_texture(albedo_prop)->sample(input.uv.x, input.uv.y, material_data.albedo_color);
				}

				if (CpuRasterSharedData.color_space == ColorSpace::kLinear)
				{
					material_data.albedo_color = tinymath::pow(material_data.albedo_color, 2.2f);
				}
			}

			if (local_properties.has_texture(metallic_prop))
			{
				if (CpuRasterSharedData.enable_mipmap)
				{
					local_properties.get_texture(metallic_prop)->sample(uv.x, uv.y, input.ddx.uv, input.ddy.uv, metallic_color);
				}
				else
				{
					local_properties.get_texture(metallic_prop)->sample(uv.x, uv.y, metallic_color);
				}

				material_data.metallic = metallic_color.r;
			}

			if (CpuRasterSharedData.workflow == PBRWorkFlow::kMetallic)
			{
				if (local_properties.has_texture(roughness_prop))
				{
					if (CpuRasterSharedData.enable_mipmap)
					{
						local_properties.get_texture(roughness_prop)->sample(uv.x, uv.y, input.ddx.uv, input.ddy.uv, roughness_color);
					}
					else
					{
						local_properties.get_texture(roughness_prop)->sample(uv.x, uv.y, roughness_color);
					}
					material_data.roughness = roughness_color.r;
				}
				else if (local_properties.has_texture(specular_prop))
				{
					if (CpuRasterSharedData.enable_mipmap)
					{
						local_properties.get_texture(specular_prop)->sample(uv.x, uv.y, input.ddx.uv, input.ddy.uv, material_data.specular_color);
					}
					else
					{
						local_properties.get_texture(specular_prop)->sample(uv.x, uv.y, material_data.specular_color);
					}
					material_data.roughness = 1.0f - material_data.specular_color.r;
				}
			}
			else
			{
				if (local_properties.has_texture(specular_prop))
				{
					if (CpuRasterSharedData.enable_mipmap)
					{
						local_properties.get_texture(specular_prop)->sample(uv.x, uv.y, input.ddx.uv, input.ddy.uv, material_data.specular_color);
					}
					else
					{
						local_properties.get_texture(specular_prop)->sample(uv.x, uv.y, material_data.specular_color);
					}
					material_data.roughness = 1.0f - material_data.specular_color.r;
				}
			}

			if (local_properties.has_texture(ao_prop))
			{
				if (CpuRasterSharedData.enable_mipmap)
				{
					local_properties.get_texture(ao_prop)->sample(input.uv.x, input.uv.y, input.ddx.uv, input.ddy.uv, ao_color);
				}
				else
				{
					local_properties.get_texture(ao_prop)->sample(input.uv.x, input.uv.y, ao_color);
				}
				material_data.ao = ao_color.r;
			}

			if (local_properties.has_texture(emission_prop))
			{
				if (CpuRasterSharedData.enable_mipmap)
				{
					local_properties.get_texture(emission_prop)->sample(input.uv.x, input.uv.y, input.ddx.uv, input.ddy.uv, material_data.emission_color);
				}
				else
				{
					local_properties.get_texture(emission_prop)->sample(input.uv.x, input.uv.y, material_data.emission_color);
				}
			}

			if (local_properties.has_float(roughness_multiplier_prop) && local_properties.has_float(roughness_offset_prop))
			{
				material_data.roughness = material_data.roughness * local_properties.get_float(roughness_multiplier_prop) + local_properties.get_float(roughness_offset_prop);
			}

			if (local_properties.has_float(metallic_multiplier_prop) && local_properties.has_float(metallic_offset_prop))
			{
				material_data.metallic = material_data.metallic * local_properties.get_float(metallic_multiplier_prop) + local_properties.get_float(metallic_offset_prop);
			}

			material_data.roughness = tinymath::max(material_data.roughness, EPSILON); // brdf lut bug (baking error)
		}

		void setup(const float& ndv, const float& roughness, const tinymath::vec3f& normal, const tinymath::vec3f& view_dir, IblData& ibl_data) const
		{
			ibl_data.irradiance = tinymath::kColorBlack;
			ibl_data.prefiltered = tinymath::kColorBlack;
			ibl_data.brdf_lut = tinymath::kColorBlack;
			if (CpuRasterSharedData.enable_ibl && ShaderPropertyMap::global_shader_properties.has_cubemap(cubemap_prop))
			{
				auto cubemap = ShaderPropertyMap::global_shader_properties.get_cubemap(cubemap_prop);
				cubemap->sample_irradiance_map(normal, ibl_data.irradiance);
				auto reflect_dir = reflect(normal, view_dir);
				cubemap->sample_prefilter_map_lod(reflect_dir, roughness, ibl_data.prefiltered);
				cubemap->sample_brdf(tinymath::vec2f(ndv, roughness), ibl_data.brdf_lut);
			}
		}

		float get_shadow_atten(const tinymath::vec4f& light_space_pos) const
		{
			if (!ShaderPropertyMap::global_shader_properties.has_framebuffer(shadowmap_prop))
			{
				return 0.0f;
			}

			auto shadowmap = ShaderPropertyMap::global_shader_properties.get_framebuffer(shadowmap_prop);

			tinymath::vec3f proj_shadow_coord = light_space_pos.xyz;
			proj_shadow_coord = proj_shadow_coord * 0.5f + 0.5f;

			if (proj_shadow_coord.z > 1.0f)
			{
				return 0.0f;
			}

			float shadow_atten = 0.0f;

			tinymath::vec2f texel_size = 1.0f / tinymath::vec2f((float)shadowmap->get_height(), (float)shadowmap->get_width());

			if (CpuRasterSharedData.pcf_on)
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
							shadow_atten += (proj_shadow_coord.z - CpuRasterSharedData.shadow_bias) > depth ? 1.0f : 0.0f;
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
					shadow_atten = (proj_shadow_coord.z - CpuRasterSharedData.shadow_bias) > depth ? 1.0f : 0.0f;
				}
			}

			return shadow_atten * 0.8f;
		}

		tinymath::vec3f metallic_workflow(const tinymath::vec3f& f0, 
										  const tinymath::vec3f& albedo, 
										  const float& intensity, 
										  const float& metallic, 
										  const float& roughness, 
										  const float& light_distance,
										  const tinymath::vec3f& halfway, 
										  const tinymath::vec3f& light_dir, 
										  const tinymath::vec3f& view_dir,
										  const tinymath::vec3f& normal) const
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

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kSpecular) != RenderFlag::kNone)
			{
				return specular;
			}

			return ((diffuse_term * albedo) / PI + specular) * radiance * ndl;
		}

		tinymath::Color calculate_main_light(const DirectionalLight& light,
														const MaterialData& material_data,
														const IblData& ibl_data,
														const tinymath::vec3f& wpos,
														const tinymath::vec3f& view_dir,
														const tinymath::vec3f& normal,
														const float& ndv,
														const tinymath::Color& albedo,
														const float& ao) const
		{
			UNUSED(wpos);
			auto light_ambient = light.ambient;
			auto light_spec = light.specular;
			auto light_diffuse = light.diffuse;
			auto intensity = light.intensity;
			UNUSED(intensity);
			light_diffuse *= intensity;
			light_spec *= intensity;
			float light_distance = 1.0f;

			auto light_dir = -tinymath::normalize(light.forward);

			auto half_dir = tinymath::normalize(view_dir + light_dir);

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kAlbedo) != RenderFlag::kNone)
			{
				return material_data.albedo_color;
			}

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kRoughness) != RenderFlag::kNone)
			{
				return tinymath::Color(material_data.roughness, material_data.roughness, material_data.roughness, 1.0f);
			}

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kMetallic) != RenderFlag::kNone)
			{
				return tinymath::Color(material_data.metallic, material_data.metallic, material_data.metallic, 1.0f);
			}

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kAO) != RenderFlag::kNone)
			{
				return ao;
			}

			if (CpuRasterSharedData.workflow == PBRWorkFlow::kSpecular)
			{
				auto spec = tinymath::pow(tinymath::max(tinymath::dot(normal, half_dir), 0.0f), (material_data.roughness) * 32.0f);
				auto ndl = tinymath::max(tinymath::dot(normal, light_dir), 0.0f);

				auto diffuse = tinymath::saturate(light_diffuse * ndl * albedo);
				tinymath::Color spec_tex = tinymath::kColorWhite;
				tinymath::Color specular = material_data.specular_color;
				specular = tinymath::saturate(light_spec * spec * spec_tex);
				auto ambient = light_ambient;
				auto ret = ambient + diffuse + specular;
				return ret;
			}
			else
			{
				tinymath::vec3f f0 = 0.04f;
				f0 = tinymath::lerp(f0, tinymath::vec3f(albedo.r, albedo.g, albedo.b), material_data.metallic);
				auto lo = metallic_workflow(f0, tinymath::vec3f(albedo.r, albedo.g, albedo.b), intensity, material_data.metallic, material_data.roughness, light_distance, half_dir, light_dir, view_dir, normal);

				tinymath::vec3f fresnel = fresnel_schlick_roughness(ndv, f0, material_data.roughness);

				tinymath::vec3f specular_term = fresnel;
				tinymath::vec3f diffuse_term = 1.0f - specular_term;
				diffuse_term *= 1.0f - material_data.metallic;

				tinymath::Color ret = tinymath::Color(lo);
				ret.a = 1.0f;

				// IBL
				tinymath::Color irradiance = ibl_data.irradiance;
				tinymath::Color prefiltered_color = ibl_data.prefiltered;
				tinymath::Color brdf_lut = ibl_data.brdf_lut;

				tinymath::Color indirect_diffuse;
				tinymath::Color indirect_specular;
				tinymath::Color env_brdf;
				if (CpuRasterSharedData.enable_ibl && ShaderPropertyMap::global_shader_properties.has_cubemap(cubemap_prop))
				{
					indirect_diffuse = irradiance.xyz * albedo.xyz * diffuse_term;
					env_brdf = (fresnel * std::clamp(brdf_lut.r, 0.0f, 1.0f) + brdf_lut.g);
					indirect_specular = prefiltered_color * env_brdf;

					auto ambient = (indirect_diffuse + indirect_specular) * ao;
					ret += tinymath::Color(ambient.x, ambient.y, ambient.z, 0.0f);
				}

				if ((CpuRasterSharedData.debug_flag & RenderFlag::kIndirectDiffuse) != RenderFlag::kNone)
				{
					return tinymath::Color(indirect_diffuse.r, indirect_diffuse.g, indirect_diffuse.b, 1.0f);
				}

				if ((CpuRasterSharedData.debug_flag & RenderFlag::kIndirectSpecular) != RenderFlag::kNone)
				{
					return tinymath::Color(indirect_specular.r, indirect_specular.g, indirect_specular.b, 1.0f);
				}

				return ret;
			}
		}

		tinymath::Color calculate_point_light(const PointLight& light,
														 const MaterialData& material_data,
														 const IblData& ibl_data,
														 const tinymath::vec3f& wpos,
														 const tinymath::vec3f& view_dir,
														 const tinymath::vec3f& normal,
														 const float& ndv,
														 const tinymath::Color& albedo,
														 const float& ao) const
		{
			UNUSED(light);
			UNUSED(material_data);
			UNUSED(ibl_data);
			UNUSED(wpos);
			UNUSED(view_dir);
			UNUSED(normal);
			UNUSED(albedo);
			UNUSED(ao);
			UNUSED(ndv);

			// TODO
			return tinymath::kColorBlack;
		}

		tinymath::Color fragment_shader(const v2f& input) const
		{
			MaterialData material_data;
			IblData ibl_data;

			setup(input, material_data);

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kMipmap) != RenderFlag::kNone
				&& local_properties.has_texture(albedo_prop))
			{
				int mip = get_mip_level(input.ddx.uv, input.ddy.uv, local_properties.get_texture(albedo_prop)->width, local_properties.get_texture(albedo_prop)->height);
				return mip_colors[mip];
			}

			auto main_light = CpuRasterSharedData.main_light;
			auto point_lights = CpuRasterSharedData.point_lights;

			tinymath::vec3f cam_pos = CpuRasterSharedData.camera_pos;
			tinymath::vec3f wpos = input.world_pos;
			tinymath::vec4f screen_pos = input.position;

			tinymath::vec3f view_dir = tinymath::normalize(cam_pos - wpos);
			tinymath::vec3f normal = tinymath::normalize(input.normal);

			tinymath::mat3x3 tbn = tinymath::kMat3x3Identity;
			if (local_properties.has_texture(normal_prop))
			{
				// todo: calculate lighting in tangent space
				tbn = tinymath::mat3x3(input.tangent, input.bitangent, input.normal);
				if (local_properties.has_texture(normal_prop))
				{
					normal = tinymath::normalize(tinymath::transpose(tbn) * material_data.unpacked_normal);
				}
			}

			float ndv = tinymath::max(tinymath::dot(normal, view_dir), 0.0f);

			setup(ndv, material_data.roughness, normal, view_dir, ibl_data);

			//shadow
			float shadow_atten = 1.0f - get_shadow_atten(input.shadow_coord);

			tinymath::Color albedo = material_data.albedo_color;
			float ao = material_data.ao;

			if (local_properties.has_float4(tint_color_prop))
			{
				albedo *= tinymath::Color(local_properties.get_float4(tint_color_prop));
			}

			tinymath::Color ret = tinymath::kColorBlack;

			ret += calculate_main_light(main_light, material_data, ibl_data, wpos, view_dir, normal, ndv, albedo, ao);

			for (auto& light : point_lights)
			{
				ret += calculate_point_light(light, material_data, ibl_data, wpos, view_dir, normal, ndv, albedo, ao);
			}

			ret *= shadow_atten;
			ret += material_data.emission_color;

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kSpecular) != RenderFlag::kNone)
			{
				return tinymath::Color(ao, ao, ao, 1.0f);
			}

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kUV) != RenderFlag::kNone)
			{
				return tinymath::Color(input.uv.x, input.uv.y, 0.0f, 1.0f);
			}

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kVertexColor) != RenderFlag::kNone)
			{
				return input.color;
			}

			if ((CpuRasterSharedData.debug_flag & RenderFlag::kNormal) != RenderFlag::kNone)
			{
				return tinymath::Color(normal.x, normal.y, normal.z, 1.0f);
			}

			if (CpuRasterSharedData.color_space == ColorSpace::kLinear)
			{
				ret = ret / (ret + tinymath::kColorWhite);
				ret = tinymath::pow(ret, 1.0f / 2.2f);
			}

			return tinymath::Color(ret.r, ret.g, ret.b, 1.0f);
		}
	};
}
#pragma once
#include "Shader.hpp"

namespace Guarneri
{
	class PBRShader : public Shader
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

	public:
		PBRShader(std::string name);
		~PBRShader();
		v2f vertex_shader(const a2v& input) const;
		tinymath::Color fragment_shader(const v2f& input) const;
		
	private:
		void setup(const v2f& input, MaterialData& material_data) const;
		void setup(const float& ndv, const float& roughness, const tinymath::vec3f& normal, const tinymath::vec3f& view_dir, IblData& ibl_data) const;

		float get_shadow_atten(const tinymath::vec4f& light_space_pos) const;

		tinymath::vec3f reflect(const tinymath::vec3f& n, const tinymath::vec3f& light_out_dir) const;

		tinymath::Color calculate_main_light(const DirectionalLight& light,
											 const MaterialData& material_data,
											 const IblData& ibl_data,
											 const tinymath::vec3f& wpos,
											 const tinymath::vec3f& v,
											 const tinymath::vec3f& n,
											 const float& ndv,
											 const tinymath::Color& albedo,
											 const float& ao,
											 const tinymath::mat3x3& tbn) const;

		tinymath::Color calculate_point_light(const PointLight& light, 
											  const MaterialData& material_data,
											  const IblData& ibl_data,
											  const tinymath::vec3f& wpos, 
											  const tinymath::vec3f& v, 
											  const tinymath::vec3f& n, 
											  const float& ndv,
											  const tinymath::Color& albedo,
											  const float& ao,
											  const tinymath::mat3x3& tbn) const;
	};
}
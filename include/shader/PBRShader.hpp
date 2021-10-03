#pragma once
#include "Shader.hpp"

namespace Guarneri
{
	class PBRShader : public Shader
	{
	public:
		PBRShader(std::string name);
		~PBRShader();
		v2f vertex_shader(const a2v& input) const;
		tinymath::Color fragment_shader(const v2f& input) const;
		
	private:
		float get_shadow_atten(const tinymath::vec4f& light_space_pos) const;

		tinymath::vec3f reflect(const tinymath::vec3f& n, const tinymath::vec3f& light_out_dir) const;

		tinymath::Color calculate_main_light(const DirectionalLight& light, 
											 const LightingData& lighting_data, 
											 const tinymath::vec3f& wpos, 
											 const tinymath::vec3f& v, 
											 const tinymath::vec3f& n, 
											 tinymath::Color albedo, 
											 tinymath::Color ao, 
											 const tinymath::vec2f& uv, 
											 const tinymath::mat3x3& tbn) const;

		tinymath::Color calculate_point_light(const PointLight& light, 
											  const LightingData& lighting_data, 
											  const tinymath::vec3f& wpos, 
											  const tinymath::vec3f& v, 
											  const tinymath::vec3f& n, 
											  tinymath::Color albedo, 
											  tinymath::Color ao, 
											  const tinymath::vec2f& uv, 
											  const tinymath::mat3x3& tbn) const;
	};
}
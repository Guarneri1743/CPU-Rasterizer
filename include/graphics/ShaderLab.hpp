#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "Shader.hpp"
#include "PBRShader.hpp"
#include "SkyboxShader.hpp"
#include "ShadowShader.hpp"
#include "LightShader.hpp"

namespace CpuRasterizer
{
	static std::unordered_map<std::string, std::shared_ptr<Shader>> name2shaders;

	class ShaderLab
	{
	public:
		void ShaderLab::initialize()
		{}

		static std::shared_ptr<Shader> get_shader(std::string name)
		{
			if (name == "pbr_shader")
			{
				return  std::make_shared<PBRShader>(name);
			}
			else if (name == "skybox_shader")
			{
				return  std::make_shared<SkyboxShader>();
			}
			else if (name == "shadow_shader")
			{
				return  std::make_shared<ShadowShader>();
			}
			else if (name == "light_shader")
			{
				return  std::make_shared<LightShader>();
			}
			else
			{
				return  std::make_shared<Shader>(name);
			}
		}
	};
}
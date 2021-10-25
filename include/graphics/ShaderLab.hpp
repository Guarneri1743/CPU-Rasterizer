#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "ShaderProgram.hpp"
#include "PBRShader.hpp"
#include "SkyboxShader.hpp"
#include "ShadowShader.hpp"
#include "LightShader.hpp"

namespace CpuRasterizer
{
	static std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> name2shaders;

	class ShaderLab
	{
	public:
		void ShaderLab::initialize()
		{}

		// todo
		static std::shared_ptr<ShaderProgram> get_shader(std::string name)
		{
			if (name == "pbr_shader")
			{
				return  std::make_shared<PBRShader>();
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
				return  std::make_shared<PBRShader>();
			}
		}
	};
}
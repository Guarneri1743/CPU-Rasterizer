#include "ShaderLab.hpp"
#include "PBRShader.hpp"
#include "SkyboxShader.hpp"
#include "ShadowShader.hpp"
#include "LightShader.hpp"

//TODO
namespace Guarneri
{
	std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderLab::name2shaders;

	void ShaderLab::initialize()
	{}

	std::shared_ptr<Shader> ShaderLab::get_shader(std::string name)
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

		/*if (name2shaders.count(name) > 0)
		{
			return name2shaders[name];
		}

		name2shaders[name] = std::make_shared<Shader>(name);
		return name2shaders[name];*/
	}
}
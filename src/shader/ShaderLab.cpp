#include "ShaderLab.hpp"
#include "SkyboxShader.hpp"
#include "ShadowShader.hpp"
#include "LightShader.hpp"

//TODO
namespace Guarneri
{
	std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderLab::name2shaders;

	void ShaderLab::initialize()
	{
		name2shaders["pbr_shader"] = std::make_shared<Shader>("pbr_shader");
		name2shaders["skybox_shader"] = std::make_shared<SkyboxShader>();
		name2shaders["shadow_shader"] = std::make_shared<ShadowShader>();
		name2shaders["light_shader"] = std::make_shared<LightShader>(); 
	}

	std::shared_ptr<Shader> ShaderLab::get_shader(std::string name)
	{
		if (name2shaders.count(name) > 0)
		{
			return name2shaders[name];
		}

		name2shaders[name] = std::make_shared<Shader>(name);
		return name2shaders[name];
	}
}
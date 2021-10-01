#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "Shader.hpp"

namespace Guarneri
{
	class ShaderLab
	{
	public:
		static void initialize();
		static std::shared_ptr<Shader> get_shader(std::string name);

	private:
		static std::unordered_map<std::string, std::shared_ptr<Shader>> name2shaders;
	};
}
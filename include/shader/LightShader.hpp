#pragma once
#include <string>
#include "Shader.hpp"
#include "tinymath.h"

namespace CpuRasterizor
{
	class LightShader : public Shader
	{
	public:
		LightShader();
		~LightShader();

		v2f vertex_shader(const a2v& input) const;
		tinymath::Color fragment_shader(const v2f& input) const;
	};
}
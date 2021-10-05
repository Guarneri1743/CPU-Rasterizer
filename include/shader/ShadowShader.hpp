#pragma once
#include <string>
#include "Shader.hpp"
#include "tinymath.h"

namespace CpuRasterizor
{
	class ShadowShader : public Shader
	{
	public:
		ShadowShader();
		~ShadowShader();

	public:
		v2f vertex_shader(const a2v& input) const;
		tinymath::Color fragment_shader(const v2f& input) const;
	};
}
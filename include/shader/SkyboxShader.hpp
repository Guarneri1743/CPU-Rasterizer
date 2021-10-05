#pragma once
#include <string>
#include "tinymath.h"
#include "Shader.hpp"

namespace Guarneri
{
	class SkyboxShader : public Shader
	{
	public:
		SkyboxShader();
		~SkyboxShader();

	public:
		v2f vertex_shader(const a2v& input) const;
		tinymath::Color fragment_shader(const v2f& input) const;
	};
}
#ifndef _SHADOW_SHADER_
#define _SHADOW_SHADER_
#include <string>
#include "Shader.hpp"
#include "Color.hpp"

namespace Guarneri
{
	class ShadowShader : public Shader
	{
	public:
		ShadowShader();
		~ShadowShader();

	public:
		v2f vertex_shader(const a2v& input) const;
		Color fragment_shader(const v2f& input) const;
	};
}
#endif
#ifndef _LIGHT_SHADER_
#define _LIGHT_SHADER_
#include <string>
#include "Shader.hpp"
#include "Color.hpp"

namespace Guarneri
{
	class LightShader : public Shader
	{
	public:
		LightShader();
		~LightShader();

		v2f vertex_shader(const a2v& input) const;
		Color fragment_shader(const v2f& input) const;
	};
}
#endif
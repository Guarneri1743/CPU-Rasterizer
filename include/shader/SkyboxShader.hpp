#ifndef _SKYBOX_SHADER_
#define _SKYBOX_SHADER_
#include <string>
#include "Color.hpp"
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
		Color fragment_shader(const v2f& input) const;
		std::string str() const;
	};
}
#endif
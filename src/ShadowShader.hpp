#ifndef _SHADOW_SHADER_
#define _SHADOW_SHADER_
#include <Guarneri.hpp>

namespace Guarneri {
	class ShadowShader : public Shader{
	public:
		ShadowShader() { this->shadow = true; }
		~ShadowShader() {}

	public: 
		v2f vertex_shader(const a2v& input) const {
			v2f o;
			o.position = light_space * m * Vector4(input.position.xyz(), 1.0f);
			o.shadow_coord = input.position;
			return o;
		}

		Color fragment_shader(const v2f& input) const {
			Color c = input.uv;
			return c;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "ShadowShader[" << this->id << "]" << std::endl;
			return ss.str();
		}
	};
}
#endif
#ifndef _LIGHT_SHADER_
#define _LIGHT_SHADER_
#include <Guarneri.hpp>

namespace Guarneri {
	class LightShader : public Shader {
	public:
		LightShader() {}
		~LightShader() {}

	public:
		v2f vertex_shader(const a2v& input) const {
			v2f o;
			auto opos = Vector4(input.position.xyz(), 1.0f);
			auto wpos = model * opos;
			auto cpos = projection * view * wpos;
			o.position = cpos;
			return o;
		}

		Color fragment_shader(const v2f& input) const {
			REF(input)
			return Color::WHITE;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "LightShader[" << this->id << "]" << std::endl;
			return ss.str();
		}
	};
}
#endif
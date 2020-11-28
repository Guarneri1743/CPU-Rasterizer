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
			o.position = misc_param.main_light.light_space() * m * Vector4(input.position.xyz(), 1.0f);
			o.shadow_coord = o.position;
			return o;
		}

		Color fragment_shader(const v2f& input) const {
			//auto shadow_coord = input.shadow_coord * 0.5f + 0.5f;
			//printf("shadow: %f, %f, %f %f\npos: %f, %f, %f %f\n", shadow_coord.x, shadow_coord.y, shadow_coord.z, shadow_coord.w, input.position.x, input.position.y, input.position.z, input.position.w);
			REF(input)
			return Color::BLACK;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "ShadowShader[" << this->id << "]" << std::endl;
			return ss.str();
		}
	};
}
#endif
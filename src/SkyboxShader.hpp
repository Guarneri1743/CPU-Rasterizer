#pragma once
#include <Guarneri.hpp>
#include <Shader.hpp>

namespace Guarneri {
	class SkyboxShader : public Shader {
	public:
		SkyboxShader() {}
		~SkyboxShader(){}

	public:
		v2f vertex_shader(const a2v& input) {
			v2f o;
			auto clip_pos = p * v * input.position;
			o.position = Vector4(clip_pos.xy(), clip_pos.ww());
			o.uv = input.uv;
			return o;
		}

		Color fragment_shader(const v2f& input)  {
			Color main_tex;
			if (name2tex.count(albedo_prop) > 0 && name2tex[albedo_prop]->sample(input.uv.x, input.uv.y, main_tex)) {
				return main_tex;
			}
			return Color::BLACK;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "SkyboxShader[" << this->id << "]" << std::endl;
			return ss.str();
		}
	};
}
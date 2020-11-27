#ifndef _SKYBOX_SHADER_
#define _SKYBOX_SHADER_
#include <Guarneri.hpp>

namespace Guarneri {
	class SkyboxShader : public Shader {
	public:
		SkyboxShader() { this->skybox = true; }
		~SkyboxShader(){}

	public:
		v2f vertex_shader(const a2v& input) const {
			v2f o;
			auto clip_pos = p * v * Vector4(input.position.xyz(), 1.0f);
			o.position = Vector4(clip_pos.xy(), clip_pos.ww());
			o.shadow_coord = input.position;
			return o;
		}

		Color fragment_shader(const v2f& input) const {
			Color sky_color;

			if (name2cubemap.count(skybox_cubemap_prop) > 0 && name2cubemap.at(skybox_cubemap_prop)->sample(input.shadow_coord.xyz(), sky_color)) {
				if ((misc_param.render_flag & RenderFlag::UV) != RenderFlag::DISABLE) {
					int index;
					return name2cubemap.at(skybox_cubemap_prop)->sample(input.shadow_coord.xyz(), index);
				}
				return sky_color;
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
#endif
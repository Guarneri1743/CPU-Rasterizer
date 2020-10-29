#pragma once
#include <common.hpp>
#include <shader.hpp>
#include <float4.hpp>
#include <texture.hpp>
#include <unordered_map>
#include <shader_manager.hpp>
#include <texture_manager.hpp>

namespace guarneri {
	class material {
	public:
		material(shader* shader) {
			this->target_shader = shader;
		}

	public:
		float4 albedo;
		float4 roughness;
		shader* target_shader;
		std::unordered_map<texture_id, texture*> name2tex;

	public:
		void set_shader(shader* shader) {
			this->target_shader = shader;
		}

		shader* get_shader() {
			return target_shader;
		}

		void set_texture(texture_id name, texture* tex) {
			if (tex == nullptr) {
				return;
			}
			name2tex[name] = tex;
		}

		texture* get_texture(texture_id name) {
			if (name2tex.count(name) > 0) {
				return name2tex[name];
			}
			return nullptr;
		}
	};
}
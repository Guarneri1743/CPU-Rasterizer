#pragma once
#include <common.hpp>
#include <shader.hpp>
#include <float4.hpp>
#include <texture.hpp>
#include <unordered_map>

namespace guarneri {
	typedef std::string texture_name;
	class material {
	public:
		material(id_t shader_id) {
			this->shader_id = shader_id;
		}

	private:
		id_t shader_id;
		std::unordered_map<texture_name, id_t> name2id;

	public:
		float4 albedo;
		float4 roughness;

	public:
		id_t get_shader_id() {
			return shader_id;
		}

		void set_texture(texture_name name, texture* tex) {
			if (tex == nullptr) {
				return;
			}
			name2id[name] = tex->get_id();
		}

		id_t get_texture(texture_name name) {
			if (name2id.count(name) > 0) {
				auto id = name2id[name];
				return id;
			}
			return INVALID_TEXTURE_ID;
		}
	};
}
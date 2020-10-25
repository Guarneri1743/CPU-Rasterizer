#pragma once
#include <common.hpp>
#include <texture.hpp>
#include <unordered_map>
#include <id_allocator.hpp>

namespace guarneri {
	class texture_manager {
	public:
		texture_manager() {
			allocator = new id_allocator(0, 1024);
		}

		~texture_manager() {
			for (auto iter = textures.begin(); iter != textures.end(); iter++) {
				delete iter->second;
			}
			textures.clear();
			delete allocator;
		}

	public:
		static texture_manager* singleton() {
			if (texture_manager::instance == nullptr) {
				texture_manager::instance = new texture_manager();
			}
			return texture_manager::instance;
		}
	private:
		static texture_manager* instance;
		std::unordered_map<id_t, texture*> textures;
		id_allocator* allocator;

	public:
		bool bind_texture(texture* texture, id_t& id) {
			int free_id = 0;
			if (allocator->alloc(free_id)) {
				id = static_cast<id_t>(free_id);
				textures[id] = texture;
				return true;
			}
			else {
				std::cerr << "bind texture failed" << std::endl;
				return false;
			}
		}

		texture* get_texture(const id_t& id) {
			if (textures.count(id) > 0) {
				return textures[id];
			}
			return nullptr;
		}

		void unbind_texture(const id_t& id) {
			allocator->free(static_cast<int>(id));
		}
	};
	texture_manager* texture_manager::instance;
}
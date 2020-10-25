#pragma once
#include <common.hpp>
#include <shader.hpp>
#include <unordered_map>
#include <id_allocator.hpp>

namespace guarneri {
	class shader_manager {
	public:
		shader_manager() {
			allocator = new id_allocator(0, 1024);
		}

		~shader_manager() {
			for (auto iter = shaders.begin(); iter != shaders.end(); iter++) {
				delete iter->second;
			}
			shaders.clear();
			delete allocator;
		}

	public:
		static shader_manager* singleton() {
			if (shader_manager::instance == nullptr) {
				shader_manager::instance = new shader_manager();
			}
			return shader_manager::instance;
		}

	private:
		static shader_manager* instance;
		std::unordered_map<id_t, shader*> shaders;
		id_allocator* allocator;

	public:
		bool add_shader(shader* texture, id_t& id) {
			int free_id = 0;
			if (allocator->alloc(free_id)) {
				id = static_cast<id_t>(free_id);
				shaders[id] = texture;
				return true;
			}
			else {
				std::cerr << "add shader failed" << std::endl;
				return false;
			}
		}

		shader* get_shader(const id_t& id) {
			if (shaders.count(id) > 0) {
				return shaders[id];
			}
			return nullptr;
		}

		void remove_shader(const id_t& id) {
			allocator->free(static_cast<int>(id));
		}
	};
	shader_manager* shader_manager::instance;
}
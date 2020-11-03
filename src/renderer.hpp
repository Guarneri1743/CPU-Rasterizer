#pragma once
#include <guarneri.hpp>

namespace guarneri {
	class renderer {
	public:
		renderer(std::unique_ptr<model> model) {
			this->target = std::move(model);
		}

		renderer(const renderer& other) {
			deep_copy(other);
		}

		~renderer() {
			target.reset();
		}

	public:
		std::shared_ptr<model> target;
		vertex vertices[3];

	public:
		static std::unique_ptr<renderer> create(std::unique_ptr<model> model) {
			return std::make_unique<renderer>(std::move(model));
		}

		static std::unique_ptr<renderer> create(const renderer& other) {
			return std::make_unique<renderer>(other);
		}

		void render() {
			auto view = camera::main_camera->view_matrix();
			auto proj = camera::main_camera->projection_matrix();
			if (target != nullptr) {
				for(auto& m : target->meshes) {
					assert(m->indices.size() % 3 == 0);
					uint32_t idx = 0;
					for(auto& index : m->indices) {
						assert(idx < 3 && index < m->vertices.size());
						vertices[idx] = m->vertices[index];
						idx++;
						if (idx == 3) {
							grapihcs().draw_primitive(m->mat, vertices[0], vertices[1], vertices[2], target->transform.local2world, view, proj);
							idx = 0;
						}
					}
				}
			}
		}

		void operator =(const renderer& other) {
			deep_copy(other);
		}

		void deep_copy(const renderer& other) {
			this->target = other.target;
			std::copy(std::begin(other.vertices), std::end(other.vertices), std::begin(vertices));
		}
	};
}
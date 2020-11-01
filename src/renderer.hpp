#pragma once
#include <guarneri.hpp>

namespace guarneri {
	class renderer {
	public:
		renderer(const std::shared_ptr<model>& model) {
			this->model = model;
		}

		renderer(const renderer& other) {
			deep_copy(other);
		}

		~renderer() {
			model.reset();
		}

	public:
		std::shared_ptr<model> model;
		vertex vertices[3];

	public:
		void render() {

			auto view = singleton<camera>::get().view_matrix();

			auto proj = singleton<camera>::get().projection_matrix();

			if (model != nullptr) {
				auto meshes = model->meshes;
				for(auto& m : meshes) {
					assert(m->indices.size() % 3 == 0);
					uint32_t idx = 0;
					for(auto& index : m->indices) {
						assert(idx < 3 && index < m->vertices.size());
						vertices[idx] = m->vertices[index];
						idx++;
						if (idx == 3) {
							grapihcs().draw_primitive(m->material, vertices[0], vertices[1], vertices[2], model->transform.local2world, view, proj);
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
			this->model = other.model;
			std::copy(std::begin(other.vertices), std::end(other.vertices), std::begin(vertices));
		}
	};
}
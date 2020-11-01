#pragma once
#include <common.hpp>
#include <mesh.hpp>
#include <material.hpp>
#include <transform.hpp>
#include <model.hpp>
#include <singleton.hpp>
#include <render_device.hpp>

namespace guarneri {
	class renderer {
	public:
		renderer(model* model, material* material) {
			this->model = model;
			this->material = material;
		}

		~renderer() {
			delete model;
			delete material;
		}

	public:
		transform transform;
		model* model;
		material* material;

	public:
		void render() {
			auto view = singleton<camera>::instance().view_matrix();
			auto proj = singleton<camera>::instance().get_projection_matrix();
			if (model != nullptr) {
				auto meshes = model->meshes;
				vertex vertices[3];
				for (auto iter = meshes.begin(); iter != meshes.end(); iter++) {
					auto mesh = *iter;
					assert(mesh.indices.size() % 3 == 0);
					uint32_t idx = 0;
					for (auto iiter = mesh.indices.begin(); iiter != mesh.indices.end(); iiter++) {
						uint32_t index = *iiter;
						if (mesh.vertices.size() > index) {
							if (idx == 3) {
								vertices[idx] = mesh.vertices[index];
								singleton<render_device>::instance().draw_primitive(*material, vertices[0], vertices[1], vertices[2], transform.local2world, view, proj);
								idx = 0;
							}
							idx++;
						}
					}
				}
			}
		}
	};
}
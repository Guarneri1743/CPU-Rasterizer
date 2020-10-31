#pragma once
#include <common.hpp>
#include <mesh.hpp>
#include <material.hpp>
#include <transform.hpp>
#include <model.hpp>

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
			if (model != nullptr) {
				auto meshes = model->meshes;
				for (auto iter = meshes.begin(); iter != meshes.end(); iter++) {
					auto mesh = *iter;
					for (auto iiter = mesh.indices.begin(); iiter != mesh.indices.end(); iiter++) {
						uint index = *iiter;
						if (mesh.vertices.size() > index) {

						}
					}
				}
			}
		}
	};
}
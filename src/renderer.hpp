#pragma once
#include <common.hpp>
#include <mesh.hpp>
#include <material.hpp>
#include <transform.hpp>

namespace guarneri {
	class renderer {
	public:
		renderer(mesh* mesh, material* material) {
			this->mesh = mesh;
			this->material = material;
		}

		~renderer() {
			delete mesh;
			delete material;
		}

	public:
		transform transform;
		mesh* mesh;
		material* material;
	};
}
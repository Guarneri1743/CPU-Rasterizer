#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	class Renderer : public Object {
	public:
		Renderer(std::unique_ptr<Model> model) {
			this->target = std::move(model);
		}

		Renderer(const Renderer& other) {
			copy(other);
		}

		~Renderer() { }

	public:
		std::shared_ptr<Model> target;
		Vertex vertices[3];

	public:
		static std::unique_ptr<Renderer> create(std::unique_ptr<Model> model) {
			return std::make_unique<Renderer>(std::move(model));
		}

		static std::unique_ptr<Renderer> create(const Renderer& other) {
			return std::make_unique<Renderer>(other);
		}

		void render() {
			auto view = misc_param.view_matrix;
			auto proj = misc_param.proj_matrix;
			auto model = target->transform.local2world;
			auto pos = target->transform.position;
			auto up = target->transform.up();
			auto forward = target->transform.forward();
			auto right = target->transform.right();
			graphics().draw_coordinates(pos, forward, up, right, view, proj);
			if (target != nullptr) {
				for(auto& m : target->meshes) {
					assert(m->indices.size() % 3 == 0);
					uint32_t idx = 0;
					for(auto& index : m->indices) {
						assert(idx < 3 && index < m->vertices.size());
						vertices[idx] = m->vertices[index];
						idx++;
						if (idx == 3) {
							graphics().draw_primitive(m->material, vertices[0], vertices[1], vertices[2], model, view, proj);
							idx = 0;
						}
					}
				}
			}
		}

		Renderer& operator =(const Renderer& other) {
			copy(other);
			return *this;
		}

		void copy(const Renderer& other) {
			this->target = other.target;
			std::copy(std::begin(other.vertices), std::end(other.vertices), std::begin(vertices));
		}

		std::string str() const {
			std::stringstream ss;
			ss << "Renderer[" << this->id << " Model: " << *target << "]";
			return ss.str();
		}
	};
}
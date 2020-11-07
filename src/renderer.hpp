#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	class renderer : public Object {
	public:
		renderer(std::unique_ptr<Model> Model) {
			this->target = std::move(Model);
		}

		renderer(const renderer& other) {
			copy(other);
		}

		~renderer() { }

	public:
		std::shared_ptr<Model> target;
		vertex vertices[3];

	public:
		static std::unique_ptr<renderer> create(std::unique_ptr<Model> Model) {
			return std::make_unique<renderer>(std::move(Model));
		}

		static std::unique_ptr<renderer> create(const renderer& other) {
			return std::make_unique<renderer>(other);
		}

		void render() {
			auto view = misc_param.view_matrix;
			auto proj = misc_param.proj_matrix;
			auto Model = target->transform.local2world;
			auto pos = target->transform.position();
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
							graphics().draw_primitive(m->mat, vertices[0], vertices[1], vertices[2], Model, view, proj);
							idx = 0;
						}
					}
				}
			}
		}

		renderer& operator =(const renderer& other) {
			copy(other);
			return *this;
		}

		void copy(const renderer& other) {
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
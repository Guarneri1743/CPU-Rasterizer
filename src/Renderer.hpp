#ifndef _RENDERER_
#define _RENDERER_
#include <Guarneri.hpp>

namespace Guarneri {
	class Renderer : public Object {
	public:
		Renderer() {}

		Renderer(std::unique_ptr<Model> model) {
			this->target = std::move(model);
		}

		Renderer(const Renderer& other) {
			copy(other);
		}

		~Renderer() { }

	public:
		std::shared_ptr<Model> target;

	public:
		static std::unique_ptr<Renderer> create(std::unique_ptr<Model>& model) {
			return std::make_unique<Renderer>(std::move(model));
		}

		static std::unique_ptr<Renderer> create(const Renderer& other) {
			return std::make_unique<Renderer>(other);
		}

		virtual Matrix4x4 view_matrix() const {
			return misc_param.view_matrix;
		}

		virtual Matrix4x4 projection_matrix() const {
			return misc_param.proj_matrix;
		}

		virtual Matrix4x4 model_matrix() const {
			return target->transform.local2world;
		}

		virtual void render() const {
			Vertex vertices[3];
			target->material->sync(model_matrix(), view_matrix(), projection_matrix());
			if (target != nullptr) {
				for(auto& m : target->meshes) {
					assert(m->indices.size() % 3 == 0);
					uint32_t idx = 0;
					for(auto& index : m->indices) {
						assert(idx < 3 && index < m->vertices.size());
						vertices[idx] = m->vertices[index];
						idx++;
						if (idx == 3) {
							Graphics().draw(target->material->target_shader, vertices[0], vertices[1], vertices[2], model_matrix(), view_matrix(), projection_matrix());
							idx = 0;
						}
					}
				} 
			}
		}

		virtual void draw_gizmos() const {
			auto view = view_matrix();
			auto proj = projection_matrix();
			auto pos = Vector3::ZERO;
			auto up = Vector3::UP;
			auto forward = Vector3::FORWARD;
			auto right = Vector3::RIGHT;
			Graphics().draw_coordinates(pos, forward, up, right, model_matrix(), view, proj);
		}

		Renderer& operator =(const Renderer& other) {
			copy(other);
			return *this;
		}

		void copy(const Renderer& other) {
			this->target = other.target;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "Renderer[" << this->id << " Model: " << *target << "]";
			return ss.str();
		}
	};
}
#endif
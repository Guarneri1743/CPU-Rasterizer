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

		virtual Matrix4x4 view_matrix(const RenderPass& render_pass) const {
			if (render_pass == RenderPass::SHADOW) {
				return misc_param.main_light.view_matrix();
			}
			return misc_param.view_matrix;
		}

		virtual Matrix4x4 projection_matrix(const RenderPass& render_pass) const {
			if (render_pass == RenderPass::SHADOW) {
				return misc_param.main_light.projection_matrix();
			}
			return misc_param.proj_matrix;
		}

		virtual Matrix4x4 model_matrix() const {
			return target->transform.local2world;
		}

		static void draw_triangle(Shader* shader, const std::vector<Triangle>& triangles, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p) {
			for (auto& tri : triangles) {
				Graphics().draw(shader, tri[0], tri[1], tri[2], m, v, p);
			}
		}

		virtual void render_shadow() const {
			render_internal(RenderPass::SHADOW);
		}

		virtual void render() const {
			render_internal(RenderPass::OBJECT);
		}

		void render_internal(const RenderPass& render_pass) const {
			if (!target->material->cast_shadow && render_pass == RenderPass::SHADOW) {
				return;
			}
#ifdef MULTI_THREAD
			auto thread_size = std::thread::hardware_concurrency();
			ThreadPool tp(thread_size);
#endif
			Vertex vertices[3];
			target->material->set_shadowmap(Graphics().get_shadowmap());
			target->material->sync(model_matrix(), view_matrix(render_pass), projection_matrix(render_pass));
			if (target != nullptr) {
				for(auto& m : target->meshes) {
					assert(m->indices.size() % 3 == 0);
					uint32_t idx = 0;
#ifdef MULTI_THREAD
					std::vector<Triangle> tris;
					auto block_size =  m->indices.size() / thread_size;
					tris.reserve(block_size);
					for (auto& index : m->indices) {
						assert(idx < 3 && index < m->vertices.size());
						vertices[idx] = m->vertices[index];
						idx++;
						if (idx == 3) {
							tris.emplace_back(Triangle(vertices[0], vertices[1], vertices[2]));
							if (tris.size() == block_size) {
								tp.enqueue(draw_triangle, target->material->get_shader(render_pass), tris, model_matrix(), view_matrix(render_pass), projection_matrix(render_pass));
								tris.clear();
							}
							idx = 0;
						}
					}
					tp.enqueue(draw_triangle, target->material->get_shader(render_pass), tris, model_matrix(), view_matrix(render_pass), projection_matrix(render_pass));
#else
					for (auto& index : m->indices) {
						assert(idx < 3 && index < m->vertices.size());
						vertices[idx] = m->vertices[index];
						idx++;
						if (idx == 3) {
							Graphics().draw(target->material->get_shader(render_pass), vertices[0], vertices[1], vertices[2], model_matrix(), view_matrix(), projection_matrix());
							idx = 0;
						}
					}
#endif
				} 
			}
		}

		virtual void draw_gizmos() const {
			auto view = misc_param.view_matrix;
			auto proj = misc_param.proj_matrix;
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
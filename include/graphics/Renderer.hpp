#pragma once
#include <string>
#include <vector>
#include <memory>
#include <stdint.h>
#include "Object.hpp"
#include "tinymath.h"
#include "Define.hpp"

namespace CpuRasterizer
{
	class Model;

	class Renderer : public Object
	{
	public:
		std::shared_ptr<Model> target;

	public:
		Renderer();
		Renderer(std::shared_ptr<Model> model);
		Renderer(const Renderer& other);
		~Renderer();
		static std::unique_ptr<Renderer> create(std::shared_ptr<Model> model);
		static std::unique_ptr<Renderer> create(const Renderer& other);
		virtual tinymath::mat4x4 view_matrix(RenderPass render_pass) const;
		virtual tinymath::mat4x4 projection_matrix(RenderPass render_pass) const;
		virtual tinymath::mat4x4 model_matrix() const;
		virtual void render_shadow() const;
		virtual void render() const;
		virtual void before_render() const {};
		void render_internal(RenderPass render_pass) const;
		virtual void draw_gizmos() const;
		Renderer& operator =(const Renderer& other);
		void copy(const Renderer& other);

	protected:
		void upload_mesh();

	protected:
		std::vector<size_t> vertex_buffer_ids;
		std::vector<size_t> index_buffer_ids;

	protected:
		bool gizmos;
	};
}
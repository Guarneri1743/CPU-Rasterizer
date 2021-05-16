#ifndef _RENDERER_
#define _RENDERER_
#include <string>
#include <vector>
#include <memory>
#include <stdint.h>

#include "Object.hpp"
#include "Matrix4x4.hpp"
#include "Triangle.hpp"
#include "Model.hpp"

namespace Guarneri
{
	class Renderer : public Object
	{
	public:
		std::shared_ptr<Model> target;

	public:
		Renderer();
		Renderer(std::unique_ptr<Model> model);
		Renderer(const Renderer& other);
		~Renderer();
		static std::unique_ptr<Renderer> create(std::unique_ptr<Model>& model);
		static std::unique_ptr<Renderer> create(const Renderer& other);
		virtual Matrix4x4 view_matrix(const RenderPass& render_pass) const;
		virtual Matrix4x4 projection_matrix(const RenderPass& render_pass) const;
		virtual Matrix4x4 model_matrix() const;
		static void draw_triangle(Shader* shader, const std::vector<Triangle>& triangles, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p);
		virtual void render_shadow() const;
		virtual void render() const;
		void render_internal(const RenderPass& render_pass) const;
		virtual void draw_gizmos() const;
		Renderer& operator =(const Renderer& other);
		void copy(const Renderer& other);
		std::string str() const;
	};
}
#endif
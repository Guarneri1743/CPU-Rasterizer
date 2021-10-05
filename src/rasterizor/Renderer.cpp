#include "Renderer.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "GlobalShaderParams.hpp"
#include "Config.h"
#include "Scene.hpp"

namespace CpuRasterizor
{
	Renderer::Renderer() : gizmos(true)
	{}

	Renderer::Renderer(std::shared_ptr<Model> model) : Renderer()
	{
		this->target = model;
	}

	Renderer::Renderer(const Renderer& other)
	{
		copy(other);
	}

	Renderer::~Renderer()
	{}

	std::unique_ptr<Renderer> Renderer::create(std::shared_ptr<Model> model)
	{
		return std::make_unique<Renderer>(model);
	}

	std::unique_ptr<Renderer> Renderer::create(const Renderer& other)
	{
		return std::make_unique<Renderer>(other);
	}

	tinymath::mat4x4 Renderer::view_matrix(const RenderPass& render_pass) const
	{
		if (render_pass == RenderPass::kShadow)
		{
			return INST(GlobalShaderParams).main_light.view_matrix();
		}
		return INST(GlobalShaderParams).view_matrix;
	}

	tinymath::mat4x4 Renderer::projection_matrix(const RenderPass& render_pass) const
	{
		if (render_pass == RenderPass::kShadow)
		{
			return INST(GlobalShaderParams).main_light.projection_matrix();
		}

		return INST(GlobalShaderParams).proj_matrix;
	}

	tinymath::mat4x4 Renderer::model_matrix() const
	{
		return target->transform->world_trs;
	}

	void Renderer::render_shadow() const
	{
		render_internal(RenderPass::kShadow);
	}

	void Renderer::render() const
	{
		render_internal(RenderPass::kObject);
	}

	void Renderer::render_internal(const RenderPass& render_pass) const
	{
		if (!target->material->cast_shadow && render_pass == RenderPass::kShadow)
		{
			return;
		}

		target->material->sync();

		auto m = model_matrix();
		auto v = view_matrix(render_pass);
		auto p = projection_matrix(render_pass);

		target->material->local_properties.set_mat4x4(mat_model, m);
		target->material->local_properties.set_mat4x4(mat_view, v);
		target->material->local_properties.set_mat4x4(mat_projection, p);
		target->material->local_properties.set_mat4x4(mat_vp, p * v);
		target->material->local_properties.set_mat4x4(mat_mvp, p * v * m);

		if (target != nullptr)
		{
			for (auto& mesh : target->meshes)
			{
				assert(mesh.indices.size() % 3 == 0);
				for (size_t idx = 0; idx < mesh.indices.size(); idx += 3)
				{
					auto& v0 = mesh.vertices[mesh.indices[idx]];
					auto& v1 = mesh.vertices[mesh.indices[idx + 1]];
					auto& v2 = mesh.vertices[mesh.indices[idx + 2]];
					INST(GraphicsDevice).submit_draw_command(target->material->get_shader(render_pass), v0, v1, v2);
				}
			}
		}
		INST(GraphicsDevice).fence_draw_commands();
	}

	void Renderer::draw_gizmos() const
	{
		if (!INST(GlobalShaderParams).enable_gizmos)
		{
			return;
		}

		auto view = INST(GlobalShaderParams).view_matrix;
		auto proj = INST(GlobalShaderParams).proj_matrix;
		auto pos = tinymath::kVec3fZero;
		auto up = tinymath::kVec3fUp;
		auto forward = tinymath::kVec3fForward;
		auto right = tinymath::kVec3fRight;
		auto scale = tinymath::scale(model_matrix().get_scale());
		tinymath::mat4x4 m = model_matrix() * tinymath::inverse(scale);
		INST(GraphicsDevice).draw_coordinates(pos, forward, up, right, m, view, proj);
	}

	Renderer& Renderer::operator =(const Renderer& other)
	{
		copy(other);
		return *this;
	}

	void Renderer::copy(const Renderer& other)
	{
		this->target = other.target;
	}
}
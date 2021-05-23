#include "Renderer.hpp"
#include <sstream>
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "GlobalShaderParams.hpp"
#include "Config.h"

namespace Guarneri
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

	Matrix4x4 Renderer::view_matrix(const RenderPass& render_pass) const
	{
		if (render_pass == RenderPass::SHADOW)
		{
			return INST(GlobalShaderParams).main_light.view_matrix();
		}
		return INST(GlobalShaderParams).view_matrix;
	}

	Matrix4x4 Renderer::projection_matrix(const RenderPass& render_pass) const
	{
		if (render_pass == RenderPass::SHADOW)
		{
			return INST(GlobalShaderParams).main_light.projection_matrix();
		}
		return INST(GlobalShaderParams).proj_matrix;
	}

	Matrix4x4 Renderer::model_matrix() const
	{
		return target->transform->world_trs;
	}

	void Renderer::render_shadow() const
	{
		render_internal(RenderPass::SHADOW);
	}

	void Renderer::render() const
	{
		render_internal(RenderPass::OBJECT);
	}

	void Renderer::render_internal(const RenderPass& render_pass) const
	{
		if (!target->material->cast_shadow && render_pass == RenderPass::SHADOW)
		{
			return;
		}

		Vertex vertices[3];
		target->material->set_shadowmap(INST(GraphicsDevice).get_shadowmap());
		target->material->sync(model_matrix(), view_matrix(render_pass), projection_matrix(render_pass));
		if (target != nullptr)
		{
			for (auto& m : target->meshes)
			{
				assert(m.indices.size() % 3 == 0);
				uint32_t idx = 0;
				for (auto& index : m.indices)
				{
					assert(idx < 3 && index < m.vertices.size());
					vertices[idx] = m.vertices[index];
					idx++;
					if (idx == 3)
					{
						INST(GraphicsDevice).draw(target->material->get_shader(render_pass), vertices[0], vertices[1], vertices[2], model_matrix(), view_matrix(render_pass), projection_matrix(render_pass));
						idx = 0;
					}
				}
			}
		}
	}

	void Renderer::draw_gizmos() const
	{
		//todo
		/*auto view = INST(GlobalShaderParams).view_matrix;
		auto proj = INST(GlobalShaderParams).proj_matrix;
		auto pos = Vector3::ZERO;
		auto up = Vector3::UP;
		auto forward = Vector3::FORWARD;
		auto right = Vector3::RIGHT;
		auto scale = Matrix4x4::scale(model_matrix().get_scale());
		Matrix4x4 mat = scale.inverse() * model_matrix();
		INST(GraphicsDevice).draw_coordinates(pos, forward, up, right, mat, view, proj);*/
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

	std::string Renderer::str() const
	{
		std::stringstream ss;
		ss << "Renderer[" << this->id << " Model: " << target->str() << "]";
		return ss.str();
	}
}
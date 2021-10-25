#include "Renderer.hpp"
#include "Singleton.hpp"
#include "GlobalShaderParams.hpp"
#include "Config.h"
#include "Scene.hpp"
#include "Model.hpp"
#include "Material.hpp"
#include "Transform.hpp"
#include "Mesh.hpp"
#include "CGL.h"

namespace CpuRasterizer
{
	Renderer::Renderer() : gizmos(true)
	{}

	Renderer::Renderer(std::shared_ptr<Model> model) : Renderer()
	{
		this->target = model;
		upload_mesh();
	}

	Renderer::Renderer(const Renderer& other)
	{
		copy(other);
	}

	Renderer::~Renderer()
	{}

	void Renderer::upload_mesh()
	{
		if (target != nullptr)
		{
			std::vector<Vertex> vertices;
			std::vector<size_t> indices;
			for (auto& mesh : target->meshes)
			{
				vertices.insert(vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
				indices.insert(indices.end(), mesh.indices.begin(), mesh.indices.end());
			}

			auto vid = cglBindVertexBuffer(vertices);
			auto iid = cglBindIndexBuffer(indices);
			vertex_buffer_ids.emplace_back(vid);
			index_buffer_ids.emplace_back(iid);
		}
	}

	std::unique_ptr<Renderer> Renderer::create(std::shared_ptr<Model> model)
	{
		return std::make_unique<Renderer>(model);
	}

	std::unique_ptr<Renderer> Renderer::create(const Renderer& other)
	{
		return std::make_unique<Renderer>(other);
	}

	tinymath::mat4x4 Renderer::view_matrix(RenderPass render_pass) const
	{
		if (render_pass == RenderPass::kShadow)
		{
			return CpuRasterSharedData.main_light.view_matrix();
		}
		return CpuRasterSharedData.view_matrix;
	}

	tinymath::mat4x4 Renderer::projection_matrix(RenderPass render_pass) const
	{
		if (render_pass == RenderPass::kShadow)
		{
			return CpuRasterSharedData.main_light.projection_matrix();
		}

		return CpuRasterSharedData.proj_matrix;
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

	void Renderer::render_internal(RenderPass render_pass) const
	{
		if (!target->material->cast_shadow && render_pass == RenderPass::kShadow)
		{
			return;
		}

		before_render();

		auto m = model_matrix();
		auto v = view_matrix(render_pass);
		auto p = projection_matrix(render_pass);

		target->material->local_properties.set_mat4x4(mat_model_prop, m);
		target->material->local_properties.set_mat4x4(mat_view_prop, v);
		target->material->local_properties.set_mat4x4(mat_projection_prop, p);
		target->material->local_properties.set_mat4x4(mat_vp_prop, p * v);
		target->material->local_properties.set_mat4x4(mat_mvp_prop, p * v * m);

		target->material->use(render_pass);

		for(size_t i = 0; i < index_buffer_ids.size(); ++i)
		{
			auto iid = index_buffer_ids[i];
			auto vid = vertex_buffer_ids[i];
			cglUseVertexBuffer(vid);
			cglUseIndexBuffer(iid);
			cglDrawPrimitive();
		}
		
		cglFencePrimitives();
	}

	void Renderer::draw_gizmos() const
	{
		if (!CpuRasterSharedData.enable_gizmos)
		{
			return;
		}

		auto view = CpuRasterSharedData.view_matrix;
		auto proj = CpuRasterSharedData.proj_matrix;
		auto pos = tinymath::kVec3fZero;
		auto up = tinymath::kVec3fUp;
		auto forward = tinymath::kVec3fForward;
		auto right = tinymath::kVec3fRight;
		auto scale = tinymath::scale(model_matrix().get_scale());
		tinymath::mat4x4 m = model_matrix() * tinymath::inverse(scale);
		cglDrawCoordinates(pos, forward, up, right, m, view, proj);
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
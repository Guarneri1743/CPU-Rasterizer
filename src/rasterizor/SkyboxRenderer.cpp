#include "SkyboxRenderer.hpp"
#include "PrimitiveFactory.hpp"
#include "SkyboxShader.hpp"
#include "Material.hpp"
#include "Singleton.hpp"
#include "Serialization.hpp"

namespace Guarneri
{
	SkyboxRenderer::SkyboxRenderer() : Renderer()
	{
		gizmos = false;
		Material* mat = new Material();
		Serializer::deserialize("/materials/skybox.material", *mat);
		target = PrimitiveFactory::skybox(std::shared_ptr<Material>(mat));
	}

	SkyboxRenderer::~SkyboxRenderer()
	{}

	tinymath::mat4x4 SkyboxRenderer::view_matrix(const RenderPass& render_pass) const
	{
		UNUSED(render_pass);
		auto view = INST(GlobalShaderParams).view_matrix;
		auto view3x3 = tinymath::mat4x4_to_mat3x3(view);
		auto view4x4 = tinymath::mat3x3_to_mat4x4(view3x3);
		return view4x4;
	}

	tinymath::mat4x4 SkyboxRenderer::projection_matrix(const RenderPass& render_pass) const
	{
		UNUSED(render_pass);
		return INST(GlobalShaderParams).proj_matrix;
	}

	tinymath::mat4x4 SkyboxRenderer::model_matrix() const
	{
		return target->transform->world_trs;
	}

	void SkyboxRenderer::draw_gizmos() const
	{}
}
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

	Matrix4x4 SkyboxRenderer::view_matrix(const RenderPass& render_pass) const
	{
		UNUSED(render_pass);
		auto view = INST(GlobalShaderParams).view_matrix;
		auto view3x3 = Matrix3x3(view);
		auto view4x4 = Matrix4x4(
			Vector4(view3x3.row(0), 0),
			Vector4(view3x3.row(1), 0),
			Vector4(view3x3.row(2), 0),
			Vector4(0, 0, 0, 1));
		return view4x4;
	}

	Matrix4x4 SkyboxRenderer::projection_matrix(const RenderPass& render_pass) const
	{
		UNUSED(render_pass);
		return INST(GlobalShaderParams).proj_matrix;
	}

	Matrix4x4 SkyboxRenderer::model_matrix() const
	{
		return target->transform->world_trs;
	}

	void SkyboxRenderer::draw_gizmos() const
	{}
}
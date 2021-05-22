#ifndef _SKYBOX_RENDERER_
#define _SKYBOX_RENDERER_
#include <memory>
#include "Define.hpp"
#include "Matrix4x4.hpp"
#include "Renderer.hpp"

namespace Guarneri
{
	class SkyboxRenderer : public Renderer
	{
	public:
		SkyboxRenderer();
		~SkyboxRenderer();

	public:
		Matrix4x4 view_matrix(const RenderPass& render_pass) const;
		Matrix4x4 projection_matrix(const RenderPass& render_pass) const;
		Matrix4x4 model_matrix() const;
		void draw_gizmos() const;
	};
}
#endif
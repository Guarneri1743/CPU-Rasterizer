#pragma once
#include <memory>
#include "Define.hpp"
#include "tinymath.h"
#include "Renderer.hpp"

namespace Guarneri
{
	class SkyboxRenderer : public Renderer
	{
	public:
		SkyboxRenderer();
		~SkyboxRenderer();

	public:
		tinymath::mat4x4 view_matrix(const RenderPass& render_pass) const;
		tinymath::mat4x4 projection_matrix(const RenderPass& render_pass) const;
		tinymath::mat4x4 model_matrix() const;
		void draw_gizmos() const;
	};
}
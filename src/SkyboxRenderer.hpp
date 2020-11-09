#ifndef _SKYBOX_RENDERER_
#define _SKYBOX_RENDERER_
#include <Guarneri.hpp>

namespace Guarneri {
	class SkyboxRenderer : public Renderer{
	public:
		SkyboxRenderer() {
			auto shader = std::make_unique<SkyboxShader>();
			auto mat = std::make_unique<Material>(shader);
			mat->ztest_func = CompareFunc::LEQUAL;
			target = PrimitiveFactory::skybox(mat);
		}

	public:
		Matrix4x4 view_matrix() const {
			auto view = misc_param.view_matrix;
			auto view3x3 = Matrix3x3(view);
			auto view4x4 = Matrix4x4(
				Vector4(view3x3.row(0), 0),
				Vector4(view3x3.row(1), 0),
				Vector4(view3x3.row(2), 0),
				Vector4(0, 0, 0, 1));
			return view4x4;
		}

		Matrix4x4 projection_matrix() const {
			return misc_param.proj_matrix;
		}

		Matrix4x4 model_matrix() const {
			return target->transform.local2world;
		}

		void draw_gizmos() { }
	};
}
#endif
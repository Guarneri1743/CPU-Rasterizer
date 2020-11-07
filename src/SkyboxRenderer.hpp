#ifndef _SKYBOX_RENDERER_
#define _SKYBOX_RENDERER_
#include <Guarneri.hpp>

namespace Guarneri {
	class SkyboxRenderer {
	public:
		SkyboxRenderer() {
			auto shader = std::make_unique<SkyboxShader>();
			auto mat = std::make_unique<Material>(shader);
			mat->ztest_mode = ZTest::LEQUAL;
			target = PrimitiveFactory::skybox(mat);
		}

	private:
		std::unique_ptr<Model> target;
		Vertex vertices[3];

	public:
		void render() {
			auto view = misc_param.view_matrix;
			auto view3x3 = Matrix3x3(view);
			auto view4x4 = Matrix4x4(
				Vector4(view3x3.row(0), 0),
				Vector4(view3x3.row(1), 0),
				Vector4(view3x3.row(2), 0),
				Vector4(0, 0, 0, 1));
			auto proj = misc_param.proj_matrix;
		
			auto model = target->transform.local2world;

			if (target != nullptr) {
				for (auto& m : target->meshes) {
					assert(m->indices.size() % 3 == 0);
					uint32_t idx = 0;
					for (auto& index : m->indices) {
						assert(idx < 3 && index < m->vertices.size());
						vertices[idx] = m->vertices[index];
						idx++;
						if (idx == 3) {
							graphics().draw_primitive(target->material, vertices[0], vertices[1], vertices[2], model, view4x4, proj);
							idx = 0;
						}
					}
				}
			}
		}
	};
}
#endif
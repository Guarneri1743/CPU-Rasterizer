#include <CPURasterizer.hpp>

using namespace Guarneri;
using namespace std;

int main()
{
	// initialize rasterizer
	CPURasterizer::prepare(1920, 1080, "CPU-Rasterizer");

	// setup main Light
	Scene demo_scene;
	demo_scene.main_light.position = Vector3(0, 1, 0);
	demo_scene.main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	demo_scene.main_light.diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.intensity = 1.0f;

	// setup Camera
	demo_scene.main_cam->position = Vector3(20.0f, 20.0f, 20.0f);
	demo_scene.main_cam->set_near(0.5f);
	demo_scene.main_cam->lookat(Vector3(0.0f, 0.0f, 0.0f));

	// stencil demo cube
	auto stencil_test_material = Material::create();
	stencil_test_material->stencil_pass_op = StencilOp::REPLACE;
	stencil_test_material->stencil_ref_val = 1;
	stencil_test_material->zwrite_mode = ZWrite::OFF;
	stencil_test_material->stencil_func = CompareFunc::ALWAYS;
	stencil_test_material->double_face = true;
	auto stencil_cube = PrimitiveFactory::cube(stencil_test_material);
	stencil_cube->transform.scale(Vector3(5.0f, 5.0f, 5.0f));
	stencil_cube->transform.translate(Vector3(0.0f, 3.0f, 0.0f));
	std::shared_ptr<Renderer> stencil_cube_renderer = Renderer::create(stencil_cube);
	demo_scene.add(stencil_cube_renderer);

	InputMgr().add_on_update_evt([](void* user_data) {
		auto rdr = *reinterpret_cast<std::shared_ptr<Renderer>*>(user_data);
		if (InputMgr().is_key_down(KeyCode::LEFT)) {
			rdr->target->transform.rotate(Vector3::UP, 5.0f);
		}
		if (InputMgr().is_key_down(KeyCode::RIGHT)) {
			rdr->target->transform.rotate(Vector3::UP, -5.0f);
		}
		if (InputMgr().is_key_down(KeyCode::W)) {
			rdr->target->transform.move_forward(0.5f);
		}
		if (InputMgr().is_key_down(KeyCode::A)) {
			rdr->target->transform.move_left(0.5f);
		}
		if (InputMgr().is_key_down(KeyCode::S)) {
			rdr->target->transform.move_forward(-0.5f);
		}
		if (InputMgr().is_key_down(KeyCode::D)) {
			rdr->target->transform.move_right(0.5f);
		}
		}, & stencil_cube_renderer);

	// backpack
	auto backpack = Model::create(res_path() + "/backpack/backpack.obj", true);
	backpack->material->lighting_param.glossiness = 64.0f;
	backpack->material->stencil_ref_val = 1;
	backpack->material->stencil_func = CompareFunc::EQUAL;
	backpack->transform.scale(Vector3(3.0f, 3.0f, 3.0));
	backpack->transform.translate(Vector3(0.0f, 5.0f, 0.0f));
	demo_scene.add(Renderer::create(backpack));

	// Plane
	auto tex_path = res_path() + "/textures/pavingstones_decorative2_2k_h_1.jpg";
	auto plane_tex = Texture::create(tex_path);
	auto plane_material = Material::create();
	plane_material->transparent = false;
	plane_material->set_texture(albedo_prop, plane_tex);
	auto plane = PrimitiveFactory::plane(plane_material);
	plane->transform.scale(Vector3(10.0f, 1.0f, 10.0f));
	demo_scene.add(Renderer::create(plane));

	InputMgr().add_on_update_evt([](void* user_data) {
		if (InputMgr().is_key_down(KeyCode::R)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.rotate(10.0f, 0.0f);
		}
		if (InputMgr().is_key_down(KeyCode::T)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.rotate(0.0f, 10.0f);
		}
		if (InputMgr().is_key_down(KeyCode::Y)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.rotate(-10.0f, 0.0f);
		}
		if (InputMgr().is_key_down(KeyCode::U)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.rotate(0.0f, -10.0f);
		}
		}, &demo_scene);

	misc_param.shadow_bias = 0.00125f;
	misc_param.color_space = ColorSpace::Gamma;
	misc_param.workflow = PBRWorkFlow::Specular;
	CPURasterizer::kick_off(demo_scene);
	return 0;
}
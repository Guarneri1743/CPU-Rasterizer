#include <Guarneri.hpp>

using namespace Guarneri;
using namespace std;

int main()
{
	// initialize rasterizer
	GuarneriRasterizer::prepare(800, 600, "SoftRasterizer");

	// setup main light
	Scene demo_scene;
	demo_scene.main_light.position = Vector3(0, 1.0f, 0);
	demo_scene.main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	demo_scene.main_light.diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.intensity = 1.0f;

	// setup camera
	demo_scene.main_cam->position = Vector3(20.0f, 20.0f, 20.0f);
	demo_scene.main_cam->set_near(0.5f);
	demo_scene.main_cam->lookat(Vector3(0.0f, 0.0f, 0.0f));

	// plane
	auto tex_path = res_path() + "/textures/pavingstones_decorative2_2k_h_1.jpg";
	auto plane_normal_path = res_path() + "/textures/pavingstones_decorative2_2k_n_1.jpg";
	auto plane_albedo = Texture::create(tex_path);
	auto plane_normal = Texture::create(plane_normal_path);
	auto plane_material = Material::create();
	plane_material->transparent = false;
	plane_material->lighting_param.glossiness = 32.0f;
	plane_material->set_texture(albedo_prop, plane_albedo);
	plane_material->set_texture(normal_prop, plane_normal);
	auto plane = PrimitiveFactory::plane(plane_material);
	plane->transform.scale(Vector3(30.0f, 1.0f, 30.0f));
	std::shared_ptr<Renderer> plane_renderer = Renderer::create(plane);
	demo_scene.add(plane_renderer);

	InputMgr().add_on_update_evt([](void* user_data) {
		auto bp = *reinterpret_cast<std::shared_ptr<Renderer>*>(user_data);
		if (InputMgr().is_key_down(KeyCode::LEFT)) {
			bp->target->transform.rotate(Vector3::UP, 5.0f);
		}
		if (InputMgr().is_key_down(KeyCode::RIGHT)) {
			bp->target->transform.rotate(Vector3::UP, -5.0f);
		}
		if (InputMgr().is_key_down(KeyCode::G)) {
			bp->target->material->lighting_param.glossiness *= 2.0f;
		}
		if (InputMgr().is_key_down(KeyCode::H)) {
			bp->target->material->lighting_param.glossiness /= 2.0f;
		}
		}, & plane_renderer);

	// cube
	auto ca = Texture::create(res_path() + "/textures/misc_Garbage_2k_alb_1.jpg");
	auto mat = Material::create();
	mat->set_texture(albedo_prop, ca);
	mat->double_face = true;
	mat->cast_shadow = true;
	auto opaque_cube = PrimitiveFactory::cube(mat);
	opaque_cube->transform.scale(Vector3(4.0f, 4.0f, 4.0f));
	opaque_cube->transform.translate(Vector3(6.0f, 4.0f, 4.0f));
	std::shared_ptr<Renderer> opaque_cube_rdr = Renderer::create(opaque_cube);
	demo_scene.add(opaque_cube_rdr);

	// transparent cube
	auto cube_albedo = Texture::create(res_path() + "/textures/misc_Garbage_2k_alb_1.jpg");
	auto box_material = Material::create();
	box_material->transparent = true;
	box_material->blend_op = BlendOp::ADD;
	box_material->src_factor = BlendFactor::SRC_ALPHA;
	box_material->dst_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;
	box_material->zwrite_mode = ZWrite::OFF;
	box_material->double_face = true;
	box_material->set_texture(albedo_prop, cube_albedo);
	auto cube = PrimitiveFactory::cube(box_material);
	cube->transform.scale(Vector3(5.0f, 5.0f, 5.0f));
	cube->transform.translate(Vector3(0.0f, 3.0f, 0.0f));
	std::shared_ptr<Renderer> cube_renderer = Renderer::create(cube);
	demo_scene.add(cube_renderer);
	InputMgr().add_on_update_evt([](void* user_data) {
		std::shared_ptr<Renderer> cb = *reinterpret_cast<std::shared_ptr<Renderer>*>(user_data);
		if (InputMgr().is_key_down(KeyCode::W)) {
			cb->target->transform.move_forward(0.2f);
		}
		if (InputMgr().is_key_down(KeyCode::A)) {
			cb->target->transform.move_left(0.2f);
		}
		if (InputMgr().is_key_down(KeyCode::S)) {
			cb->target->transform.move_backward(0.2f);
		}
		if (InputMgr().is_key_down(KeyCode::D)) {
			cb->target->transform.move_right(0.2f);
		}
		if (InputMgr().is_key_down(KeyCode::LEFT)) {
			cb->target->transform.rotate(Vector3::UP, -5.0f);
		}
		if (InputMgr().is_key_down(KeyCode::RIGHT)) {
			cb->target->transform.rotate(Vector3::UP, 5.0f);
		}
	}, & cube_renderer);

	InputMgr().add_on_update_evt([](void* user_data) {
		auto cb = *reinterpret_cast<std::shared_ptr<Renderer>*>(user_data);
		if (InputMgr().is_key_down(KeyCode::B)) {
			cb->target->material->transparent = !cb->target->material->transparent;
			cb->target->material->zwrite_mode = cb->target->material->zwrite_mode == ZWrite::ON ?  ZWrite::OFF : ZWrite::ON;
		}
		}, & cube_renderer);

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
		}, & demo_scene);

	GuarneriRasterizer::kick_off(demo_scene);
	return 0;
}
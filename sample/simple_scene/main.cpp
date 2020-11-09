#include <Guarneri.hpp>

using namespace Guarneri;
using namespace std;

int main()
{
	// initialize rasterizer
	GuarneriRasterizer::prepare(800, 600, "SoftRasterizer");

	// setup main Light
	Scene demo_scene;
	demo_scene.main_light.direction = Vector3(1, 1, 1);
	demo_scene.main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	demo_scene.main_light.diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.intensity = 1.0f;

	// setup Camera
	demo_scene.main_cam->position = Vector3(20.0f, 20.0f, 20.0f);
	demo_scene.main_cam->set_near(0.5f);
	demo_scene.main_cam->set_projection(Projection::ORTHO);
	demo_scene.main_cam->lookat(Vector3(0.0f, 0.0f, 0.0f));

	// backpack
	//auto backpack = Model::create(res_path() + "/backpack/backpack.obj");
	//backpack->material->lighting_param.glossiness = 64.0f;
	//backpack->transform.scale(Vector3(3.0f, 3.0f, 3.0));
	//backpack->transform.translate(Vector3(0.0f, 5.0f, 0.0f));
	//demo_scene.add(Renderer::create(backpack), false);

	// Plane
	auto tex_path = res_path() + "/textures/pavingstones_decorative2_2k_h_1.jpg";
	auto plane_tex = Texture::create(tex_path);
	/*auto Noise = Texture::create(512, 512, TextureFormat::rgba);
	Noise::generate_fractal_image(Noise, 512, 512);*/
	auto plane_material = Material::create();
	plane_material->transparent = false;
	plane_material->set_texture(albedo_prop, plane_tex);
	auto Plane = PrimitiveFactory::Plane(plane_material);
	Plane->transform.scale(Vector3(10.0f, 1.0f, 10.0f));
	demo_scene.add(Renderer::create(Plane), false);

	// transparent cube
	auto cube_tex = Texture::create(res_path() + "/textures/misc_Garbage_2k_alb_1.jpg");
	auto box_material = Material::create();
	box_material->transparent = true;
	box_material->blend_op = BlendOp::ADD;
	box_material->src_factor = BlendFactor::SRC_ALPHA;
	box_material->dst_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;
	box_material->zwrite_mode = ZWrite::OFF;
	box_material->set_texture(albedo_prop, cube_tex);
	auto cube = PrimitiveFactory::cube(box_material);
	cube->transform.scale(Vector3(3.0f, 3.0f, 3.0f));
	cube->transform.translate(Vector3(5.0f, 3.0f, 5.0f));
	std::shared_ptr<Renderer> cube_renderer = Renderer::create(cube);
	demo_scene.add(cube_renderer, true);
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
	}, &cube_renderer);

	InputMgr().add_on_update_evt([](void* user_data) {
		Scene* s = reinterpret_cast<Scene*>(user_data);
		if (InputMgr().is_key_down(KeyCode::R)) {
			s->main_light.direction += Vector3(0.2f, 0.0f, 0.2f);
		}
		if (InputMgr().is_key_down(KeyCode::T)) {
			s->main_light.direction -= Vector3(0.2f, 0.0f, 0.2f);
		}
		}, & demo_scene);

	GuarneriRasterizer::kick_off(demo_scene);
	return 0;
}
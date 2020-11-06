#include <guarneri.hpp>

using namespace guarneri;
using namespace std;

int main()
{
	// initialize rasterizer
	prepare(800, 600, "SoftRasterizer");

	// setup main light
	scene demo_scene;
	demo_scene.main_light.direction = float3(1, 1, 1);
	demo_scene.main_light.ambient = color(0.05f, 0.05f, 0.05f, 1.0f);
	demo_scene.main_light.diffuse = color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.specular = color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.intensity = 1.0f;

	// setup camera
	demo_scene.main_cam->position = float3(20.0f, 20.0f, 20.0f);
	demo_scene.main_cam->lookat(float3(0.0f, 0.0f, 0.0f));

	// backpack
	auto backpack = model::create(res_path() + "/backpack/backpack.obj");
	backpack->transform.scale(float3(3.0f, 3.0f, 3.0));
	backpack->transform.translate(float3(0.0f, 3.0f, 0.0f));
	//demo_scene.add(renderer::create(std::move(backpack)), false);

	// plane
	auto tex_path = res_path() + "/textures/pavingstones_decorative2_2k_h_1.jpg";
	auto plane_tex = texture::create(tex_path);
	/*auto noise = texture::create(512, 512, texture_format::rgba);
	noise::generate_fractal_image(noise, 512, 512);*/
	auto plane_material = material::create();
	plane_material->transparent = false;
	plane_material->set_texture(albedo_prop, plane_tex);
	auto plane = primitive_factory::plane(std::move(plane_material));
	plane->transform.scale(float3(2.0f, 2.0f, 2.0f));
	demo_scene.add(renderer::create(std::move(plane)), false);

	// transparent cube
	auto cube_tex = texture::create(res_path() + "/textures/misc_Garbage_2k_alb_1.jpg");
	auto box_material = material::create();
	box_material->transparent = true;
	box_material->blend_op = blend_operator::add;
	box_material->src_factor = blend_factor::src_alpha;
	box_material->dst_factor = blend_factor::one_minus_src_alpha;
	box_material->zwrite_mode = zwrite::off;
	box_material->set_texture(albedo_prop, cube_tex);
	auto cube = primitive_factory::cube(std::move(box_material));
	cube->transform.scale(float3(3.0f, 3.0f, 3.0f));
	cube->transform.translate(float3(5.0f, 2.0f, 5.0f));
	std::shared_ptr<renderer> cube_renderer = renderer::create(std::move(cube));
	demo_scene.add(cube_renderer, true);
	demo_scene.add_on_update_evt([](void* user_data) {
		std::shared_ptr<renderer> cb = *reinterpret_cast<std::shared_ptr<renderer>*>(user_data);
		if (input_mgr().is_key_down(key_code::W)) {
			cb->target->transform.move_forward(0.2f);
		}
		if (input_mgr().is_key_down(key_code::A)) {
			cb->target->transform.move_left(0.2f);
		}
		if (input_mgr().is_key_down(key_code::S)) {
			cb->target->transform.move_backward(0.2f);
		}
		if (input_mgr().is_key_down(key_code::D)) {
			cb->target->transform.move_right(0.2f);
		}
	}, &cube_renderer);


	kick_off(demo_scene);
	return 0;
}
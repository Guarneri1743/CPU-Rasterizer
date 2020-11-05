#include <guarneri.hpp>

using namespace guarneri;
using namespace std;

int main()
{
	// initialize rasterizer
	prepare(600, 400, "SoftRasterizer");

	scene demo_scene;

	// backpack
	/*auto backpack = model::create(res_path() + "/backpack/backpack.obj");
	objects.push_back(std::move(renderer::create(std::move(backpack))));*/

	// plane
	auto tex_path = res_path() + "/textures/pavingstones_decorative2_2k_h_1.jpg";
	auto plane_tex = texture::create(tex_path);
	/*auto noise = texture::create(512, 512, texture_format::rgba);
	noise::generate_fractal_image(noise, 512, 512);*/
	auto plane_material = material::create();
	plane_material->transparent = false;
	plane_material->set_texture(albedo_prop, plane_tex);
	auto plane = primitive_factory::plane(std::move(plane_material));
	plane->transform.translate(float3::DOWN);
	plane->transform.scale(float3(3.0f, 1.0f, 3.0f));
	cout << "create: " << plane << endl;
	demo_scene.add(renderer::create(std::move(plane)), false);

	// transparent cube
	auto box_material = material::create();
	box_material->transparent = true;
	box_material->blend_op = blend_operator::add;
	box_material->src_factor = blend_factor::src_alpha;
	box_material->dst_factor = blend_factor::one_minus_src_alpha;
	box_material->zwrite_mode = zwrite::off;
	auto cube = primitive_factory::cube(std::move(box_material));
	cout << "create: " << cube << endl;
	demo_scene.add(renderer::create(std::move(cube)), true);
	 
	kick_off(demo_scene);
	return 0;
}
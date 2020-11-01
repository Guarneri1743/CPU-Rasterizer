#include <iostream>
#include <guarneri.hpp>

using namespace guarneri;
using namespace std;

int main()
{
	float angle = 1;

	uint32_t w = 800;
	uint32_t h = 600;

	gdi_window gdiwin(w, h, _T("SoftRasterizerTitle"), _T("SoftRasterizer"));

	auto& device = singleton<render_device>::get();
	device.initialize(gdiwin.framebuffer, w, h);

	float aspect = (float)w / (float)h;
	float3 cam_pos = float3(5.0f, 5.0f, 5.0f);
	float3 box_pos = float3(0.0f, 0.0f, 0.0f);
	singleton<camera>::get().initialize(cam_pos, aspect, 45.0f, 0.5f, 500.0f, camera::projection::perspective);

	// backpack
	model backpack;
	backpack.load_from_file(res_path() + "/backpack/backpack.obj");
	renderer model_renderer(backpack);

	// cube
	material box_material;
	box_material.transparent = true;
	box_material.blend_op = blend_operator::add;
	box_material.src_factor = blend_factor::src_alpha;
	box_material.dst_factor = blend_factor::one_minus_src_alpha;
	box_material.zwrite_mode = zwrite::off;
	model box = model_generator::cube(box_material);
	renderer box_renderer(box);

	// plane
	auto tex_path = res_path() + "/textures/pavingstones_decorative2_2k_h_1.jpg";
	texture plane_tex(tex_path.c_str(), "MainTex");
	texture noise("MainTex", 512, 512, texture_format::rgba);
	noise::generate_fractal_image(noise, 512, 512);
	material plane_material;
	plane_material.transparent = false;
	plane_material.set_texture(albedo_prop, &plane_tex);
	model plane = model_generator::plane(plane_material);
	plane.transform.translate(float3::DOWN);
	plane.transform.scale(float3(3.0f, 1.0f, 3.0f));
	renderer plane_renderer(plane);

	while (gdiwin.is_valid()) {
		if (IS_ON(VK_F3)) device.r_flag = render_flag::wire_frame;
		if (IS_ON(VK_F2)) device.r_flag = render_flag::scanline;
		if (IS_ON(VK_F1)) device.r_flag = render_flag::shaded;
		if (IS_ON(VK_F4)) device.r_flag = render_flag::depth;
		if (IS_ON(VK_F5)) device.r_flag = (render_flag)((int)render_flag::shaded | (int)render_flag::uv);
		if (IS_ON(VK_F6)) device.r_flag = (render_flag)((int)render_flag::shaded | (int)render_flag::vertex_color);
		if (IS_ON(VK_UP)) box_pos.z -= 0.1f;
		if (IS_ON(VK_DOWN)) box_pos.z += 0.1f;
		if (IS_ON(VK_LEFT)) box_pos.x += 0.1f;
		if (IS_ON(VK_RIGHT)) box_pos.x -= 0.1f;
		if (IS_ON('W')) box_pos.y += 0.1f;
		if (IS_ON('S')) box_pos.y -= 0.1f;
		if (IS_ON('R')) angle += 1.0f;

		singleton<camera>::get().set_position(cam_pos);
		singleton<camera>::get().set_target(float3(0.0f, 0.0f, 0.0f));

		device.clear_buffer();

		plane_renderer.render();
		model_renderer.render();
		box_renderer.render();

		gdiwin.flush();

		Sleep(0);
	}

	return 0;
}
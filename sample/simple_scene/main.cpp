#include <iostream>
#include <guarneri.hpp>

using namespace guarneri;
using namespace std;


int main()
{
	uint32_t w = 800;
	uint32_t h = 600;

	gdi_window gdiwin(w, h, _T("SoftRasterizerTitle"), _T("SoftRasterizer"));

	auto& device = grapihcs();
	device.initialize(gdiwin.framebuffer, w, h);

	float aspect = (float)w / (float)h;
	float3 cam_pos = float3(5.0f, 5.0f, 5.0f);
	float3 box_pos = float3(0.0f, 0.0f, 0.0f);
	singleton<camera>::get().initialize(cam_pos, aspect, 45.0f, 0.5f, 500.0f, camera::projection::perspective);

	scene demo_scene;
	std::cout << "start_render" << std::endl;
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

		singleton<camera>::get().set_position(cam_pos);
		singleton<camera>::get().set_target(float3(0.0f, 0.0f, 0.0f));

		device.clear_buffer();

		demo_scene.render();

		gdiwin.flush();

		Sleep(0);
	}

	return 0;
}
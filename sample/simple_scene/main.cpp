#include <iostream>
#include <guarneri.hpp>

using namespace guarneri;
using namespace std;


int main()
{
	uint32_t w = 800;
	uint32_t h = 600;
	window().initialize(w, h, "SoftRasterizer", input_mgr().event_callback);

	float3 cam_pos = float3(5.0f, 5.0f, 5.0f);
	float3 box_pos = float3(0.0f, 0.0f, 0.0f);
	camera::create_main(cam_pos, window().aspect, 45.0f, 0.5f, 500.0f, camera::projection::perspective);

	grapihcs().initialize(window().framebuffer, window().width, window().height);

	scene demo_scene;
	std::cout << "start render" << std::endl;
	while (window().is_valid()) {
		if (IS_ON(VK_F3)) grapihcs().r_flag = render_flag::wire_frame;
		if (IS_ON(VK_F2)) grapihcs().r_flag = render_flag::scanline;
		if (IS_ON(VK_F1)) grapihcs().r_flag = render_flag::shaded;
		if (IS_ON(VK_F4)) grapihcs().r_flag = render_flag::depth;
		if (IS_ON(VK_F5)) grapihcs().r_flag = (render_flag)((int)render_flag::shaded | (int)render_flag::uv);
		if (IS_ON(VK_F6)) grapihcs().r_flag = (render_flag)((int)render_flag::shaded | (int)render_flag::vertex_color);
		if (IS_ON(VK_UP)) box_pos.z -= 0.1f;
		if (IS_ON(VK_DOWN)) box_pos.z += 0.1f;
		if (IS_ON(VK_LEFT)) box_pos.x += 0.1f;
		if (IS_ON(VK_RIGHT)) box_pos.x -= 0.1f;
		if (IS_ON('W')) box_pos.y += 0.1f;
		if (IS_ON('S')) box_pos.y -= 0.1f;
		input_mgr().update();

		camera::main_camera->set_position(cam_pos);
		camera::main_camera->set_target(float3(0.0f, 0.0f, 0.0f));

		grapihcs().clear_buffer();

		demo_scene.update();
		demo_scene.render();

		window().flush();

		Sleep(0);
	}

	return 0;
}
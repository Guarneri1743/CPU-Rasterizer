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
	while (window().is_valid()) {
		input_mgr().update();

		camera::main_camera->set_position(cam_pos);

		grapihcs().clear_buffer();

		demo_scene.update();
		demo_scene.render();

		window().flush();

		Sleep(0);
	}

	return 0;
}
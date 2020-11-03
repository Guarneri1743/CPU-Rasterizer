#include <iostream>
#include <guarneri.hpp>

using namespace guarneri;
using namespace std;


int main()
{
	uint32_t w = 800;
	uint32_t h = 600;
	window().initialize(w, h, "SoftRasterizer", input_mgr().event_callback);
	input_mgr().add_on_key_down_evt([](key_code code, void* data) { if (code == key_code::ESC) window().dispose(); }, nullptr);
	camera::create_main(float3(5.0f, 5.0f, 5.0f), window().aspect, 45.0f, 0.5f, 500.0f, camera::projection::perspective);
	grapihcs().initialize(window().framebuffer, window().width, window().height);

	scene demo_scene;
	while (window().is_valid()) {
		input_mgr().update();

		grapihcs().clear_buffer();
		camera::main_camera->update();
		demo_scene.update();
		demo_scene.render();

		window().flush();

		Sleep(0);
	}

	return 0;
}
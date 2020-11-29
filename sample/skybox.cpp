#include <Guarneri.hpp>

using namespace Guarneri;
using namespace std;

int main()
{
	// initialize rasterizer
	GuarneriRasterizer::prepare(800, 600, "SoftRasterizer");

	// setup main light
	Scene demo_scene;

	// setup camera
	demo_scene.main_cam->position = Vector3(20.0f, 20.0f, 20.0f);
	demo_scene.main_cam->set_near(0.5f);
	demo_scene.main_cam->lookat(Vector3(0.0f, 0.0f, 0.0f));

	// setup skybox
	std::vector<std::string> cubemap_path = {
		res_path() + "/cubemap/lake_right.jpg",
		res_path() + "/cubemap/lake_left.jpg",
		res_path() + "/cubemap/lake_top.jpg",
		res_path() + "/cubemap/lake_bottom.jpg",
		res_path() + "/cubemap/lake_front.jpg",
		res_path() + "/cubemap/lake_back.jpg",
	};
	demo_scene.skybox->load_cubemap(cubemap_path);
	demo_scene.enable_skybox = true;

	GuarneriRasterizer::kick_off(demo_scene);
	return 0;
}
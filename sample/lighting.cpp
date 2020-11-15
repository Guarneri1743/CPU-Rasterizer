#include <Guarneri.hpp>

using namespace Guarneri;
using namespace std;

int main()
{
	// initialize rasterizer
	GuarneriRasterizer::prepare(800, 600, "SoftRasterizer");

	// setup main Light
	Scene demo_scene;
	demo_scene.main_light.direction = Vector3(0, 1.0f, 0);
	demo_scene.main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	demo_scene.main_light.diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.intensity = 3.0f;

	// setup Camera
	demo_scene.main_cam->position = Vector3(10.5f, 15.0f, 13.0f);
	demo_scene.main_cam->set_near(0.5f);
	demo_scene.main_cam->lookat(Vector3(0.0f, 7.0f, 0.0f));

	// backpack
	auto backpack = Model::create(res_path() + "/backpack/backpack.obj");
	backpack->material->lighting_param.glossiness = 32.0f;
	backpack->transform.scale(Vector3(3.0f, 3.0f, 3.0));
	backpack->transform.translate(Vector3(0.0f, 5.0f, 0.0f));
	std::shared_ptr<Renderer> backpack_renderer = Renderer::create(backpack);
	demo_scene.add(backpack_renderer);

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
		}, & backpack_renderer);


	InputMgr().add_on_update_evt([](void* user_data) {
		if (InputMgr().is_key_down(KeyCode::R)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.direction += Vector3(0.2f, 0.0f, 0.2f);
		}
		if (InputMgr().is_key_down(KeyCode::T)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.direction -= Vector3(0.2f, 0.0f, 0.2f);
		}
		}, &demo_scene);

	GuarneriRasterizer::kick_off(demo_scene);
	return 0;
}
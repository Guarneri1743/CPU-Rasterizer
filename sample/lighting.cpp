#include <Guarneri.hpp>

using namespace Guarneri;
using namespace std;

int main()
{
	// initialize rasterizer
	GuarneriRasterizer::prepare(800, 600, "SoftRasterizer");

	// setup main Light
	Scene demo_scene;
	demo_scene.main_light.position = Vector3(-2.0f, 4.0f, -1.0f);
	demo_scene.main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	demo_scene.main_light.diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.intensity = 3.0f;

	// setup Camera
	// demo_scene.main_cam->set_projection(Projection::ORTHO);
	demo_scene.main_cam->position = Vector3(10.5f, 15.0f, 13.0f);
	demo_scene.main_cam->set_near(0.5f);
	demo_scene.main_cam->lookat(Vector3(0.0f, 7.0f, 0.0f));

	// Plane
	auto plane_a_path = res_path() + "/textures/Metal_ScavengerMetal_2k_alb_1.jpg";
	auto plane_n_path = res_path() + "/textures/Metal_ScavengerMetal_2k_n_1.jpg";
	auto plane_s_path = res_path() + "/textures/Metal_ScavengerMetal_2k_g_1.jpg";
	auto plane_ao_path = res_path() + "/textures/Metal_ScavengerMetal_2k_ao_1.jpg";
	auto plane_albedo = Texture::create(plane_a_path);
	auto plane_normal = Texture::create(plane_n_path);
	auto plane_s = Texture::create(plane_s_path);
	auto plane_ao = Texture::create(plane_ao_path);
	plane_albedo->filtering = Filtering::POINT;
	plane_normal->filtering = Filtering::POINT;
	plane_s->filtering = Filtering::POINT;
	plane_ao->filtering = Filtering::POINT;
	auto plane_material = Material::create();
	plane_material->transparent = false;
	plane_material->lighting_param.glossiness = 32.0f;
	plane_material->set_texture(albedo_prop, plane_albedo);
	plane_material->set_texture(normal_prop, plane_normal);
	plane_material->set_texture(specular_prop, plane_s);
	plane_material->set_texture(ao_prop, plane_ao);
	auto Plane = PrimitiveFactory::Plane(plane_material);
	Plane->transform.scale(Vector3(30.0f, 1.0f, 30.0f));
	std::shared_ptr<Renderer> plane_renderer = Renderer::create(Plane);
	demo_scene.add(plane_renderer);

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
			s->main_light.rotate(10.0f, 0.0f);
		}
		if (InputMgr().is_key_down(KeyCode::T)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.rotate(0.0f, 10.0f);
		}
		if (InputMgr().is_key_down(KeyCode::Y)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.rotate(-10.0f, 0.0f);
		}
		if (InputMgr().is_key_down(KeyCode::U)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.rotate(0.0f, -10.0f);
		}
		}, &demo_scene);

	GuarneriRasterizer::kick_off(demo_scene);
	return 0;
}
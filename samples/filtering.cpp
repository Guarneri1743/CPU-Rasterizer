#include "CPURasterizer.hpp"

using namespace Guarneri;
using namespace std;


int main()
{
	// initialize rasterizer
	CPURasterizer::prepare(800, 600, "CPU-Rasterizer");

	// setup main Light
	Scene demo_scene;
	demo_scene.main_light.position = Vector3(0.0f, 8.0f, 0.0f);
	demo_scene.main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	demo_scene.main_light.diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	demo_scene.main_light.intensity = 3.0f;

	// setup Camera
	demo_scene.main_cam->position = Vector3(10.0f, 15.0f, 10.0f) / 1.5f;
	demo_scene.main_cam->set_near(0.5f);
	demo_scene.main_cam->lookat(Vector3(0.0f, 0.0f, 0.0f));

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
	auto plane = PrimitiveFactory::plane(plane_material);
	plane->transform.scale(Vector3(10.0f, 1.0f, 10.0f));
	std::shared_ptr<Renderer> plane_renderer = Renderer::create(plane);
	demo_scene.add(plane_renderer);

	INST(InputManager).add_on_update_evt([](void* user_data) {
		if (INST(InputManager).is_key_down(KeyCode::B)) {
			auto t = *reinterpret_cast<std::shared_ptr<Texture>*>(user_data);
			t->filtering = t->filtering == Filtering::BILINEAR ? Filtering::POINT : Filtering::BILINEAR;
		}
		}, & plane_albedo);

	INST(InputManager).add_on_update_evt([](void* user_data) {
		if (INST(InputManager).is_key_down(KeyCode::B)) {
			auto t = *reinterpret_cast<std::shared_ptr<Texture>*>(user_data);
			t->filtering = t->filtering == Filtering::BILINEAR ? Filtering::POINT : Filtering::BILINEAR;
		}
		}, & plane_normal);

	INST(InputManager).add_on_update_evt([](void* user_data) {
		if (INST(InputManager).is_key_down(KeyCode::B)) {
			auto t = *reinterpret_cast<std::shared_ptr<Texture>*>(user_data);
			t->filtering = t->filtering == Filtering::BILINEAR ? Filtering::POINT : Filtering::BILINEAR;
		}
		}, & plane_s);

	INST(InputManager).add_on_update_evt([](void* user_data) {
		if (INST(InputManager).is_key_down(KeyCode::B)) {
			auto t = *reinterpret_cast<std::shared_ptr<Texture>*>(user_data);
			t->filtering = t->filtering == Filtering::BILINEAR ? Filtering::POINT : Filtering::BILINEAR;
		}
		}, & plane_ao);

	INST(InputManager).add_on_update_evt([](void* user_data) {
		if (INST(InputManager).is_key_down(KeyCode::G)) {
			auto bp = *reinterpret_cast<std::shared_ptr<Renderer>*>(user_data);
			bp->target->material->lighting_param.glossiness *= 2.0f;
		}
		if (INST(InputManager).is_key_down(KeyCode::H)) {
			auto bp = *reinterpret_cast<std::shared_ptr<Renderer>*>(user_data);
			bp->target->material->lighting_param.glossiness /= 2.0f;
		}
		}, &plane_renderer);

	INST(InputManager).add_on_update_evt([](void* user_data) {
		if (INST(InputManager).is_key_down(KeyCode::R)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.rotate(10.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::T)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.rotate(0.0f, 10.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::Y)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.rotate(-10.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::U)) {
			Scene* s = reinterpret_cast<Scene*>(user_data);
			s->main_light.rotate(0.0f, -10.0f);
		}
		}, &demo_scene);

	INST(MiscParameter).workflow = PBRWorkFlow::Specular;
	INST(MiscParameter).color_space = ColorSpace::Gamma;
	CPURasterizer::kick_off(demo_scene);
	return 0;
}
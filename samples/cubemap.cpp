#include "CPURasterizer.hpp"
#include <string>

using namespace Guarneri;
using namespace std;

bool auto_rotation = false;

int main()
{
	// initialize rasterizer
	CPURasterizer::prepare(800, 600, "CPU-Rasterizer");

	// setup main light
	Scene demo_scene;
	DirectionalLight main_light;
	main_light.rotate(230.0f, 0.0f);
	main_light.position = Vector3(0.0f, 8.0f, 0.0f);
	main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	main_light.diffuse = Color(0.9f, 0.9f, 1.0f, 1.0f);
	main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	main_light.intensity = 1.3f;
	demo_scene.set_main_light(main_light);

	// setup camera
	demo_scene.main_cam->position = Vector3(10.78f, 5.0f, -19.0f);
	demo_scene.main_cam->set_near(0.5f);
	demo_scene.main_cam->lookat(Vector3(0.0f, 0.0f, 0.0f));

	// setup skybox
	std::vector<std::string> cubemap_path = {
		RES_PATH + "/cubemap/space_right.png",
		RES_PATH + "/cubemap/space_left.png",
		RES_PATH + "/cubemap/space_top.png",
		RES_PATH + "/cubemap/space_bottom.png",
		RES_PATH + "/cubemap/space_front.png",
		RES_PATH + "/cubemap/space_back.png",
	};

	auto cubemap = CubeMap::create(cubemap_path);
	demo_scene.skybox->target->material->set_cubemap(cubemap_prop, cubemap);
	demo_scene.enable_skybox = true;

	// helmet
	auto helmet = Model::create(RES_PATH + "/pbr_helmet/helmet.obj", false);
	helmet->material->lighting_param.glossiness = 32.0f;
	helmet->material->cast_shadow = true;
	helmet->material->set_cubemap(cubemap_prop, cubemap);
	auto tex_a_path = RES_PATH + "/pbr_helmet/textures/albedo.jpg";
	auto tex_r_path = RES_PATH + "/pbr_helmet/textures/roughness.png";
	auto tex_ao_path = RES_PATH + "/pbr_helmet/textures/ao.jpg";
	auto tex_m_path = RES_PATH + "/pbr_helmet/textures/metallic.png";
	auto tex_albedo = Texture::create(tex_a_path);
	auto tex_r = Texture::create(tex_r_path);
	auto tex_ao = Texture::create(tex_ao_path);
	auto tex_m = Texture::create(tex_m_path);

	tex_albedo->filtering = Filtering::POINT;
	tex_r->filtering = Filtering::POINT;
	tex_ao->filtering = Filtering::POINT;
	tex_m->filtering = Filtering::POINT;

	helmet->material->set_texture(albedo_prop, tex_albedo);
	helmet->material->set_texture(roughness_prop, tex_r);
	helmet->material->set_texture(metallic_prop, tex_m);
	helmet->material->set_texture(ao_prop, tex_ao); 


	helmet->transform.scale(Vector3(0.2f, 0.2f, 0.2f));
	helmet->transform.rotate(Vector3::UP, 300.0f);
	helmet->transform.translate(Vector3(0.0f, -5.0f, 0.0f));
	std::shared_ptr<Renderer> helmet_renderer = Renderer::create(helmet);
	demo_scene.add(helmet_renderer);


	INST(InputManager).add_on_update_evt([](void* user_data)
	{
		auto rdr = *reinterpret_cast<std::shared_ptr<Renderer>*>(user_data);
		if (INST(InputManager).is_key_down(KeyCode::LEFT))
		{
			rdr->target->transform.rotate(Vector3::UP, 5.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::RIGHT))
		{
			rdr->target->transform.rotate(Vector3::UP, -5.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::G))
		{
			rdr->target->material->lighting_param.glossiness *= 2.0f;
		}
		if (INST(InputManager).is_key_down(KeyCode::H))
		{
			rdr->target->material->lighting_param.glossiness /= 2.0f;
		}
	}, & helmet_renderer);

	INST(InputManager).add_on_update_evt([](void* user_data)
	{
		if (INST(InputManager).is_key_down(KeyCode::SPACE))
		{
			auto_rotation = !auto_rotation;
		}

		Scene* s = reinterpret_cast<Scene*>(user_data);
		if (auto_rotation)
		{
			s->main_light.rotate(10.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::R))
		{
			s->main_light.rotate(10.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::T))
		{
			s->main_light.rotate(0.0f, 10.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::Y))
		{
			s->main_light.rotate(-10.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::U))
		{
			s->main_light.rotate(0.0f, -10.0f);
		}
	}, &demo_scene);

	INST(MiscParameter).color_space = ColorSpace::Gamma;
	CPURasterizer::kick_off(demo_scene);
	return 0;
}
#include <Guarneri.hpp>

using namespace Guarneri;
using namespace std;

bool auto_rotation = false;

int main()
{
	// initialize rasterizer
	GuarneriRasterizer::prepare(800, 600, "SoftRasterizer");

	// setup main light
	Scene demo_scene;
	DirectionalLight main_light;
	main_light.position = Vector3(0.0f, 8.0f, 0.0f);
	main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	main_light.diffuse = Color(0.9f, 0.9f, 1.0f, 1.0f);
	main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	main_light.intensity = 1.3f;
	demo_scene.set_main_light(main_light);

	// setup camera
	demo_scene.main_cam->position = Vector3(-17.0f, -4.6f, 23.8f);
	demo_scene.main_cam->set_near(0.5f);
	demo_scene.main_cam->lookat(Vector3(0.0f, 0.0f, 0.0f));

	// setup skybox
	std::vector<std::string> cubemap_path = {
		res_path() + "/cubemap/space_right.png",
		res_path() + "/cubemap/space_left.png",
		res_path() + "/cubemap/space_top.png",
		res_path() + "/cubemap/space_bottom.png",
		res_path() + "/cubemap/space_front.png",
		res_path() + "/cubemap/space_back.png",
	};
	demo_scene.skybox->load_cubemap(cubemap_path);
	demo_scene.enable_skybox = true;

	// helmet
	auto helmet = Model::create(res_path() + "/helmet/helmet.obj", false);
	helmet->material->lighting_param.glossiness = 32.0f;
	helmet->material->cast_shadow = true;

	auto tex_a_path = res_path() + "/helmet/TEX/T1_C.jpg";
	auto tex_n_path = res_path() + "/helmet/TEX/T1_N.jpg";
	auto tex_r_path = res_path() + "/helmet/TEX/T1_R.jpg";
	auto tex_e_path = res_path() + "/helmet/TEX/T1_E.png";
	auto tex_ao_path = res_path() + "/helmet/TEX/T1_AO.jpg";
	auto tex_m_path = res_path() + "/helmet/TEX/T1_M.png";
	auto tex_albedo = Texture::create(tex_a_path);
	auto tex_normal = Texture::create(tex_n_path);
	auto tex_r = Texture::create(tex_r_path);
	auto tex_ao = Texture::create(tex_ao_path);
	auto tex_e = Texture::create(tex_e_path);
	auto tex_m = Texture::create(tex_m_path);

	tex_albedo->filtering = Filtering::POINT;
	tex_normal->filtering = Filtering::POINT;
	tex_r->filtering = Filtering::POINT;
	tex_ao->filtering = Filtering::POINT;
	tex_e->filtering = Filtering::POINT;
	tex_m->filtering = Filtering::POINT;

	helmet->material->set_texture(albedo_prop, tex_albedo);
	helmet->material->set_texture(roughness_prop, tex_r);
	helmet->material->set_texture(metallic_prop, tex_m);
	helmet->material->set_texture(emission_prop, tex_e);
	helmet->material->set_texture(normal_prop, tex_normal);
	helmet->material->set_texture(ao_prop, tex_ao);

	helmet->transform.scale(Vector3(0.02f, 0.02f, 0.02f));
	std::shared_ptr<Renderer> helmet_renderer = Renderer::create(helmet);
	demo_scene.add(helmet_renderer);

	//auto ironman = Model::create(res_path() + "/ironman/LronMan.obj");
	//ironman->material->lighting_param.glossiness = 32.0f;
	//ironman->material->cast_shadow = true;
	//auto tex_a_path = res_path() + "/ironman/textures/LronMan_Diffuse.png";
	//auto tex_n_path = res_path() + "/ironman/textures/LronMan_Normal.png";
	//auto tex_s_path = res_path() + "/ironman/textures/LronMan_Specular.png";
	//auto tex_ao_path = res_path() + "/ironman/textures/LronMan_f0.png";
	//auto tex_albedo = Texture::create(tex_a_path);
	//auto tex_normal = Texture::create(tex_n_path);
	//auto tex_s = Texture::create(tex_s_path);
	//auto tex_ao = Texture::create(tex_ao_path);
	//tex_albedo->filtering = Filtering::POINT;
	//tex_normal->filtering = Filtering::POINT;
	//tex_s->filtering = Filtering::POINT;
	//tex_ao->filtering = Filtering::POINT;

	//ironman->material->set_texture(albedo_prop, tex_albedo);
	//ironman->material->set_texture(specular_prop, tex_s);
	//ironman->material->set_texture(normal_prop, tex_normal);
	//ironman->material->set_texture(ao_prop, tex_ao);

	//std::shared_ptr<Renderer> ironman_renderer = Renderer::create(ironman);
	//demo_scene.add(ironman_renderer);

	InputMgr().add_on_update_evt([](void* user_data)
	{
		if (InputMgr().is_key_down(KeyCode::SPACE))
		{
			auto_rotation = !auto_rotation;
		}

		Scene* s = reinterpret_cast<Scene*>(user_data);
		if (auto_rotation)
		{
			s->main_light.rotate(10.0f, 0.0f);
		}
		if (InputMgr().is_key_down(KeyCode::R))
		{
			s->main_light.rotate(10.0f, 0.0f);
		}
		if (InputMgr().is_key_down(KeyCode::T))
		{
			s->main_light.rotate(0.0f, 10.0f);
		}
		if (InputMgr().is_key_down(KeyCode::Y))
		{
			s->main_light.rotate(-10.0f, 0.0f);
		}
		if (InputMgr().is_key_down(KeyCode::U))
		{
			s->main_light.rotate(0.0f, -10.0f);
		}
	}, &demo_scene);


	GuarneriRasterizer::kick_off(demo_scene);
	return 0;
}
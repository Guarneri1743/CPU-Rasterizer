#include <memory>
#include "CPURasterizer.hpp"

using namespace Guarneri;
using namespace std;

bool auto_rotation = false;

int main()
{
	// initialize rasterizer
	CPURasterizer::prepare(800, 600, "CPU-Rasterizer");

	// setup main Light
	Scene demo_scene;
	DirectionalLight main_light;
	main_light.position = Vector3(0.0f, 8.0f, 0.0f);
	main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	main_light.diffuse = Color(0.9f, 0.9f, 1.0f, 1.0f);
	main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	main_light.intensity = 1.3f;
	demo_scene.set_main_light(main_light);

	PointLight pl1, pl2, pl3;
	pl1.position = Vector3(10.0f, 2.0f, 10.0f);
	pl1.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	pl1.diffuse = Color(1.0f, 0.0f, 0.0f, 1.0f);
	pl1.specular = Color(1.0f, 0.0f, 0.0f, 1.0f);
	pl1.intensity = 3.0f;

	pl2.position = Vector3(-10.0f, 2.0f, -10.0f);
	pl2.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	pl2.diffuse = Color(0.0f, 1.0f, 0.0f, 1.0f);
	pl2.specular = Color(0.0f, 1.0f, 0.0f, 1.0f);
	pl2.intensity = 3.0f;

	pl3.position = Vector3(-10.0f, 2.0f, 10.0f);
	pl3.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	pl3.diffuse = Color(0.0f, 0.0f, 1.0f, 1.0f);
	pl3.specular = Color(0.0f, 0.0f, 1.0f, 1.0f);
	pl3.intensity = 3.0f;

	demo_scene.add_point_light(pl1);
	demo_scene.add_point_light(pl2);
	demo_scene.add_point_light(pl3);

	// setup Camera
	//demo_scene.main_cam->set_projection(Projection::ORTHO);
	demo_scene.main_cam->position = Vector3(10.67f, 20.42f, 9.36f);
	demo_scene.main_cam->set_near(0.5f);
	demo_scene.main_cam->lookat(Vector3(0.0f, 7.0f, 0.0f));

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

	// light cube
	auto ptlmat = Material::create(std::unique_ptr<Shader>(new LightShader()));
	ptlmat->double_face = true;
	auto p1 = PrimitiveFactory::cube(ptlmat);
	p1->transform.scale(Vector3(0.5f, 0.5f, 0.5f));
	p1->transform.translate(pl1.position);
	std::shared_ptr<Renderer> p1r = Renderer::create(p1);
	demo_scene.add(p1r);

	auto pt2mat = Material::create(std::unique_ptr<Shader>(new LightShader()));
	pt2mat->double_face = true;
	auto p2 = PrimitiveFactory::cube(pt2mat);
	p2->transform.scale(Vector3(0.5f, 0.5f, 0.5f));
	p2->transform.translate(pl2.position);
	std::shared_ptr<Renderer> p2r = Renderer::create(p2);
	demo_scene.add(p2r);

	auto pt3mat = Material::create(std::unique_ptr<Shader>(new LightShader()));
	pt3mat->double_face = true;
	auto p3 = PrimitiveFactory::cube(pt3mat);
	p3->transform.scale(Vector3(0.5f, 0.5f, 0.5f));
	p3->transform.translate(pl3.position);
	std::shared_ptr<Renderer> p3r = Renderer::create(p3);
	demo_scene.add(p3r);

	// Plane
	auto plane_a_path = RES_PATH + "/textures/Metal_ScavengerMetal_2k_alb_1.jpg";
	auto plane_n_path = RES_PATH + "/textures/Metal_ScavengerMetal_2k_n_1.jpg";
	auto plane_s_path = RES_PATH + "/textures/Metal_ScavengerMetal_2k_g_1.jpg";
	auto plane_ao_path = RES_PATH + "/textures/Metal_ScavengerMetal_2k_ao_1.jpg";
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
	plane_material->set_texture(albedo_prop, plane_albedo);
	plane_material->set_texture(normal_prop, plane_normal);
	plane_material->set_texture(specular_prop, plane_s);
	plane_material->set_texture(ao_prop, plane_ao);
	plane_material->set_cubemap(cubemap_prop, cubemap);
	auto Plane = PrimitiveFactory::plane(plane_material);
	Plane->transform.scale(Vector3(30.0f, 1.0f, 30.0f));
	std::shared_ptr<Renderer> plane_renderer = Renderer::create(Plane);
	demo_scene.add(plane_renderer);

	// backpack
	auto backpack = Model::create(RES_PATH + "/backpack/backpack.obj", true);
	backpack->material->cast_shadow = true;

	auto bp_a = RES_PATH + "/backpack/diffuse.jpg";
	auto bp_n = RES_PATH + "/backpack/normal.png";
	auto bp_ao = RES_PATH + "/backpack/ao.jpg";
	auto bp_s = RES_PATH + "/backpack/specular.jpg";
	auto bp_r = RES_PATH + "/backpack/roughness.jpg";
	auto tex_bp_a = Texture::create(bp_a);
	auto tex_bp_n = Texture::create(bp_n);
	auto tex_bp_ao = Texture::create(bp_ao);
	auto tex_bp_s = Texture::create(bp_s);
	auto tex_bp_r = Texture::create(bp_r);
	backpack->material->set_texture(albedo_prop, tex_bp_a);
	backpack->material->set_texture(normal_prop, tex_bp_n);
	backpack->material->set_texture(specular_prop, tex_bp_s);
	backpack->material->set_texture(ao_prop, tex_bp_ao);
	backpack->material->set_texture(roughness_prop, tex_bp_r);
	backpack->material->set_cubemap(cubemap_prop, cubemap);

	backpack->transform.scale(Vector3(3.0f, 3.0f, 3.0f));
	backpack->transform.translate(Vector3(0.0f, 8.0f, 0.0f));
	std::shared_ptr<Renderer> backpack_renderer = Renderer::create(backpack);
	demo_scene.add(backpack_renderer);

	INST(InputManager).add_on_update_evt([](void* user_data) {
		auto bp = *reinterpret_cast<std::shared_ptr<Renderer>*>(user_data);
		if (INST(InputManager).is_key_down(KeyCode::LEFT)) {
			bp->target->transform.rotate(Vector3::UP, 5.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::RIGHT)) {
			bp->target->transform.rotate(Vector3::UP, -5.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::G)) {
			bp->target->material->lighting_param.glossiness *= 2.0f;
		}
		if (INST(InputManager).is_key_down(KeyCode::H)) {
			bp->target->material->lighting_param.glossiness /= 2.0f;
		}
		}, & backpack_renderer);


	INST(InputManager).add_on_update_evt([](void* user_data) {
		if (INST(InputManager).is_key_down(KeyCode::SPACE))
		{
			auto_rotation = !auto_rotation;
		}
		
		Scene* s = reinterpret_cast<Scene*>(user_data);
		if (auto_rotation)
		{
			s->main_light.rotate(10.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::R)) {
			s->main_light.rotate(10.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::T)) {
			s->main_light.rotate(0.0f, 10.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::Y)) {
			s->main_light.rotate(-10.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::U)) {
			s->main_light.rotate(0.0f, -10.0f);
		}
		}, &demo_scene);

	INST(MiscParameter).shadow_bias = 0.00125f;
	INST(MiscParameter).color_space = ColorSpace::Gamma;
	INST(MiscParameter).workflow = PBRWorkFlow::Specular;
	CPURasterizer::kick_off(demo_scene);
	return 0;
}
#include <iostream>
#include "Application.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include <filesystem>

#undef GetObject
using namespace Guarneri;
using namespace std;

void matrix_case()
{
	Matrix4x4 m(2, 0, 0, 50, 
				0, 2, 0, 60, 
				0, 0, 2, 70, 
				0, 0, 0, 1);

	Matrix4x4 t(2, 0, 0, 0, 
				0, 2, 0, 0, 
				0, 0, 2, 0, 
				50, 60, 70, 1);

	Matrix4x4 i(0.5f, 0, 0, -25, 
				0, 0.5f, 0, -30, 
				0, 0, 0.5f, -35,
				0, 0, 0, 1);

	assert(m.transpose() == t);
	assert(m.inverse() == i);

	auto local2world = m;
	Vector3 local_pt(125, -70, 30);
	auto world_pt = local2world.transform_point(local_pt);
	assert(world_pt == Vector3(300, -80, 130));
}

void transform_serialization()
{
	Transform t1, t2, t3;
	t1.local_position = Vector3(1, 2, 3);
	t1.local_rotation = Matrix4x4::rotation(Vector3::UP, 45.0f).to_euler();
	t1.local_scale = Vector3(2, 2, 2);
	t2.local_position = Vector3(3, 4, 5);
	t3.local_position = Vector3(4, 5, 6);
	t1.set_parent(&t2); t2.set_parent(&t3);

	rapidjson::Document doc;
	doc.SetObject();
	doc.AddMember("t3", Transform::serialize(doc, t3), doc.GetAllocator());

	std::string path = ("/objects/test_transform.transform");
	std::FILE* fd = fopen(path.c_str(), "w+");
	if (fd != nullptr)
	{
		char write_buffer[256];
		rapidjson::FileWriteStream fs(fd, write_buffer, sizeof(write_buffer));
		rapidjson::PrettyWriter<rapidjson::FileWriteStream> material_writer(fs);
		doc.Accept(material_writer);
		fclose(fd);
	}

	fd = fopen(path.c_str(), "r");
	if (fd != nullptr)
	{
		char read_buffer[256];
		rapidjson::FileReadStream fs(fd, read_buffer, sizeof(read_buffer));
		rapidjson::Document dedoc;
		dedoc.ParseStream(fs);
		auto o = dedoc["t3"].GetObject();
		Transform* det = Transform::deserialize(o);
		assert(det->name == t3.name);
		assert(det->local_position == t3.local_position);
		assert(det->local_rotation == t3.local_rotation);
		assert(det->local_scale == t3.local_scale);
		assert(det->child_count() == t3.child_count());
		fclose(fd);
	}
}

void texture_material_serialization()
{
	std::string bp_a = "/backpack/diffuse.jpg";
	std::string bp_n = "/backpack/normal.png";
	std::string bp_ao = "/backpack/ao.jpg";
	std::string bp_s = "/backpack/specular.jpg";
	std::string bp_r = "/backpack/roughness.jpg";

	auto tex_bp_a = Texture::load_raw(bp_a);
	auto tex_bp_n = Texture::load_raw(bp_n);
	auto tex_bp_ao = Texture::load_raw(bp_ao);
	auto tex_bp_s = Texture::load_raw(bp_s);
	auto tex_bp_r = Texture::load_raw(bp_r);

	Texture::serialize(*tex_bp_a, "/backpack/diffuse.texture");
	Texture::serialize(*tex_bp_n, "/backpack/normal.texture");
	Texture::serialize(*tex_bp_ao, "/backpack/ao.texture");
	Texture::serialize(*tex_bp_s, "/backpack/specular.texture");
	Texture::serialize(*tex_bp_r, "/backpack/roughness.texture");
	Texture::deserialize("/backpack/diffuse.texture", *tex_bp_a);
	Texture::deserialize("/backpack/normal.texture", *tex_bp_n);
	Texture::deserialize("/backpack/ao.texture", *tex_bp_ao);
	Texture::deserialize("/backpack/specular.texture", *tex_bp_s);
	Texture::deserialize("/backpack/roughness.texture", *tex_bp_r);

	auto plane_a_path = "/common_textures/Metal_ScavengerMetal_2k_alb_1.jpg";
	auto plane_n_path = "/common_textures/Metal_ScavengerMetal_2k_n_1.jpg";
	auto plane_s_path = "/common_textures/Metal_ScavengerMetal_2k_g_1.jpg";
	auto plane_ao_path = "/common_textures/Metal_ScavengerMetal_2k_ao_1.jpg";
	auto plane_albedo = Texture::load_raw(plane_a_path);
	auto plane_normal = Texture::load_raw(plane_n_path);
	auto plane_s = Texture::load_raw(plane_s_path);
	auto plane_ao = Texture::load_raw(plane_ao_path);

	Texture::serialize(*plane_albedo, "/common_textures/Metal_ScavengerMetal_2k_alb_1.texture");
	Texture::serialize(*plane_normal, "/common_textures/Metal_ScavengerMetal_2k_n_1.texture");
	Texture::serialize(*plane_s, "/common_textures/Metal_ScavengerMetal_2k_g_1.texture");
	Texture::serialize(*plane_ao, "/common_textures/Metal_ScavengerMetal_2k_ao_1.texture");
	Texture::deserialize("/common_textures/Metal_ScavengerMetal_2k_alb_1.texture", *plane_albedo);
	Texture::deserialize("/common_textures/Metal_ScavengerMetal_2k_n_1.texture", *plane_normal);
	Texture::deserialize("/common_textures/Metal_ScavengerMetal_2k_g_1.texture", *plane_s);
	Texture::deserialize("/common_textures/Metal_ScavengerMetal_2k_ao_1.texture", *plane_ao);


	auto backpac_mat = std::make_shared<Material>();
	backpac_mat->material_name = "back_pack";

	std::vector<std::string> cubemaps = {
		"/cubemaps/space_right.png",
		"/cubemaps/space_left.png",
		"/cubemaps/space_top.png",
		"/cubemaps/space_bottom.png",
		"/cubemaps/space_front.png",
		"/cubemaps/space_back.png",
	};

	std::string cubemap_path = "/cubemaps/space.cubemap";
	auto runtime_cubemap = std::make_shared<CubeMap>("space", cubemaps);
	CubeMap::serialize(*runtime_cubemap, cubemap_path.c_str());

	auto cubemap = std::make_shared<CubeMap>(cubemap_path.c_str());
	backpac_mat->cast_shadow = true;
	backpac_mat->set_texture(albedo_prop, tex_bp_a);
	backpac_mat->set_texture(normal_prop, tex_bp_n);
	backpac_mat->set_texture(specular_prop, tex_bp_s);
	backpac_mat->set_texture(ao_prop, tex_bp_ao);
	backpac_mat->set_texture(roughness_prop, tex_bp_r);
	backpac_mat->set_cubemap(cubemap_prop, cubemap);

	std::string path = "/materials/backpack.material";
	Material::serialize(*backpac_mat, path);

	std::shared_ptr<Material> demat = std::make_shared<Material>();
	Material::deserialize(path, *demat);

	assert(demat->material_name == backpac_mat->material_name);
	assert(demat->blend_op == backpac_mat->blend_op);
	assert(demat->cast_shadow == backpac_mat->cast_shadow);
	assert(demat->color_mask == backpac_mat->color_mask);
	assert(demat->double_face == backpac_mat->double_face);
	assert(demat->blend_op == backpac_mat->blend_op);
	assert(demat->src_factor == backpac_mat->src_factor);
	assert(demat->dst_factor == backpac_mat->dst_factor);
	assert(demat->stencil_func == backpac_mat->stencil_func);
	assert(demat->stencil_pass_op == backpac_mat->stencil_pass_op);
	assert(demat->stencil_fail_op == backpac_mat->stencil_fail_op);
	assert(demat->stencil_read_mask == backpac_mat->stencil_read_mask);
	assert(demat->stencil_ref_val == backpac_mat->stencil_ref_val);
	assert(demat->stencil_write_mask == backpac_mat->stencil_write_mask);
	assert(demat->stencil_zfail_op == backpac_mat->stencil_zfail_op);
	assert(demat->name2int.size() == backpac_mat->name2int.size());
	assert(demat->name2float.size() == backpac_mat->name2float.size());
	assert(demat->name2tex.size() == backpac_mat->name2tex.size());
	assert(demat->name2cubemap.size() == backpac_mat->name2cubemap.size());
	assert(demat->name2float4.size() == backpac_mat->name2float4.size());

	auto light_shader = std::make_shared<LightShader>();
	auto light_material = std::make_shared<Material>("light_material", std::dynamic_pointer_cast<Shader>(light_shader));
	light_material->double_face = true;
	Material::serialize(*light_material, "/materials/light.material");

	auto skybox_shader = std::make_shared<SkyboxShader>();
	auto space_material = std::make_shared<Material>("skybox_material", std::move(skybox_shader));
	space_material->ztest_func = CompareFunc::LEQUAL;
	space_material->set_cubemap(cubemap_prop, cubemap);
	Material::serialize(*space_material, "/materials/space.material");

	auto plane_material = std::make_shared<Material>("plane_material", std::make_shared<Shader>("pbr_shader"));
	plane_material->transparent = false;
	plane_material->set_texture(albedo_prop, plane_albedo);
	plane_material->set_texture(normal_prop, plane_normal);
	plane_material->set_texture(specular_prop, plane_s);
	plane_material->set_texture(ao_prop, plane_ao);
	plane_material->set_cubemap(cubemap_prop, cubemap);
	Material::serialize(*plane_material, "/materials/plane.material");
}

void generate_lighting_scene()
{
	auto plane_material = Material::load_asset("/materials/plane.material");
	auto plane = PrimitiveFactory::plane(plane_material);
	plane->name = "plane";
	plane->transform->set_local_scale(Vector3(30.0f, 1.0f, 30.0f));
	Model::serialize(*plane, "/models/plane.model");

	auto backpack = Model::load_raw("/backpack/backpack.obj", true);
	backpack->name = "backpack";
	backpack->material = Material::load_asset("/materials/backpack.material");
	backpack->transform->set_local_scale(Vector3(3.0f, 3.0f, 3.0f));
	backpack->transform->set_local_position(Vector3(0.0f, 8.0f, 0.0f));
	Model::serialize(*backpack, "/models/backpack.model");

	auto light_material = Material::load_asset("/materials/light.material");
	auto cube = PrimitiveFactory::cube(light_material);
	cube->name = "point_light";
	cube->transform->set_local_scale(Vector3(0.5f, 0.5f, 0.5f));
	cube->transform->set_world_position(Vector3(10.0f, 2.0f, 10.0f));
	Model::serialize(*cube, "/models/light.model");

	// setup main Light
	auto demo_scene = Scene::load_asset("lighting_sample");
	demo_scene->initialize();
	DirectionalLight main_light;
	Matrix4x4 rot = Matrix4x4::from_euler({ 0.388f, -0.629f, -0.67f });
	main_light.yaw = rot.to_euler().y;
	main_light.pitch = rot.to_euler().x;
	main_light.position = Vector3(0.0f, 8.0f, 0.0f);
	main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	main_light.diffuse = Color(0.9f, 0.9f, 1.0f, 1.0f);
	main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	main_light.intensity = 1.3f;
	demo_scene->set_main_light(main_light);

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

	demo_scene->add_point_light(pl1);
	demo_scene->add_point_light(pl2);
	demo_scene->add_point_light(pl3);

	// setup Camera
	//demo_scene->main_cam->set_projection(Projection::ORTHO);
	demo_scene->main_cam->transform->set_world_position(Vector3(8.5f, 19.8f, 15.13f));
	demo_scene->main_cam->set_near(0.5f);
	//demo_scene->main_cam->transform->lookat(Vector3(0.0f, 7.0f, 0.0f));
	demo_scene->main_cam->transform->set_world_angle(-328.0f, -151.7f, 0.0f);

	// setup skybox
	auto space_material = Material::load_asset("/materials/space.material");
	demo_scene->skybox->target->material = space_material;
	demo_scene->enable_skybox = true;

	// light cube
	auto p1 = Model::load_asset("/models/light.model");
	p1->transform->set_local_scale(Vector3(0.5f, 0.5f, 0.5f));
	p1->transform->set_world_position(pl1.position);
	demo_scene->add(p1);

	auto p2 = std::shared_ptr<Model>(p1);
	p2->transform->set_local_scale(Vector3(0.5f, 0.5f, 0.5f));
	p2->transform->set_world_position(pl2.position);
	demo_scene->add(p2);

	auto p3 = std::shared_ptr<Model>(p1);
	p3->transform->set_local_scale(Vector3(0.5f, 0.5f, 0.5f));
	p3->transform->set_world_position(pl3.position);
	demo_scene->add(p3);

	// Plane
	auto p = Model::load_asset("/models/plane.model");
	p->transform->set_local_scale(Vector3(30.0f, 0.0f, 30.0f));
	demo_scene->add(p);

	// backpack
	auto bp = Model::load_asset("/models/backpack.model");
	bp->material = Material::load_asset("/materials/backpack.material");
	bp->transform->set_local_scale(Vector3(3.0f, 3.0f, 3.0f));
	bp->transform->set_world_position(Vector3(0.0f, 8.0f, 0.0f));
	demo_scene->add(bp);

	demo_scene->pcf_on = true;
	demo_scene->enable_shadow = true;
	demo_scene->main_cam->enable_msaa = false;
	demo_scene->shadow_bias = 0.00125f;
	demo_scene->color_space = ColorSpace::Gamma;
	demo_scene->work_flow = PBRWorkFlow::Specular;
	Scene::serialize(*demo_scene, "/scenes/lighting_sample.scene");
}

void generate_stencil_scene()
{
	// setup main Light
	auto demo_scene = Scene::load_asset("stencil_sample"); 
	demo_scene->initialize();
	DirectionalLight main_light;
	main_light.position = Vector3(0.0f, 8.0f, 0.0f);
	main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	main_light.diffuse = Color(0.9f, 0.9f, 1.0f, 1.0f);
	main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	main_light.intensity = 1.3f;
	demo_scene->set_main_light(main_light);

	// setup Camera
	demo_scene->main_cam->transform->set_world_position(Vector3(3.4f, 21.8f, -14.0f));
	demo_scene->main_cam->set_near(0.5f);
	demo_scene->main_cam->transform->set_world_angle(-311.0f, -15.0f, 0.0f);

	// stencil demo cube
	auto stencil_test_material = std::make_shared<Material>();
	stencil_test_material->stencil_pass_op = StencilOp::REPLACE;
	stencil_test_material->stencil_ref_val = 1;
	stencil_test_material->zwrite_mode = ZWrite::OFF;
	stencil_test_material->stencil_func = CompareFunc::ALWAYS;
	stencil_test_material->double_face = true;
	Material::serialize(*stencil_test_material, "/materials/stencil_sample.material");

	auto cube = PrimitiveFactory::cube(Material::load_asset("/materials/stencil_sample.material"));
	cube->name = "stencil_cube";
	cube->transform->set_local_position(Vector3(0.0f, 8.0f, 0.0f));
	cube->transform->set_local_scale(Vector3(5.0f, 5.0f, 5.0f));
	Model::serialize(*cube, "/models/stencil_cube.model");
	demo_scene->add(Model::load_asset("/models/stencil_cube.model"));

	// backpack
	auto backpack = Model::load_raw("/backpack/backpack.obj", true);
	backpack->name = "backpack";
	backpack->material = Material::load_asset("/materials/backpack.material");
	backpack->material->stencil_ref_val = 1;
	backpack->material->stencil_func = CompareFunc::EQUAL;
	Material::serialize(*backpack->material, "/materials/stencil_backpack.material");
	backpack->material = Material::load_asset("/materials/stencil_backpack.material");
	backpack->transform->set_local_scale(Vector3(3.0f, 3.0f, 3.0f));
	backpack->transform->set_local_position(Vector3(0.0f, 8.0f, 0.0f));
	Model::serialize(*backpack, "/models/stencil_backpack.model");

	backpack = Model::load_asset("/models/stencil_backpack.model");
	demo_scene->add(backpack);

	// Plane
	auto plane = Model::load_asset("/models/plane.model");
	demo_scene->add(plane);
	demo_scene->shadow_bias = 0.00125f;
	demo_scene->color_space = ColorSpace::Gamma;
	demo_scene->work_flow = PBRWorkFlow::Specular;
	Scene::serialize(*demo_scene, "/scenes/stencil_sample.scene");
}

void generate_filtering_scene()
{
	// setup main Light
	auto demo_scene = Scene::load_asset("filtering_sample");
	demo_scene->initialize();
	DirectionalLight main_light;
	main_light.position = Vector3(0.0f, 8.0f, 0.0f);
	main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	main_light.diffuse = Color(0.9f, 0.9f, 1.0f, 1.0f);
	main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	main_light.intensity = 1.3f;
	demo_scene->set_main_light(main_light);

	// setup Camera
	demo_scene->main_cam->transform->set_world_position(Vector3(3.4f, 21.8f, -14.0f));
	demo_scene->main_cam->set_near(0.5f);
	demo_scene->main_cam->transform->set_world_angle(-311.0f, -15.0f, 0.0f);

	// Plane
	auto plane = Model::load_asset("/models/plane.model");
	demo_scene->add(plane);
	demo_scene->shadow_bias = 0.00125f;
	demo_scene->color_space = ColorSpace::Gamma;
	demo_scene->work_flow = PBRWorkFlow::Specular;
	Scene::serialize(*demo_scene, "/scenes/filtering_sample.scene");
}

void generate_cubemap_scene()
{
	auto helmet = Model::load_raw("/pbr_helmet/helmet.obj", false);
	helmet->material = std::make_shared<Material>();
	helmet->material->lighting_param.glossiness = 32.0f;
	helmet->material->cast_shadow = true;
	std::string cubemap_path = "/cubemaps/space.cubemap";
	auto cubemap = CubeMap::load_asset(cubemap_path);
	helmet->material->set_cubemap(cubemap_prop, cubemap);
	auto tex_a_path = "/pbr_helmet/textures/albedo.jpg";
	auto tex_r_path = "/pbr_helmet/textures/roughness.png";
	auto tex_ao_path = "/pbr_helmet/textures/ao.jpg";
	auto tex_m_path = "/pbr_helmet/textures/metallic.png";
	auto tex_albedo = Texture::load_raw(tex_a_path);
	auto tex_r = Texture::load_raw(tex_r_path);
	auto tex_ao = Texture::load_raw(tex_ao_path);
	auto tex_m = Texture::load_raw(tex_m_path);
	tex_albedo->filtering = Filtering::POINT;
	tex_r->filtering = Filtering::POINT;
	tex_ao->filtering = Filtering::POINT;
	tex_m->filtering = Filtering::POINT;
	Texture::serialize(*tex_albedo, "/helmet/albedo.texture");
	Texture::serialize(*tex_r, "/helmet/roughness.texture");
	Texture::serialize(*tex_ao, "/helmet/ao.texture");
	Texture::serialize(*tex_m, "/helmet/metallic.texture");
	tex_albedo = Texture::load_asset("/helmet/albedo.texture");
	tex_r = Texture::load_asset("/helmet/roughness.texture");
	tex_ao = Texture::load_asset("/helmet/ao.texture");
	tex_m = Texture::load_asset("/helmet/metallic.texture");

	helmet->material->set_texture(albedo_prop, tex_albedo);
	helmet->material->set_texture(roughness_prop, tex_r);
	helmet->material->set_texture(metallic_prop, tex_m);
	helmet->material->set_texture(ao_prop, tex_ao);
	Material::serialize(*helmet->material, "/materials/helmet.material");
	helmet->material = Material::load_asset("/materials/helmet.material");

	helmet->transform->set_local_scale(Vector3(0.1f, 0.1f, 0.1f));
	helmet->transform->rotate(Vector3::UP, 300.0f);
	helmet->transform->set_local_position(Vector3(0.0f, 5.0f, 0.0f));
	helmet->flip_uv = false;
	Model::serialize(*helmet, "/models/helmet.model");
	helmet = Model::load_asset("/models/helmet.model");

	// setup main Light
	auto demo_scene = Scene::load_asset("cubemap_sample");
	demo_scene->initialize();
	DirectionalLight main_light;
	main_light.position = Vector3(0.0f, 8.0f, 0.0f);
	main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	main_light.diffuse = Color(0.9f, 0.9f, 1.0f, 1.0f);
	main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	main_light.intensity = 1.3f;
	demo_scene->set_main_light(main_light);

	// setup Camera
	demo_scene->main_cam->transform->set_world_position(Vector3(3.4f, 21.8f, -14.0f));
	demo_scene->main_cam->set_near(0.5f);
	demo_scene->main_cam->transform->set_world_angle(-311.0f, -15.0f, 0.0f);

	// Plane
	auto plane = Model::load_asset("/models/plane.model");
	demo_scene->shadow_bias = 0.00125f;
	demo_scene->color_space = ColorSpace::Linear;
	demo_scene->work_flow = PBRWorkFlow::Metallic;

	demo_scene->add(plane);
	demo_scene->add(helmet);

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

	// light cube
	auto p1 = Model::load_asset("/models/light.model");
	p1->transform->set_local_scale(Vector3(0.5f, 0.5f, 0.5f));
	p1->transform->set_world_position(pl1.position);
	demo_scene->add(p1);

	auto p2 = std::shared_ptr<Model>(p1);
	p2->transform->set_local_scale(Vector3(0.5f, 0.5f, 0.5f));
	p2->transform->set_world_position(pl2.position);
	demo_scene->add(p2);

	auto p3 = std::shared_ptr<Model>(p1);
	p3->transform->set_local_scale(Vector3(0.5f, 0.5f, 0.5f));
	p3->transform->set_world_position(pl3.position);
	demo_scene->add(p3);

	demo_scene->add_point_light(pl1);
	demo_scene->add_point_light(pl2);
	demo_scene->add_point_light(pl3);

	demo_scene->enable_shadow = true;
	demo_scene->enable_skybox = true;
	demo_scene->pcf_on = true;
	demo_scene->shadow_bias = 0.00125f;
	demo_scene->color_space = ColorSpace::Linear;
	demo_scene->work_flow = PBRWorkFlow::Metallic;

	Scene::serialize(*demo_scene, "/scenes/cubemap_sample.scene");
}

void generate_blending_scene()
{
	// cube
	auto ca = Texture::load_raw("/textures/misc_Garbage_2k_alb_1.jpg");
	auto mat = std::make_shared<Material>();
	mat->set_texture(albedo_prop, ca);
	mat->double_face = true;
	mat->cast_shadow = true;
	Material::serialize(*mat, "/materials/opaque_cube.material");
	mat = Material::load_asset("/materials/opaque_cube.material");
	auto opaque_cube = PrimitiveFactory::cube(mat);
	opaque_cube->transform->set_local_scale(Vector3(4.0f, 4.0f, 4.0f));
	opaque_cube->transform->set_local_position(Vector3(6.0f, 4.0f, 4.0f));
	Model::serialize(*opaque_cube, "/models/opaque_cube.model");
	opaque_cube = Model::load_asset("/models/opaque_cube.model");

	// transparent cube
	auto cube_albedo = Texture::load_asset("/common_textures/Metal_ScavengerMetal_2k_alb_1.texture");
	auto box_material = std::make_shared<Material>();
	box_material->transparent = true;
	box_material->blend_op = BlendOp::ADD;
	box_material->src_factor = BlendFactor::SRC_ALPHA;
	box_material->dst_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;
	box_material->zwrite_mode = ZWrite::OFF;
	box_material->double_face = true;
	box_material->set_texture(albedo_prop, cube_albedo);
	Material::serialize(*box_material, "/materials/transparent_cube.material");
	box_material = Material::load_asset("/materials/transparent_cube.material");
	auto transparent_cube = PrimitiveFactory::cube(box_material);
	transparent_cube->transform->set_local_scale(Vector3(5.0f, 5.0f, 5.0f));
	transparent_cube->transform->set_local_position(Vector3(0.0f, 3.0f, 0.0f));
	Model::serialize(*transparent_cube, "/models/transparent_cube.model");
	transparent_cube = Model::load_asset("/models/transparent_cube.model");

	// setup main Light
	auto demo_scene = Scene::load_asset("blending_sample");
	demo_scene->initialize();
	DirectionalLight main_light;
	main_light.position = Vector3(0.0f, 8.0f, 0.0f);
	main_light.ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);
	main_light.diffuse = Color(0.9f, 0.9f, 1.0f, 1.0f);
	main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	main_light.intensity = 1.3f;
	demo_scene->set_main_light(main_light);

	// setup Camera
	demo_scene->main_cam->transform->set_world_position(Vector3(3.4f, 21.8f, -14.0f));
	demo_scene->main_cam->set_near(0.5f);
	demo_scene->main_cam->transform->set_world_angle(-311.0f, -15.0f, 0.0f);

	// Plane
	auto plane = Model::load_asset("/models/plane.model");

	demo_scene->add(plane);
	demo_scene->add(transparent_cube);
	demo_scene->add(opaque_cube);
	demo_scene->enable_shadow = true;
	demo_scene->enable_skybox = true;
	demo_scene->pcf_on = true;
	demo_scene->shadow_bias = 0.00125f;
	demo_scene->color_space = ColorSpace::Gamma;
	demo_scene->work_flow = PBRWorkFlow::Specular;

	Scene::serialize(*demo_scene, "/scenes/blending_sample.scene");
}

void generate_model(std::string name, std::string path)
{
	auto model = Model::load_raw(path, false);
	model->name = name;
	Model::serialize(*model, "/models/" + name + ".model");
}

void generate_models()
{
	generate_model("bunny", "/misc/bunny.obj");
	generate_model("bunny_hq", "/misc/bunny_hq.ply");
	generate_model("teapot", "/misc/teapot.obj");
	generate_model("dragon", "/misc/dragon.ply");
	generate_model("buddha", "/misc/buddha.ply");
}

int main()
{
	matrix_case();
	transform_serialization();
	texture_material_serialization();
	generate_lighting_scene();
	generate_stencil_scene();
	generate_filtering_scene();
	generate_cubemap_scene();
	generate_blending_scene();
	generate_models();
}

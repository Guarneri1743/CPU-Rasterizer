#include <iostream>
#include "CPURasterizer.hpp"
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

	Matrix4x4 rot = Matrix4x4::rotation(Vector3::UP, 70.0f);
	Vector3 euler = rot.to_euler();
	Matrix4x4 fe = Matrix4x4::from_euler(euler);
	assert(rot == Matrix4x4::from_euler(euler));
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

	auto plane_a_path = "/textures/Metal_ScavengerMetal_2k_alb_1.jpg";
	auto plane_n_path = "/textures/Metal_ScavengerMetal_2k_n_1.jpg";
	auto plane_s_path = "/textures/Metal_ScavengerMetal_2k_g_1.jpg";
	auto plane_ao_path = "/textures/Metal_ScavengerMetal_2k_ao_1.jpg";
	auto plane_albedo = Texture::load_raw(plane_a_path);
	auto plane_normal = Texture::load_raw(plane_n_path);
	auto plane_s = Texture::load_raw(plane_s_path);
	auto plane_ao = Texture::load_raw(plane_ao_path);

	Texture::serialize(*plane_albedo, "/textures/Metal_ScavengerMetal_2k_alb_1.texture");
	Texture::serialize(*plane_normal, "/textures/Metal_ScavengerMetal_2k_n_1.texture");
	Texture::serialize(*plane_s, "/textures/Metal_ScavengerMetal_2k_g_1.texture");
	Texture::serialize(*plane_ao, "/textures/Metal_ScavengerMetal_2k_ao_1.texture");
	Texture::deserialize("/textures/Metal_ScavengerMetal_2k_alb_1.texture", *plane_albedo);
	Texture::deserialize("/textures/Metal_ScavengerMetal_2k_n_1.texture", *plane_normal);
	Texture::deserialize("/textures/Metal_ScavengerMetal_2k_g_1.texture", *plane_s);
	Texture::deserialize("/textures/Metal_ScavengerMetal_2k_ao_1.texture", *plane_ao);


	auto backpac_mat = Material::create();
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
	auto runtime_cubemap = CubeMap::create("space", cubemaps);
	CubeMap::serialize(*runtime_cubemap, cubemap_path.c_str());

	auto cubemap = CubeMap::create(cubemap_path.c_str());
	backpac_mat->cast_shadow = true;
	backpac_mat->set_texture(albedo_prop, tex_bp_a);
	backpac_mat->set_texture(normal_prop, tex_bp_n);
	backpac_mat->set_texture(specular_prop, tex_bp_s);
	backpac_mat->set_texture(ao_prop, tex_bp_ao);
	backpac_mat->set_texture(roughness_prop, tex_bp_r);
	backpac_mat->set_cubemap(cubemap_prop, cubemap);

	std::string path = "/materials/backpack.material";
	Material::serialize(*backpac_mat, path);

	std::shared_ptr<Material> demat = Material::create();
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
	auto light_material = Material::create("light_material", std::dynamic_pointer_cast<Shader>(light_shader));
	light_material->double_face = true;
	Material::serialize(*light_material, "/materials/light.material");

	auto skybox_shader = std::make_shared<SkyboxShader>();
	auto space_material = Material::create("skybox_material", std::move(skybox_shader));
	space_material->ztest_func = CompareFunc::LEQUAL;
	space_material->set_cubemap(cubemap_prop, cubemap);
	Material::serialize(*space_material, "/materials/space.material");

	auto plane_material = Material::create("plane_material", std::make_shared<Shader>("pbr_shader"));
	plane_material->transparent = false;
	plane_material->set_texture(albedo_prop, plane_albedo);
	plane_material->set_texture(normal_prop, plane_normal);
	plane_material->set_texture(specular_prop, plane_s);
	plane_material->set_texture(ao_prop, plane_ao);
	plane_material->set_cubemap(cubemap_prop, cubemap);
	Material::serialize(*plane_material, "/materials/plane.material");
}

void model_serialization()
{
	auto plane_material = Material::create("/materials/plane.material");
	auto plane = PrimitiveFactory::plane(plane_material);
	plane->name = "plane";
	plane->transform->set_local_scale(Vector3(30.0f, 1.0f, 30.0f));
	Model::serialize(*plane, "/models/plane.model");

	auto backpack = Model::load_raw("/backpack/backpack.obj", true);
	backpack->name = "backpack";
	backpack->material = Material::create("/materials/backpack.material");
	backpack->transform->set_local_scale(Vector3(3.0f, 3.0f, 3.0f));
	backpack->transform->set_world_position(Vector3(0.0f, 8.0f, 0.0f));
	Model::serialize(*backpack, "/models/backpack.model");

	auto light_material = Material::create("/materials/light.material");
	auto p1 = PrimitiveFactory::cube(light_material);
	p1->transform->set_local_scale(Vector3(0.5f, 0.5f, 0.5f));
	p1->transform->set_world_position(Vector3(10.0f, 2.0f, 10.0f));
	Model::serialize(*p1, "/models/light.model");
}

void scene_serialization()
{
	// setup main Light
	auto demo_scene = Scene::create("lighting_sample");
	DirectionalLight main_light;
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
	demo_scene->main_cam->transform->set_world_position(Vector3(10.67f, 20.42f, 9.36f));
	demo_scene->main_cam->set_near(0.5f);
	//demo_scene->main_cam->transform->lookat(Vector3(0.0f, 7.0f, 0.0f));
	demo_scene->main_cam->transform->set_world_angle(-313.0f, -113.0f, 0.0f);

	// setup skybox
	auto space_material = Material::create("/materials/space.material");
	demo_scene->skybox->target->material = space_material;
	demo_scene->enable_skybox = true;

	// light cube
	auto p1 = Model::create("/models/light.model");
	p1->transform->set_local_scale(Vector3(0.5f, 0.5f, 0.5f));
	p1->transform->set_world_position(pl1.position);
	demo_scene->add(p1);

	auto p2 = Model::create(*p1);
	p2->transform->set_local_scale(Vector3(0.5f, 0.5f, 0.5f));
	p2->transform->set_world_position(pl2.position);
	demo_scene->add(p2);

	auto p3 = Model::create(*p1);
	p3->transform->set_local_scale(Vector3(0.5f, 0.5f, 0.5f));
	p3->transform->set_world_position(pl3.position);
	demo_scene->add(p3);

	// Plane
	auto plane = Model::create("/models/plane.model");
	plane->transform->set_local_scale(Vector3(30.0f, 1.0f, 30.0f));
	demo_scene->add(plane);

	// backpack
	auto backpack = Model::create("/models/backpack.model");
	backpack->material = Material::create("/materials/backpack.material");
	backpack->transform->set_local_scale(Vector3(3.0f, 3.0f, 3.0f));
	backpack->transform->set_world_position(Vector3(0.0f, 8.0f, 0.0f));
	demo_scene->add(backpack);

	Scene::serialize(*demo_scene, "/scenes/lighting_sample.scene");
}

int main()
{
	CPURasterizer::prepare(800, 700, "test");
	matrix_case();
	transform_serialization();
	texture_material_serialization();
	model_serialization();
	scene_serialization();
}

#include "Scene.hpp"
#include "Marcos.h"
#include "GraphicsDevice.hpp"
#include "Singleton.hpp"
#include "GlobalShaderParams.hpp"
#include "InputManager.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Matrix4x4.hpp"
#include "Color.hpp"
#include "Config.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include <filesystem>
#include <iostream>
#include "Utility.hpp"
#include "Logger.hpp"

#undef near
#undef far
#undef GetObject

namespace Guarneri
{
	std::unique_ptr<Scene> Scene::current_scene ;

	Scene::Scene(std::string name)
	{
		selection = nullptr;
		this->name = name;
		enable_skybox = false;
		main_light.intensity = 1.0f;
		main_light.diffuse = Color(1.0f, 0.8f, 0.8f, 1.0f);
		main_light.ambient = Color(0.1f, 0.05f, 0.2f, 1.0f);
		main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
		main_light.position = Vector3(1.0f, 1.0f, 1.0f);
		skybox = std::make_unique<SkyboxRenderer>();
	}

	Scene::~Scene()
	{
		selection = nullptr;
		LOG("destruct scene: {}", name);
	}

	void Scene::initialize()
	{
		float aspect = (float)INST(GraphicsDevice).get_width() / INST(GraphicsDevice).get_height();
		main_cam = Camera::create(Vector3(5.0f, 5.0f, 5.0f), aspect, 45.0f, 0.5f, 100.0f);
		main_cam->transform->set_world_angle(33.0f, -330.0f, 0.0f);
		Camera::set_main_camera(main_cam.get());
		debug_cam_distance = 6.0f;
		debug_world_cam_distance = 8.0f;
		debug_cam = std::move(Camera::create(main_cam->transform->world_position() + Vector3(1.0f, 1.0f, -1.0f) * debug_cam_distance, aspect, 45.0f, 0.5f, 10.0f));
		world_debug_cam = std::move(Camera::create(Vector3(1.0f, 1.0f, -1.0f) * debug_world_cam_distance, aspect, 45.0f, 0.5f, 10.0f));

		INST(InputManager).add_on_mouse_move_evt([](Vector2 prev, Vector2 pos, void* data)
		{
			if (INST(InputManager).is_mouse_down(MouseButton::RIGHT))
			{
				Scene* s = reinterpret_cast<Scene*>(data);
				Vector2 offset = (pos - prev) * CAMERA_ROTATE_SPEED;
				s->main_cam->transform->rotate(offset.y, offset.x, 0.0f);
			}
			if (INST(InputManager).is_mouse_down(MouseButton::MIDDLE))
			{
				Vector2 offset = (pos - prev) * CAMERA_ROTATE_SPEED;
				Scene* s = reinterpret_cast<Scene*>(data);
				s->main_cam->transform->move_left(offset.x);
				s->main_cam->transform->move_ascend(offset.y);
			}
		}, this);

		INST(InputManager).add_on_mouse_wheel_rolling_evt([](MouseWheel rolling, void* data)
		{
			Scene* s = reinterpret_cast<Scene*>(data);
			if (rolling == MouseWheel::UP)
			{
				s->main_cam->transform->move_forward(CAMERA_ZOOM_SPEED);
			}
			else
			{
				s->main_cam->transform->move_backward(CAMERA_ZOOM_SPEED);
			}
		}, this);
	}


	void Scene::add(std::shared_ptr<Model> model)
	{
		if (model->material->transparent)
		{
			transparent_objects.emplace_back(std::make_unique<Renderer>(model));
		}
		else
		{
			objects.emplace_back(std::make_unique<Renderer>(model));
		}
	}

	void Scene::update()
	{
		INST(GlobalShaderParams).cam_far = main_cam->far;
		INST(GlobalShaderParams).cam_near = main_cam->near;
		INST(GlobalShaderParams).view_matrix = main_cam->view_matrix();
		INST(GlobalShaderParams).proj_matrix = main_cam->projection_matrix();
		INST(GlobalShaderParams).main_light = main_light;
		INST(GlobalShaderParams).point_lights = point_lights;
		INST(GlobalShaderParams).camera_pos = main_cam->transform->world_position();
		if (INST(InputManager).is_key_down(KeyCode::W)) {
			main_cam->transform->move_forward(CAMERA_MOVE_SPEED);
		}
		if (INST(InputManager).is_key_down(KeyCode::A)) {
			main_cam->transform->move_left(CAMERA_MOVE_SPEED);
		}
		if (INST(InputManager).is_key_down(KeyCode::S)) {
			main_cam->transform->move_backward(CAMERA_MOVE_SPEED);
		}
		if (INST(InputManager).is_key_down(KeyCode::D)) {
			main_cam->transform->move_right(CAMERA_MOVE_SPEED);
		}
		if (INST(InputManager).is_key_down(KeyCode::Q)) {
			main_cam->transform->move_ascend(CAMERA_MOVE_SPEED);
		}
		if (INST(InputManager).is_key_down(KeyCode::E)) {
			main_cam->transform->move_descend(CAMERA_MOVE_SPEED);
		}
		if (INST(InputManager).is_mouse_down(MouseButton::MIDDLE))
		{
			main_cam->transform->move_ascend(CAMERA_MOVE_SPEED);
		}
		if (INST(InputManager).is_mouse_down(MouseButton::MIDDLE))
		{
			main_cam->transform->move_descend(CAMERA_MOVE_SPEED);
		}

		if (INST(InputManager).is_key_down(KeyCode::DELETION))
		{
			int remove_idx = -1;
			if (selection != nullptr)
			{
				int idx = 0;
				for (auto& obj : objects)
				{
					if (obj->target->transform.get() == selection)
					{
						remove_idx = idx;
					}
					idx++;
				}

				if (remove_idx != -1)
				{
					LOG("remove: {}", objects[remove_idx]->target->name);
					objects.erase(objects.begin() + remove_idx);
					remove_idx = -1;
				}

				idx = 0;
				for (auto& obj : transparent_objects)
				{
					if (obj->target->transform.get() == selection)
					{
						remove_idx = idx;
					}
					idx++;
				}

				if (remove_idx != -1)
				{
					LOG("remove: {}", transparent_objects[remove_idx]->target->name);
					transparent_objects.erase(transparent_objects.begin() + remove_idx);
					remove_idx = -1;
				}
				selection = nullptr;
			}
		}
	}

	void Scene::draw_camera_coords()
	{
		Vector2 offset = Vector2(-(INST(GraphicsDevice).get_width() / 2.0f - 50.0f), -(INST(GraphicsDevice).get_height() / 2.0f - 50.0f));
		Vector3 pos = main_cam->transform->world_position();
		Vector3 forward = main_cam->transform->forward();
		Vector3 right = main_cam->transform->right();
		Vector3 up = main_cam->transform->up();
		INST(GraphicsDevice).draw_coordinates(pos, forward, up, right, debug_cam->view_matrix(), debug_cam->projection_matrix(), offset);
		debug_cam->transform->set_world_position((main_cam->transform->world_position() + Vector3(1.0f, 1.0f, -1.0f) * debug_cam_distance));
		debug_cam->transform->lookat(main_cam->transform->world_position());
	}

	void Scene::draw_world_coords()
	{
		Vector2 offset = Vector2(-(INST(GraphicsDevice).get_width() / 2.0f - 150.0f), -(INST(GraphicsDevice).get_height() / 2.0f - 150.0f));
		INST(GraphicsDevice).draw_coordinates(Vector3::ZERO, Vector3::FORWARD * 3.0f, Vector3::UP * 3.0f, Vector3::RIGHT * 3.0f, world_debug_cam->view_matrix(), world_debug_cam->projection_matrix(), offset);
		INST(GraphicsDevice).draw_coordinates(Vector3::ZERO, Vector3::FORWARD * 3.0f, Vector3::UP * 3.0f, Vector3::RIGHT * 3.0f, main_cam->view_matrix(), main_cam->projection_matrix());

		Vector3 pos = main_cam->transform->world_position();
		Vector3 forward = main_cam->transform->forward();
		Vector3 right = main_cam->transform->right();
		Vector3 up = main_cam->transform->up();
		INST(GraphicsDevice).draw_coordinates(pos, forward, up, right, world_debug_cam->view_matrix(), world_debug_cam->projection_matrix(), offset);

		world_debug_cam->transform->set_world_position((Vector3(1.0f, 1.0f, -1.0f) * debug_world_cam_distance));
		world_debug_cam->transform->lookat(Vector3::ZERO);
	}

	void Scene::set_main_light(const DirectionalLight& light)
	{
		main_light = light;
	}

	void Scene::add_point_light(const PointLight& light)
	{
		point_lights.emplace_back(light);
	}

	void Scene::render()
	{
		INST(GraphicsDevice).clear_buffer(BufferFlag::COLOR | BufferFlag::DEPTH | BufferFlag::STENCIL);
		if (INST(GlobalShaderParams).enable_shadow)
		{
			render_shadow();
			INST(GraphicsDevice).present();
		}
		render_objects();
		INST(GraphicsDevice).present();
		draw_gizmos();
	}

	void Scene::render_shadow()
	{
		if ((INST(GlobalShaderParams).render_flag & RenderFlag::DEPTH) != RenderFlag::DISABLE)
		{
			return;
		}

		for (auto& obj : objects)
		{
			obj->render_shadow();
		}
	}

	void Scene::render_objects()
	{
		INST(GraphicsDevice).multi_thread = true;
		INST(GraphicsDevice).tile_based = true;

		if ((INST(GlobalShaderParams).render_flag & RenderFlag::SHADOWMAP) != RenderFlag::DISABLE)
		{
			return;
		}

		bool enable_frustum_culling = (INST(GlobalShaderParams).culling_clipping_flag & CullingAndClippingFlag::APP_FRUSTUM_CULLING) != CullingAndClippingFlag::DISABLE;
		if (enable_frustum_culling)
		{
			// todo: CPU Frustum Culling
		}

		for (auto& obj : objects)
		{
			obj->render();
		}

		if (enable_skybox && INST(GlobalShaderParams).enable_ibl)
		{
			// todo: fix tile based skybox 
			INST(GraphicsDevice).tile_based = false;
			skybox->render();
			INST(GraphicsDevice).tile_based = true;
		}

		// todo: OIT
		for (auto& obj : transparent_objects)
		{
			obj->render();
		}
	}

	void Scene::draw_gizmos()
	{
		for (auto& obj : objects)
		{
			obj->draw_gizmos();
		}

		for (auto& obj : transparent_objects)
		{
			obj->draw_gizmos();
		}

		draw_camera_coords();
		//draw_world_coords();
	}

	std::unique_ptr<Scene> Scene::load_asset(const std::string& path)
	{
		auto scene = std::make_unique<Scene>("");
		deserialize(path, *scene);
		return scene;
	}

	void Scene::open_scene(const char* path)
	{
		INST(InputManager).clear_evts();
		auto scene = Scene::load_asset(path);
		if (scene == nullptr) return;
		INST(GlobalShaderParams).enable_shadow = scene->enable_shadow;
		INST(GlobalShaderParams).pcf_on = scene->pcf_on;
		INST(GlobalShaderParams).shadow_bias = scene->shadow_bias;
		INST(GlobalShaderParams).color_space = scene->color_space;
		INST(GlobalShaderParams).workflow = scene->work_flow;
		Scene::set_current(std::move(scene));
		LOG("open scene: {}", path);
	}

	void Scene::serialize(const Scene& scene, const std::string& path)
	{
		rapidjson::Document doc;
		doc.SetObject();

		rapidjson::Value name;
		name.SetString(scene.name.c_str(), doc.GetAllocator());
		doc.AddMember("name", name, doc.GetAllocator());
		doc.AddMember("main_light", DirectionalLight::serialize(doc, scene.main_light), doc.GetAllocator());

		rapidjson::Value point_lights;
		point_lights.SetArray();
		for (auto& pl : scene.point_lights)
		{
			point_lights.PushBack(PointLight::serialize(doc, pl), doc.GetAllocator());
		}
		doc.AddMember("point_lights", point_lights, doc.GetAllocator());

		rapidjson::Value models;
		models.SetArray();
		for (auto& obj : scene.objects)
		{
			rapidjson::Value meta_path;
			meta_path.SetString(obj->target->meta_path.c_str(), doc.GetAllocator());
			models.PushBack(meta_path, doc.GetAllocator());
		}
		for (auto& obj : scene.transparent_objects)
		{
			rapidjson::Value meta_path;
			meta_path.SetString(obj->target->meta_path.c_str(), doc.GetAllocator());
			models.PushBack(meta_path, doc.GetAllocator());
		}
		doc.AddMember("models", models, doc.GetAllocator()); 
		doc.AddMember("enable_ibl", scene.enable_skybox, doc.GetAllocator());
		doc.AddMember("enable_shadow", scene.enable_shadow, doc.GetAllocator());
		doc.AddMember("pcf_on", scene.pcf_on, doc.GetAllocator());
		doc.AddMember("shadow_bias", scene.shadow_bias, doc.GetAllocator());
		doc.AddMember("color_space", (int32_t)scene.color_space, doc.GetAllocator());
		doc.AddMember("work_flow", (int32_t)scene.work_flow, doc.GetAllocator());
		
		rapidjson::Value cubemap_path;
		cubemap_path.SetString(scene.cubemap->meta_path.c_str(), doc.GetAllocator());
		doc.AddMember("cubemap_path", cubemap_path, doc.GetAllocator());

		rapidjson::Value main_cam = Camera::serialize(doc, *scene.main_cam).GetObject();
		doc.AddMember("main_cam", main_cam, doc.GetAllocator());

		rapidjson::StringBuffer sb;
		sb.Clear();
		rapidjson::PrettyWriter<rapidjson::StringBuffer> ww(sb);
		doc.Accept(ww);

		std::filesystem::path abs_path(ASSETS_PATH + path);
		if (!std::filesystem::exists(abs_path.parent_path()))
		{
			std::filesystem::create_directories(abs_path.parent_path());
		}
		std::FILE* fd = fopen(abs_path.string().c_str(), "w+");
		if (fd != nullptr)
		{
			char write_buffer[256];
			rapidjson::FileWriteStream fs(fd, write_buffer, sizeof(write_buffer));
			rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(fs);
			doc.Accept(writer);
			fclose(fd);
			LOG("save scene: {}", path);
		}
		else
		{
			ERROR("path does not exist: {}", ASSETS_PATH + path);
		}
	}

	void Scene::deserialize(const std::string& path, Scene& scene)
	{
		scene.initialize();
		scene.asset_path = path;
		std::filesystem::path abs_path(ASSETS_PATH + path);
		std::FILE* fd = fopen(abs_path.string().c_str(), "r");
		if (fd != nullptr)
		{
			char read_buffer[256];
			rapidjson::FileReadStream fs(fd, read_buffer, sizeof(read_buffer));
			rapidjson::Document doc;
			doc.ParseStream(fs);
			
			const char* name = doc["name"].GetString();
			scene.name = name;
			if (scene.name == "")
			{
				size_t last_slash = path.find_last_of("/\\");
				std::string nice_name = path;
				nice_name = nice_name.replace(nice_name.begin(), nice_name.begin() + last_slash + 1, "");
				scene.name = nice_name;
			}
			scene.main_light = DirectionalLight::deserialize(doc["main_light"].GetObject());
			auto point_lights = doc["point_lights"].GetArray();
			for (rapidjson::SizeType idx = 0; idx < point_lights.Size(); idx++)
			{
				scene.point_lights.push_back(PointLight::deserialize(point_lights[idx].GetObject()));
			}

			auto models = doc["models"].GetArray();
			for (rapidjson::SizeType idx = 0; idx < models.Size(); idx++)
			{
				scene.add(Model::load_asset(models[idx].GetString()));
			}

			const char* cubemap_path = doc["cubemap_path"].GetString();
			if (cubemap_path != nullptr)
			{
				scene.cubemap = std::shared_ptr<CubeMap>(new CubeMap(cubemap_path));
			}

			scene.enable_skybox = doc["enable_ibl"].GetBool();
			scene.enable_shadow = doc["enable_shadow"].GetBool();
			scene.pcf_on = doc["pcf_on"].GetBool();
			scene.shadow_bias = doc["shadow_bias"].GetFloat();
			scene.color_space = (ColorSpace)doc["color_space"].GetInt();
			scene.work_flow = (PBRWorkFlow)doc["work_flow"].GetInt();

			scene.main_cam = Camera::deserialize(doc["main_cam"].GetObject());
			Camera::set_main_camera(scene.main_cam.get());

			fclose(fd);
		}
		else
		{
			ERROR("path does not exist: {}", ASSETS_PATH + path);
		}
	}
}
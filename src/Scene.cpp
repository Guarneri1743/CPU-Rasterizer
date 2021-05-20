#include "Scene.hpp"
#include "Marcos.h"
#include "GDIWindow.hpp"
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

#undef near
#undef far
#undef GetObject

namespace Guarneri
{
	Scene::Scene(std::string name)
	{
		this->name = name;
		enable_skybox = false;
		main_light.intensity = 1.0f;
		main_light.diffuse = Color(1.0f, 0.8f, 0.8f, 1.0f);
		main_light.ambient = Color(0.1f, 0.05f, 0.2f, 1.0f);
		main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
		main_light.position = Vector3(1.0f, 1.0f, 1.0f);
		main_cam = Camera::create(Vector3(5.0f, 5.0f, 5.0f), INST(GDIWindow).aspect, 45.0f, 0.5f, 100.0f);
		main_cam->transform->set_world_angle(33.0f, -330.0f, 0.0f);
		skybox = std::make_unique<SkyboxRenderer>();
	}

	Scene::~Scene()
	{}

	void Scene::initialize()
	{
		debug_cam_distance = 6.0f;
		debug_world_cam_distance = 8.0f;
		debug_cam = std::move(Camera::create(main_cam->transform->world_position() + Vector3(1.0f, 1.0f, -1.0f) * debug_cam_distance, INST(GDIWindow).aspect, 45.0f, 0.5f, 10.0f));
		world_debug_cam = std::move(Camera::create(Vector3(1.0f, 1.0f, -1.0f) * debug_world_cam_distance, INST(GDIWindow).aspect, 45.0f, 0.5f, 10.0f));

		INST(InputManager).add_on_mouse_move_evt([](Vector2 prev, Vector2 pos, void* data)
		{
			if (INST(InputManager).is_mouse_down(MouseButton::RIGHT))
			{
				Vector2 offset = (pos - prev) * Vector2(INST(GDIWindow).width, INST(GDIWindow).height) * CAMERA_ROTATE_SPEED;
				Scene* s = reinterpret_cast<Scene*>(data);
				s->main_cam->transform->rotate(offset.y, offset.x, 0.0f);
			}
			if (INST(InputManager).is_mouse_down(MouseButton::MIDDLE))
			{
				Vector2 offset = (pos - prev) * Vector2(INST(GDIWindow).width, INST(GDIWindow).height) * CAMERA_ROTATE_SPEED;
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

		INST(InputManager).add_on_key_down_evt([](KeyCode code, void* data)
		{
			UNUSED(data);
			if (code == KeyCode::F1)
			{
				INST(GlobalShaderParams).render_flag = INST(GlobalShaderParams).render_flag ^ RenderFlag::SHADOWMAP;
			}
			else if (code == KeyCode::F2)
			{
				INST(GlobalShaderParams).render_flag = INST(GlobalShaderParams).render_flag ^ RenderFlag::DEPTH;
			}
			else if (code == KeyCode::F3)
			{
				INST(GlobalShaderParams).render_flag = INST(GlobalShaderParams).render_flag ^ RenderFlag::WIREFRAME;
			}
			else if (code == KeyCode::F4)
			{
				INST(GlobalShaderParams).render_flag = INST(GlobalShaderParams).render_flag ^ RenderFlag::FRAME_TILE;
			}
			else if (code == KeyCode::F5)
			{
				INST(GlobalShaderParams).render_flag = INST(GlobalShaderParams).render_flag ^ RenderFlag::UV;
			}
			else if (code == KeyCode::F6)
			{
				INST(GlobalShaderParams).render_flag = INST(GlobalShaderParams).render_flag ^ RenderFlag::CULLED_BACK_FACE;
			}
			else if (code == KeyCode::F7)
			{
				INST(GlobalShaderParams).render_flag = INST(GlobalShaderParams).render_flag ^ RenderFlag::NORMAL;
			}
			else if (code == KeyCode::F8)
			{
				INST(GlobalShaderParams).render_flag = INST(GlobalShaderParams).render_flag ^ RenderFlag::SPECULAR;
			}
			else if (code == KeyCode::F9)
			{
				INST(GlobalShaderParams).render_flag = INST(GlobalShaderParams).render_flag ^ RenderFlag::STENCIL;
			}
			else if (code == KeyCode::Z)
			{
				INST(GlobalShaderParams).render_flag = INST(GlobalShaderParams).render_flag ^ RenderFlag::EARLY_Z_DEBUG;
			}
			else if (code == KeyCode::P)
			{
				INST(GlobalShaderParams).enable_shadow = !INST(GlobalShaderParams).enable_shadow;
			}
			else if (code == KeyCode::O)
			{
				INST(GlobalShaderParams).pcf_on = !INST(GlobalShaderParams).pcf_on;
			}
			else if (code == KeyCode::M)
			{
				INST(GlobalShaderParams).shadow_bias *= 2.0f;
			}
			else if (code == KeyCode::N)
			{
				INST(GlobalShaderParams).shadow_bias /= 2.0f;
			}
			else if (code == KeyCode::A)
			{
				if (!INST(GlobalShaderParams).enable_msaa)
				{
					INST(GraphicsDevice).set_subsample_count(16);
				}
				else
				{
					INST(GraphicsDevice).set_subsample_count(0);
				}
			}
		}, nullptr);
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
		/*if (input_mgr().is_key_down(KeyCode::W)) {
			main_cam->move_forward(CAMERA_MOVE_SPEED);
		}
		if (input_mgr().is_key_down(KeyCode::A)) {
			main_cam->move_left(CAMERA_MOVE_SPEED);
		}
		if (input_mgr().is_key_down(KeyCode::S)) {
			main_cam->move_backward(CAMERA_MOVE_SPEED);
		}
		if (input_mgr().is_key_down(KeyCode::D)) {
			main_cam->move_right(CAMERA_MOVE_SPEED);
		}
		if (input_mgr().is_key_down(KeyCode::Q)) {
			main_cam->move_ascend(CAMERA_MOVE_SPEED);
		}
		if (input_mgr().is_key_down(KeyCode::E)) {
			main_cam->move_descend(CAMERA_MOVE_SPEED);
		}*/
		if (INST(InputManager).is_mouse_down(MouseButton::MIDDLE))
		{
			main_cam->transform->move_ascend(CAMERA_MOVE_SPEED);
		}
		if (INST(InputManager).is_mouse_down(MouseButton::MIDDLE))
		{
			main_cam->transform->move_descend(CAMERA_MOVE_SPEED);
		}
	}

	void Scene::draw_camera_coords()
	{
		Vector2 offset = Vector2(-(INST(GDIWindow).width / 2 - 50.0f), -(INST(GDIWindow).height / 2 - 50.0f));
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
		Vector2 offset = Vector2(-(INST(GDIWindow).width / 2 - 150.0f), -(INST(GDIWindow).height / 2 - 150.0f));
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

		if (enable_skybox)
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

	std::unique_ptr<Scene> Scene::create(const std::string& name)
	{
		return std::unique_ptr<Scene>(new Scene(name));
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
		doc.AddMember("enable_skybox", scene.enable_skybox, doc.GetAllocator());
		doc.AddMember("enable_shadow", scene.enable_shadow, doc.GetAllocator());
		doc.AddMember("pcf_on", scene.pcf_on, doc.GetAllocator());
		doc.AddMember("shadow_bias", scene.shadow_bias, doc.GetAllocator());
		doc.AddMember("color_space", (int32_t)scene.color_space, doc.GetAllocator());
		doc.AddMember("work_flow", (int32_t)scene.work_flow, doc.GetAllocator());

		rapidjson::Value main_cam = Camera::serialize(doc, *scene.main_cam).GetObject();
		doc.AddMember("main_cam", main_cam, doc.GetAllocator());

		rapidjson::StringBuffer sb;
		sb.Clear();
		rapidjson::PrettyWriter<rapidjson::StringBuffer> ww(sb);
		doc.Accept(ww);
		std::cout << sb.GetString() << std::endl;

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
			std::cout << "save scene: " << path << std::endl;
		}
		else
		{
			std::cout << "path does not exist: " << ASSETS_PATH + path << std::endl;
		}
	}

	std::unique_ptr<Scene> Scene::deserialize(const std::string& path)
	{
		std::filesystem::path abs_path(ASSETS_PATH + path);
		std::FILE* fd = fopen(abs_path.string().c_str(), "r");
		if (fd != nullptr)
		{
			char read_buffer[256];
			rapidjson::FileReadStream fs(fd, read_buffer, sizeof(read_buffer));
			rapidjson::Document doc;
			doc.ParseStream(fs);
			
			const char* name = doc["name"].GetString();
			auto scene = std::unique_ptr<Scene>(new Scene(name));

			scene->main_light = DirectionalLight::deserialize(doc["main_light"].GetObject());
			auto point_lights = doc["point_lights"].GetArray();
			for (rapidjson::SizeType idx = 0; idx < point_lights.Size(); idx++)
			{
				scene->point_lights.push_back(PointLight::deserialize(point_lights[idx].GetObject()));
			}

			auto models = doc["models"].GetArray();
			for (rapidjson::SizeType idx = 0; idx < models.Size(); idx++)
			{
				scene->add(Model::create(models[idx].GetString()));
			}

			scene->enable_skybox = doc["enable_skybox"].GetBool();
			scene->enable_shadow = doc["enable_shadow"].GetBool();
			scene->pcf_on = doc["pcf_on"].GetBool();
			scene->shadow_bias = doc["shadow_bias"].GetFloat();
			scene->color_space = (ColorSpace)doc["color_space"].GetInt();
			scene->work_flow = (PBRWorkFlow)doc["work_flow"].GetInt();

			scene->main_cam = Camera::deserialize(doc["main_cam"].GetObject());
			scene->initialize();

			fclose(fd);
			return scene;
		}
		else
		{
			std::cout << "path does not exist: " << ASSETS_PATH + path << std::endl;
		}
		return nullptr;
	}
}
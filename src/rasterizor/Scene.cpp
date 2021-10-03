#include "Scene.hpp"
#include "Marcos.h"
#include "GraphicsDevice.hpp"
#include "Singleton.hpp"
#include "GlobalShaderParams.hpp"
#include "InputManager.hpp"
#include "tinymath.h"
#include "Config.h"
#include <filesystem>
#include <iostream>
#include "Utility.hpp"
#include "Logger.hpp"
#include "Serialization.hpp"

#define CAMERA_ROTATE_SPEED 0.25f
#define CAMERA_MOVE_SPEED 0.2f
#define CAMERA_ZOOM_SPEED 2.0f

#undef near
#undef far
#undef GetObject

namespace Guarneri
{
	Scene* Scene::current_scene;

	Scene::Scene()
	{
		this->name = "default_scene";
		selection = nullptr;
		enable_skybox = false;
		main_light.intensity = 1.0f;
		main_light.diffuse = tinymath::Color(1.0f, 0.8f, 0.8f, 1.0f);
		main_light.ambient = tinymath::Color(0.1f, 0.05f, 0.2f, 1.0f);
		main_light.specular = tinymath::Color(1.0f, 1.0f, 1.0f, 1.0f);
		skybox = std::make_unique<SkyboxRenderer>();
	}

	Scene::Scene(std::string name) : Scene()
	{
		this->name = name;
	}

	Scene::~Scene()
	{
		selection = nullptr;
		LOG("destruct scene: {}", name);
	}

	void Scene::initialize()
	{
		float aspect = (float)INST(GraphicsDevice).get_width() / INST(GraphicsDevice).get_height();
		main_cam = Camera::create(tinymath::vec3f(5.0f, 5.0f, 5.0f), aspect, 45.0f, 0.5f, 100.0f);
		main_cam->transform->set_world_angle(33.0f, -330.0f, 0.0f);
		Camera::set_main_camera(main_cam.get());
		debug_cam_distance = 6.0f;
		debug_world_cam_distance = 8.0f;
		debug_cam = std::move(Camera::create(main_cam->transform->world_position() + tinymath::vec3f(1.0f, 1.0f, -1.0f) * debug_cam_distance, aspect, 45.0f, 0.5f, 10.0f));
		world_debug_cam = std::move(Camera::create(tinymath::vec3f(1.0f, 1.0f, -1.0f) * debug_world_cam_distance, aspect, 45.0f, 0.5f, 10.0f));

		INST(InputManager).add_on_mouse_move_evt([](tinymath::vec2f prev, tinymath::vec2f pos, void* data)
		{
			if (INST(InputManager).is_mouse_down(MouseButton::RIGHT))
			{
				Scene* s = reinterpret_cast<Scene*>(data);
				tinymath::vec2f offset = (pos - prev) * CAMERA_ROTATE_SPEED;
				s->main_cam->transform->rotate(offset.y, offset.x, 0.0f);
			}
			if (INST(InputManager).is_mouse_down(MouseButton::MIDDLE))
			{
				tinymath::vec2f offset = (pos - prev) * CAMERA_ROTATE_SPEED;
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

		// setup shadowmap and cubemap
		shadowmap_id = INST(GraphicsDevice).create_buffer(512, 512, FrameContent::Depth);
		std::shared_ptr<FrameBuffer> shadowmap;
		INST(GraphicsDevice).get_buffer(shadowmap_id, shadowmap);

		Shader::global_shader_properties.set_cubemap(cubemap_prop, Scene::current()->cubemap);
		Shader::global_shader_properties.set_framebuffer(shadowmap_prop, shadowmap);
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
		tinymath::vec2f offset = tinymath::vec2f(-(INST(GraphicsDevice).get_width() / 2.0f - 50.0f), -(INST(GraphicsDevice).get_height() / 2.0f - 50.0f));
		tinymath::vec3f pos = main_cam->transform->world_position();
		tinymath::vec3f forward = main_cam->transform->forward();
		tinymath::vec3f right = main_cam->transform->right();
		tinymath::vec3f up = main_cam->transform->up();
		INST(GraphicsDevice).draw_coordinates(pos, forward, up, right, debug_cam->view_matrix(), debug_cam->projection_matrix(), offset);
		debug_cam->transform->set_world_position((main_cam->transform->world_position() + tinymath::vec3f(1.0f, 1.0f, -1.0f) * debug_cam_distance));
		debug_cam->transform->lookat(main_cam->transform->world_position());
	}

	void Scene::draw_world_coords()
	{
		tinymath::vec2f offset = tinymath::vec2f(-(INST(GraphicsDevice).get_width() / 2.0f - 150.0f), -(INST(GraphicsDevice).get_height() / 2.0f - 150.0f));
		INST(GraphicsDevice).draw_coordinates(tinymath::kVec3fZero, tinymath::kVec3fForward * 3.0f, tinymath::kVec3fUp * 3.0f, tinymath::kVec3fRight * 3.0f, world_debug_cam->view_matrix(), world_debug_cam->projection_matrix(), offset);
		INST(GraphicsDevice).draw_coordinates(tinymath::kVec3fZero, tinymath::kVec3fForward * 3.0f, tinymath::kVec3fUp * 3.0f, tinymath::kVec3fRight * 3.0f, main_cam->view_matrix(), main_cam->projection_matrix());

		tinymath::vec3f pos = main_cam->transform->world_position();
		tinymath::vec3f forward = main_cam->transform->forward();
		tinymath::vec3f right = main_cam->transform->right();
		tinymath::vec3f up = main_cam->transform->up();
		INST(GraphicsDevice).draw_coordinates(pos, forward, up, right, world_debug_cam->view_matrix(), world_debug_cam->projection_matrix(), offset);

		world_debug_cam->transform->set_world_position((tinymath::vec3f(1.0f, 1.0f, -1.0f) * debug_world_cam_distance));
		world_debug_cam->transform->lookat(tinymath::kVec3fZero);
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
		INST(GraphicsDevice).clear_buffer(FrameContent::Color | FrameContent::Depth | FrameContent::Stencil | FrameContent::Coverage);
		if (INST(GlobalShaderParams).enable_shadow)
		{
			INST(GraphicsDevice).set_active_frame_buffer(shadowmap_id);
			render_shadow();
			INST(GraphicsDevice).present();
			INST(GraphicsDevice).reset_active_frame_buffer();
		}

		render_objects();
		INST(GraphicsDevice).present();
		draw_gizmos();
	}

	void Scene::render_shadow()
	{
		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::DEPTH) != RenderFlag::DISABLE)
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

		if ((INST(GlobalShaderParams).debug_flag & RenderFlag::SHADOWMAP) != RenderFlag::DISABLE)
		{
			return;
		}

		bool enable_frustum_culling = (INST(GlobalShaderParams).culling_clipping_flag & CullingAndClippingFlag::APP_FRUSTUM_CULLING) != CullingAndClippingFlag::DISABLE;
		if (enable_frustum_culling)
		{
			// todo: CPU Frustum Culling
		}

		if (enable_skybox && INST(GlobalShaderParams).enable_ibl)
		{
			skybox->render();
		}

		for (auto& obj : objects)
		{
			obj->render();
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

		debug_scene();
	}

	void Scene::debug_scene()
	{
		if (INST(GlobalShaderParams).debug_flag == RenderFlag::DISABLE) { return; }

		FrameBuffer* active_fbo = INST(GraphicsDevice).get_active_framebuffer();
		std::shared_ptr<FrameBuffer> shadow_map;
		INST(GraphicsDevice).get_buffer(shadowmap_id, shadow_map);
		size_t w, h;
		active_fbo->get_size(w, h);
		for (size_t row = 0; row < h; ++row)
		{
			for (size_t col = 0; col < w; ++col)
			{
				// stencil visualization
				if ((INST(GlobalShaderParams).debug_flag & RenderFlag::STENCIL) != RenderFlag::DISABLE)
				{
					uint8_t stencil;
					if (active_fbo->read_stencil(row, col, stencil))
					{
						tinymath::color_rgba c = ColorEncoding::encode_rgba(stencil, stencil, stencil, 255);
						active_fbo->write_color(row, col, c);
					}
				}

				// depth buffer visualization
				if ((INST(GlobalShaderParams).debug_flag & RenderFlag::DEPTH) != RenderFlag::DISABLE)
				{
					float cur_depth;
					if (active_fbo->read_depth(row, col, cur_depth))
					{
						float linear_depth = Shader::linearize_01depth(cur_depth, INST(GlobalShaderParams).cam_near, INST(GlobalShaderParams).cam_far);
						tinymath::Color depth_color = tinymath::kColorWhite * linear_depth;
						tinymath::color_rgba c = ColorEncoding::encode_rgba(depth_color);
						active_fbo->write_color(row, col, c);
					}
				}

				// shadowmap visualization
				if ((INST(GlobalShaderParams).debug_flag & RenderFlag::SHADOWMAP) != RenderFlag::DISABLE)
				{
					float u, v;
					pixel2uv(w, h, row, col, u, v);
					float cur_depth;
					if (shadow_map->read_depth(u, v, cur_depth))
					{
						tinymath::Color depth_color = tinymath::kColorWhite * cur_depth;
						tinymath::color_rgba c = ColorEncoding::encode_rgba(depth_color);
						active_fbo->write_color(u, v, c);
					}
				}
			}
		}
	}

	void Scene::resize_shadowmap(const size_t& w, const size_t& h)
	{
		std::shared_ptr<FrameBuffer> shadowmap;
		INST(GraphicsDevice).get_buffer(shadowmap_id, shadowmap);
		shadowmap->resize(w, h);
	}

	void Scene::get_shadowmap_size(size_t & w, size_t & h)
	{
		std::shared_ptr<FrameBuffer> shadowmap;
		INST(GraphicsDevice).get_buffer(shadowmap_id, shadowmap);
		w = shadowmap->get_width();
		h = shadowmap->get_height();
	}

	void Scene::open_scene(const char* path)
	{
		INST(InputManager).clear_evts();
		Scene* deserialized_scene = new Scene();
		Serializer::deserialize(path, *deserialized_scene);
		if (deserialized_scene == nullptr) return;
		INST(GlobalShaderParams).enable_shadow = deserialized_scene->enable_shadow;
		INST(GlobalShaderParams).pcf_on = deserialized_scene->pcf_on;
		INST(GlobalShaderParams).shadow_bias = deserialized_scene->shadow_bias;
		INST(GlobalShaderParams).color_space = deserialized_scene->color_space;
		INST(GlobalShaderParams).workflow = deserialized_scene->work_flow;
		current_scene = deserialized_scene;
		current_scene->initialize();
		LOG("open scene: {}", path);
	}
}
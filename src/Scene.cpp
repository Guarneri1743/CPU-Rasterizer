#include "Scene.hpp"
#include "Marcos.h"
#include "GDIWindow.hpp"
#include "GraphicsDevice.hpp"
#include "Singleton.hpp"
#include "Misc.hpp"
#include "InputManager.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Matrix4x4.hpp"
#include "Color.hpp"
#include "Config.h"

#undef near
#undef far

namespace Guarneri
{
	Scene::Scene()
	{
		initialize();
	}

	Scene::~Scene()
	{}

	// todo: serialzie & deserialize Scene data
	void Scene::initialize()
	{
		enable_skybox = false;
		main_light.intensity = 1.0f;
		main_light.diffuse = Color(1.0f, 0.8f, 0.8f, 1.0f);
		main_light.ambient = Color(0.1f, 0.05f, 0.2f, 1.0f);
		main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
		main_light.position = Vector3(1.0f, 1.0f, 1.0f);
		debug_cam_distance = 6.0f;
		debug_world_cam_distance = 8.0f;
		main_cam = std::move(Camera::create(Vector3(5.0f, 5.0f, 5.0f), INST(GDIWindow).aspect, 45.0f, 0.5f, 100.0f));
		main_cam->lookat(Vector3::ZERO);

		debug_cam = std::move(Camera::create(main_cam->position + Vector3(1.0f, 1.0f, -1.0f) * debug_cam_distance, INST(GDIWindow).aspect, 45.0f, 0.5f, 10.0f));
		world_debug_cam = std::move(Camera::create(Vector3(1.0f, 1.0f, -1.0f) * debug_world_cam_distance, INST(GDIWindow).aspect, 45.0f, 0.5f, 10.0f));

		skybox = std::make_unique<SkyboxRenderer>();

		INST(InputManager).add_on_mouse_move_evt([](Vector2 prev, Vector2 pos, void* data)
		{
			if (INST(InputManager).is_mouse_down(MouseButton::RIGHT))
			{
				Vector2 offset = (pos - prev) * Vector2(INST(GDIWindow).width, INST(GDIWindow).height) * CAMERA_ROTATE_SPEED;
				Scene* s = reinterpret_cast<Scene*>(data);
				s->main_cam->rotate(offset.x, offset.y);
			}
			if (INST(InputManager).is_mouse_down(MouseButton::MIDDLE))
			{
				Vector2 offset = (pos - prev) * Vector2(INST(GDIWindow).width, INST(GDIWindow).height) * CAMERA_ROTATE_SPEED;
				Scene* s = reinterpret_cast<Scene*>(data);
				s->main_cam->move_left(offset.x);
				s->main_cam->move_ascend(offset.y);
			}
		}, this);

		INST(InputManager).add_on_mouse_wheel_rolling_evt([](MouseWheel rolling, void* data)
		{
			Scene* s = reinterpret_cast<Scene*>(data);
			if (rolling == MouseWheel::UP)
			{
				s->main_cam->move_forward(CAMERA_ZOOM_SPEED);
			}
			else
			{
				s->main_cam->move_backward(CAMERA_ZOOM_SPEED);
			}
		}, this);

		INST(InputManager).add_on_key_down_evt([](KeyCode code, void* data)
		{
			UNUSED(data);
			if (code == KeyCode::F1)
			{
				INST(MiscParameter).render_flag = INST(MiscParameter).render_flag ^ RenderFlag::SHADOWMAP;
			}
			else if (code == KeyCode::F2)
			{
				INST(MiscParameter).render_flag = INST(MiscParameter).render_flag ^ RenderFlag::DEPTH;
			}
			else if (code == KeyCode::F3)
			{
				INST(MiscParameter).render_flag = INST(MiscParameter).render_flag ^ RenderFlag::WIREFRAME;
			}
			else if (code == KeyCode::F4)
			{
				INST(MiscParameter).render_flag = INST(MiscParameter).render_flag ^ RenderFlag::FRAME_TILE;
			}
			else if (code == KeyCode::F5)
			{
				INST(MiscParameter).render_flag = INST(MiscParameter).render_flag ^ RenderFlag::UV;
			}
			else if (code == KeyCode::F6)
			{
				INST(MiscParameter).render_flag = INST(MiscParameter).render_flag ^ RenderFlag::CULLED_BACK_FACE;
			}
			else if (code == KeyCode::F7)
			{
				INST(MiscParameter).render_flag = INST(MiscParameter).render_flag ^ RenderFlag::NORMAL;
			}
			else if (code == KeyCode::F8)
			{
				INST(MiscParameter).render_flag = INST(MiscParameter).render_flag ^ RenderFlag::SPECULAR;
			}
			else if (code == KeyCode::F9)
			{
				INST(MiscParameter).render_flag = INST(MiscParameter).render_flag ^ RenderFlag::STENCIL;
			}
			else if (code == KeyCode::Z)
			{
				INST(MiscParameter).render_flag = INST(MiscParameter).render_flag ^ RenderFlag::EARLY_Z_DEBUG;
			}
			else if (code == KeyCode::P)
			{
				INST(MiscParameter).enable_shadow = !INST(MiscParameter).enable_shadow;
			}
			else if (code == KeyCode::O)
			{
				INST(MiscParameter).pcf_on = !INST(MiscParameter).pcf_on;
			}
			else if (code == KeyCode::M)
			{
				INST(MiscParameter).shadow_bias *= 2.0f;
			}
			else if (code == KeyCode::N)
			{
				INST(MiscParameter).shadow_bias /= 2.0f;
			}
			else if (code == KeyCode::A)
			{
				if (!INST(MiscParameter).enable_msaa)
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

	void Scene::add(std::shared_ptr<Renderer> rdr)
	{
		if (rdr->target->material->transparent)
		{
			transparent_objects.emplace_back(rdr);
		}
		else
		{
			objects.emplace_back(rdr);
		}
	}

	void Scene::update()
	{
		INST(MiscParameter).cam_far = main_cam->far;
		INST(MiscParameter).cam_near = main_cam->near;
		INST(MiscParameter).view_matrix = main_cam->view_matrix();
		INST(MiscParameter).proj_matrix = main_cam->projection_matrix();
		INST(MiscParameter).main_light = main_light;
		INST(MiscParameter).point_lights = point_lights;
		INST(MiscParameter).camera_pos = main_cam->position;
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
			main_cam->move_ascend(CAMERA_MOVE_SPEED);
		}
		if (INST(InputManager).is_mouse_down(MouseButton::MIDDLE))
		{
			main_cam->move_descend(CAMERA_MOVE_SPEED);
		}
	}

	void Scene::draw_camera_coords()
	{
		Vector2 offset = Vector2(-(INST(GDIWindow).width / 2 - 50.0f), -(INST(GDIWindow).height / 2 - 50.0f));
		Vector3 pos = main_cam->position;
		Vector3 forward = main_cam->forward;
		Vector3 right = main_cam->right;
		Vector3 up = main_cam->up;
		INST(GraphicsDevice).draw_coordinates(pos, forward, up, right, debug_cam->view_matrix(), debug_cam->projection_matrix(), offset);
		debug_cam->position = (main_cam->position + Vector3(1.0f, 1.0f, -1.0f) * debug_cam_distance);
		debug_cam->lookat(main_cam->position);
	}

	void Scene::draw_world_coords()
	{
		Vector2 offset = Vector2(-(INST(GDIWindow).width / 2 - 150.0f), -(INST(GDIWindow).height / 2 - 150.0f));
		INST(GraphicsDevice).draw_coordinates(Vector3::ZERO, Vector3::FORWARD * 3.0f, Vector3::UP * 3.0f, Vector3::RIGHT * 3.0f, world_debug_cam->view_matrix(), world_debug_cam->projection_matrix(), offset);
		INST(GraphicsDevice).draw_coordinates(Vector3::ZERO, Vector3::FORWARD * 3.0f, Vector3::UP * 3.0f, Vector3::RIGHT * 3.0f, main_cam->view_matrix(), main_cam->projection_matrix());

		Vector3 pos = main_cam->position;
		Vector3 forward = main_cam->forward;
		Vector3 right = main_cam->right;
		Vector3 up = main_cam->up;
		INST(GraphicsDevice).draw_coordinates(pos, forward, up, right, world_debug_cam->view_matrix(), world_debug_cam->projection_matrix(), offset);

		world_debug_cam->position = (Vector3(1.0f, 1.0f, -1.0f) * debug_world_cam_distance);
		world_debug_cam->lookat(Vector3::ZERO);
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
		if (INST(MiscParameter).enable_shadow)
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
		if ((INST(MiscParameter).render_flag & RenderFlag::DEPTH) != RenderFlag::DISABLE)
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

		if ((INST(MiscParameter).render_flag & RenderFlag::SHADOWMAP) != RenderFlag::DISABLE)
		{
			return;
		}

		bool enable_frustum_culling = (INST(MiscParameter).culling_clipping_flag & CullingAndClippingFlag::APP_FRUSTUM_CULLING) != CullingAndClippingFlag::DISABLE;
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
}
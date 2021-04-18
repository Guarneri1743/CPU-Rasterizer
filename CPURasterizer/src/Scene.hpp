#ifndef _SCENE_
#define _SCENE_
#include <CPURasterizer.hpp>

namespace Guarneri
{
	class Scene
	{
	public:
		DirectionalLight main_light;
		std::vector<PointLight> point_lights;
		std::vector<std::shared_ptr<Renderer>> objects;
		std::vector<std::shared_ptr<Renderer>> transparent_objects;
		std::unique_ptr<SkyboxRenderer> skybox;
		bool enable_skybox;
		std::unique_ptr<Camera> main_cam;
		std::unique_ptr<Camera> debug_cam;
		std::unique_ptr<Camera> world_debug_cam;
		float debug_cam_distance;
		float debug_world_cam_distance;

	public:
		Scene();
		~Scene();
		void initialize();
		void add(std::shared_ptr<Renderer> rdr);
		void update();
		void draw_camera_coords();
		void draw_world_coords();
		void set_main_light(const DirectionalLight& light);
		void add_point_light(const PointLight& light);
		void render();
		void render_shadow();
		void render_objects();
		void draw_gizmos();
	};


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
		main_cam = std::move(Camera::create(Vector3(5.0f, 5.0f, 5.0f), Window().aspect, 45.0f, 0.5f, 100.0f));
		main_cam->lookat(Vector3::ZERO);

		debug_cam = std::move(Camera::create(main_cam->position + Vector3(1.0f, 1.0f, -1.0f) * debug_cam_distance, Window().aspect, 45.0f, 0.5f, 10.0f));
		world_debug_cam = std::move(Camera::create(Vector3(1.0f, 1.0f, -1.0f) * debug_world_cam_distance, Window().aspect, 45.0f, 0.5f, 10.0f));

		skybox = std::make_unique<SkyboxRenderer>();

		InputMgr().add_on_mouse_move_evt([](Vector2 prev, Vector2 pos, void* data)
		{
			if (InputMgr().is_mouse_down(MouseButton::RIGHT))
			{
				Vector2 offset = (pos - prev) * Vector2(Window().width, Window().height) * CAMERA_ROTATE_SPEED;
				Scene* s = reinterpret_cast<Scene*>(data);
				s->main_cam->rotate(offset.x, offset.y);
			}
			if (InputMgr().is_mouse_down(MouseButton::MIDDLE))
			{
				Vector2 offset = (pos - prev) * Vector2(Window().width, Window().height) * CAMERA_ROTATE_SPEED;
				Scene* s = reinterpret_cast<Scene*>(data);
				s->main_cam->move_left(offset.x);
				s->main_cam->move_ascend(offset.y);
			}
		}, this);

		InputMgr().add_on_mouse_wheel_rolling_evt([](MouseWheel rolling, void* data)
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

		InputMgr().add_on_key_down_evt([](KeyCode code, void* data)
		{
			REF(data)
				if (code == KeyCode::F1)
				{
					misc_param.render_flag = misc_param.render_flag ^ RenderFlag::SHADOWMAP;
				}
				else if (code == KeyCode::F2)
				{
					misc_param.render_flag = misc_param.render_flag ^ RenderFlag::DEPTH;
				}
				else if (code == KeyCode::F3)
				{
					misc_param.render_flag = misc_param.render_flag ^ RenderFlag::WIREFRAME;
				}
				else if (code == KeyCode::F4)
				{
					misc_param.render_flag = misc_param.render_flag ^ RenderFlag::FRAME_TILE;
				}
				else if (code == KeyCode::F5)
				{
					misc_param.render_flag = misc_param.render_flag ^ RenderFlag::UV;
				}
				else if (code == KeyCode::F6)
				{
					misc_param.render_flag = misc_param.render_flag ^ RenderFlag::CULLED_BACK_FACE;
				}
				else if (code == KeyCode::F7)
				{
					misc_param.render_flag = misc_param.render_flag ^ RenderFlag::NORMAL;
				}
				else if (code == KeyCode::F8)
				{
					misc_param.render_flag = misc_param.render_flag ^ RenderFlag::SPECULAR;
				}
				else if (code == KeyCode::F9)
				{
					misc_param.render_flag = misc_param.render_flag ^ RenderFlag::STENCIL;
				}
				else if (code == KeyCode::Z)
				{
					misc_param.render_flag = misc_param.render_flag ^ RenderFlag::EARLY_Z_DEBUG;
				}
				else if (code == KeyCode::P)
				{
					misc_param.enable_shadow = !misc_param.enable_shadow;
				}
				else if (code == KeyCode::O)
				{
					misc_param.pcf_on = !misc_param.pcf_on;
				}
				else if (code == KeyCode::M)
				{
					misc_param.shadow_bias *= 2.0f;
				}
				else if (code == KeyCode::N)
				{
					misc_param.shadow_bias /= 2.0f;
				}
				else if (code == KeyCode::A)
				{
					if (!misc_param.enable_msaa)
					{
						Graphics().set_subsample_count(16);
					}
					else
					{
						Graphics().set_subsample_count(0);
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
		misc_param.cam_far = main_cam->far;
		misc_param.cam_near = main_cam->near;
		misc_param.view_matrix = main_cam->view_matrix();
		misc_param.proj_matrix = main_cam->projection_matrix();
		misc_param.main_light = main_light;
		misc_param.point_lights = point_lights;
		misc_param.camera_pos = main_cam->position;
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
		if (InputMgr().is_mouse_down(MouseButton::MIDDLE))
		{
			main_cam->move_ascend(CAMERA_MOVE_SPEED);
		}
		if (InputMgr().is_mouse_down(MouseButton::MIDDLE))
		{
			main_cam->move_descend(CAMERA_MOVE_SPEED);
		}
	}

	void Scene::draw_camera_coords()
	{
		Vector2 offset = Vector2(-(Window().width / 2 - 50.0f), -(Window().height / 2 - 50.0f));
		Vector3 pos = main_cam->position;
		Vector3 forward = main_cam->forward;
		Vector3 right = main_cam->right;
		Vector3 up = main_cam->up;
		Graphics().draw_coordinates(pos, forward, up, right, debug_cam->view_matrix(), debug_cam->projection_matrix(), offset);
		debug_cam->position = (main_cam->position + Vector3(1.0f, 1.0f, -1.0f) * debug_cam_distance);
		debug_cam->lookat(main_cam->position);
	}

	void Scene::draw_world_coords()
	{
		Vector2 offset = Vector2(-(Window().width / 2 - 150.0f), -(Window().height / 2 - 150.0f));
		Graphics().draw_coordinates(Vector3::ZERO, Vector3::FORWARD * 3.0f, Vector3::UP * 3.0f, Vector3::RIGHT * 3.0f, world_debug_cam->view_matrix(), world_debug_cam->projection_matrix(), offset);
		Graphics().draw_coordinates(Vector3::ZERO, Vector3::FORWARD * 3.0f, Vector3::UP * 3.0f, Vector3::RIGHT * 3.0f, main_cam->view_matrix(), main_cam->projection_matrix());

		Vector3 pos = main_cam->position;
		Vector3 forward = main_cam->forward;
		Vector3 right = main_cam->right;
		Vector3 up = main_cam->up;
		Graphics().draw_coordinates(pos, forward, up, right, world_debug_cam->view_matrix(), world_debug_cam->projection_matrix(), offset);

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
		Graphics().clear_buffer(BufferFlag::COLOR | BufferFlag::DEPTH | BufferFlag::STENCIL);
		if (misc_param.enable_shadow)
		{
			render_shadow();
			Graphics().present();
		}
		render_objects();
		Graphics().present();
		draw_gizmos();
	}

	void Scene::render_shadow()
	{
		if ((misc_param.render_flag & RenderFlag::DEPTH) != RenderFlag::DISABLE)
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
		Graphics().multi_thread = true;
		Graphics().tile_based = true;

		if ((misc_param.render_flag & RenderFlag::SHADOWMAP) != RenderFlag::DISABLE)
		{
			return;
		}

		bool enable_frustum_culling = (misc_param.culling_clipping_flag & CullingAndClippingFlag::APP_FRUSTUM_CULLING) != CullingAndClippingFlag::DISABLE;
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
			Graphics().tile_based = false;
			skybox->render();
			Graphics().tile_based = true;
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
#endif
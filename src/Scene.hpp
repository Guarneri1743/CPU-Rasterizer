#pragma once
#include <Guarneri.hpp>

namespace Guarneri{
	class Scene{
	public:
		Scene() {
			initialize();
		}

		~Scene() {

		}

	public:
		DirectionalLight main_light;
		std::vector<std::shared_ptr<Renderer>> objects;
		std::vector<std::shared_ptr<Renderer>> transparent_objects;
		std::unique_ptr<SkyboxRenderer> skybox;
		std::shared_ptr<Camera> main_cam;
		std::shared_ptr<Camera> debug_cam;
		std::shared_ptr<Camera> world_debug_cam;
		float debug_cam_distance;
		float debug_world_cam_distance;
		std::unordered_map<void (*)(void* user_data), void*> on_update_evts;

	public:
		// todo: serialzie & deserialize Scene data
		void initialize() {
			main_light.direction = Vector3(1.0f, 1.0f, 1.0f);
			main_light.intensity = 1.0f;
			main_light.diffuse = Color(1.0f, 0.8f, 0.8f, 1.0f);
			main_light.ambient = Color(0.1f, 0.05f, 0.2f, 1.0f);
			main_light.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
			debug_cam_distance = 6.0f;
			debug_world_cam_distance = 8.0f;
			main_cam = std::move(Camera::create(Vector3(5.0f, 5.0f, 5.0f), window().aspect, 45.0f, 0.5f, 500.0f, Projection::PERSPECTIVE));
			main_cam->lookat(Vector3::ZERO);
			debug_cam = std::move(Camera::create(main_cam->position + Vector3(1.0f, 1.0f, -1.0f) * debug_cam_distance, window().aspect, 45.0f, 0.5f, 10.0f, Projection::PERSPECTIVE));
			world_debug_cam = std::move(Camera::create(Vector3(1.0f, 1.0f, -1.0f) * debug_world_cam_distance, window().aspect, 45.0f, 0.5f, 10.0f, Projection::PERSPECTIVE));

			skybox = std::make_unique<SkyboxRenderer>();

			input_mgr().add_on_mouse_move_evt([](Vector2 prev, Vector2 pos, void* data) {
				if (input_mgr().is_mouse_down(MouseButton::RIGHT)) {
					Vector2 offset = (pos - prev) * Vector2(window().width, window().height) * CAMERA_ROTATE_SPEED;
					Scene* s = reinterpret_cast<Scene*>(data);
					s->main_cam->rotate(offset.x, offset.y);
				}
				if (input_mgr().is_mouse_down(MouseButton::MIDDLE)) {
					Vector2 offset = (pos - prev) * Vector2(window().width, window().height) * CAMERA_ROTATE_SPEED;
					Scene* s = reinterpret_cast<Scene*>(data);
					s->main_cam->move_left(offset.x);
					s->main_cam->move_ascend(offset.y);
				}
			}, this);

			input_mgr().add_on_mouse_wheel_rolling_evt([](MouseWheel rolling, void* data) {
					Scene* s = reinterpret_cast<Scene*>(data);
					if (rolling == MouseWheel::UP) {
						s->main_cam->move_forward(CAMERA_ZOOM_SPEED);
					}
					else {
						s->main_cam->move_backward(CAMERA_ZOOM_SPEED);
					}
				}, this);

			input_mgr().add_on_key_down_evt([](KeyCode code, void* data){			
				REF(data)
				if (code == KeyCode::F1) {
					misc_param.flag = (RenderFlag)((int)misc_param.flag ^ (int)RenderFlag::SHADED);
				}
				else if (code == KeyCode::F2) {
					misc_param.flag = (RenderFlag)((int)misc_param.flag ^ (int)RenderFlag::SCANLINE);
				}
				else if (code == KeyCode::F3) {
					misc_param.flag = (RenderFlag)((int)misc_param.flag ^ (int)RenderFlag::WIREFRAME);
				}
				else if (code == KeyCode::F4) {
					misc_param.flag = (RenderFlag)((int)misc_param.flag ^ (int)RenderFlag::DEPTH);
				}
				else if (code == KeyCode::F5) {
					misc_param.flag = (RenderFlag)((int)misc_param.flag ^ (int)RenderFlag::UV);
				}
				else if (code == KeyCode::F6) {
					misc_param.flag = (RenderFlag)((int)misc_param.flag ^ (int)RenderFlag::VERTEX_COLOR);
				}
				else if (code == KeyCode::F7) {
					misc_param.flag = (RenderFlag)((int)misc_param.flag ^ (int)RenderFlag::SEMI_TRANSPARENT);
				}
				else if (code == KeyCode::F8) {
					misc_param.flag = (RenderFlag)((int)misc_param.flag ^ (int)RenderFlag::NORMAL);
				}
				}, nullptr);
		}

		void add(std::shared_ptr<Renderer> target, bool transparent) {
			if (transparent) {
				transparent_objects.push_back(target);
			}
			else {
				objects.push_back(target);
			}
		}

		void add_on_update_evt(void (*on_update)(void* ud), void* user_data) {
			if (on_update_evts.count(on_update) > 0) {
				return;
			}
			on_update_evts[on_update] = user_data;
		}

		void update() {
			misc_param.cam_far = main_cam->far;
			misc_param.cam_near = main_cam->near;
			misc_param.view_matrix = main_cam->view_matrix();
			misc_param.proj_matrix = main_cam->projection_matrix();
			misc_param.screen_width = window().width;
			misc_param.screen_height = window().height;
			misc_param.main_light = main_light;
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
			if (input_mgr().is_mouse_down(MouseButton::MIDDLE)) {
				main_cam->move_ascend(CAMERA_MOVE_SPEED);
			}
			if (input_mgr().is_mouse_down(MouseButton::MIDDLE)) {
				main_cam->move_descend(CAMERA_MOVE_SPEED);
			}

			for (auto& kv : on_update_evts) {
				auto evt = kv.first;
				auto user_data = kv.second;
				evt(user_data);
			}
		}

		void draw_camera_coords() {
			Vector2 offset = Vector2(-(window().width / 2 - 50.0f), -(window().height / 2 - 50.0f));
			Vector3 pos = main_cam->position;
			Vector3 forward = main_cam->forward;
			Vector3 right = main_cam->right;
			Vector3 up = main_cam->up;
			graphics().draw_coordinates(pos, forward, up, right, debug_cam->view_matrix(), debug_cam->projection_matrix(), offset);
			debug_cam->position = (main_cam->position + Vector3(1.0f, 1.0f, -1.0f) * debug_cam_distance);
			debug_cam->lookat(main_cam->position);
		}

		void draw_world_coords() {
			Vector2 offset = Vector2(-(window().width / 2 - 150.0f), -(window().height / 2 - 150.0f));
			graphics().draw_coordinates(Vector3::ZERO, Vector3::FORWARD * 3.0f, Vector3::UP * 3.0f, Vector3::RIGHT * 3.0f, world_debug_cam->view_matrix(), world_debug_cam->projection_matrix(), offset);
			graphics().draw_coordinates(Vector3::ZERO, Vector3::FORWARD * 3.0f, Vector3::UP * 3.0f, Vector3::RIGHT * 3.0f, main_cam->view_matrix(), main_cam->projection_matrix());
	
			Vector3 pos = main_cam->position;
			Vector3 forward = main_cam->forward;
			Vector3 right = main_cam->right;
			Vector3 up = main_cam->up;
			graphics().draw_coordinates(pos, forward, up, right, world_debug_cam->view_matrix(), world_debug_cam->projection_matrix(), offset);

			world_debug_cam->position = (Vector3(1.0f, 1.0f, -1.0f) * debug_world_cam_distance);
			world_debug_cam->lookat(Vector3::ZERO);
		}

		void render() {
			for (auto& obj : objects) {
				obj->render();
			}

			for (auto& obj : transparent_objects) {
				obj->render();
			}

			skybox->render();

			//draw_world_coords();
			draw_camera_coords();
		}
	};
}
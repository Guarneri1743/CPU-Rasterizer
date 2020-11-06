#pragma once
#include <guarneri.hpp>

namespace guarneri{
	class scene{
	public:
		scene() {
			initialize();
		}

		~scene() {

		}

	public:
		directional_light main_light;
		std::vector<std::shared_ptr<renderer>> objects;
		std::vector<std::shared_ptr<renderer>> transparent_objects;
		std::shared_ptr<camera> main_cam;
		std::shared_ptr<camera> debug_cam;
		std::shared_ptr<camera> world_debug_cam;
		float debug_cam_distance;
		float debug_world_cam_distance;
		std::unordered_map<void (*)(void* user_data), void*> on_update_evts;

	public:
		// todo: serialzie & deserialize scene data
		void initialize() {
			main_light.direction = float3(1.0f, 1.0f, 1.0f);
			main_light.intensity = 1.0f;
			main_light.diffuse = color(1.0f, 0.8f, 0.8f, 1.0f);
			main_light.ambient = color(0.1f, 0.05f, 0.2f, 1.0f);
			main_light.specular = color(1.0f, 1.0f, 1.0f, 1.0f);
			debug_cam_distance = 6.0f;
			debug_world_cam_distance = 8.0f;
			main_cam = std::move(camera::create(float3(5.0f, 5.0f, 5.0f), window().aspect, 60.0f, 0.5f, 500.0f, projection::perspective));
			main_cam->lookat(float3::ZERO);
			debug_cam = std::move(camera::create(main_cam->position + float3(1.0f, 1.0f, -1.0f) * debug_cam_distance, window().aspect, 60.0f, 0.5f, 10.0f, projection::perspective));
			world_debug_cam = std::move(camera::create(float3(1.0f, 1.0f, -1.0f) * debug_world_cam_distance, window().aspect, 60.0f, 0.5f, 10.0f, projection::perspective));

			input_mgr().add_on_mouse_move_evt([](float2 prev, float2 pos, void* data) {
				if (input_mgr().is_mouse_down(mouse_button::right)) {
					float2 offset = (pos - prev) * float2(window().width, window().height) * CAMERA_ROTATE_SPEED;
					scene* s = reinterpret_cast<scene*>(data);
					s->main_cam->rotate(offset.x, offset.y);
				}
				if (input_mgr().is_mouse_down(mouse_button::middle)) {
					float2 offset = (pos - prev) * float2(window().width, window().height) * CAMERA_ROTATE_SPEED;
					scene* s = reinterpret_cast<scene*>(data);
					s->main_cam->move_left(offset.x);
					s->main_cam->move_ascend(offset.y);
				}
			}, this);

			input_mgr().add_on_mouse_wheel_rolling_evt([](mouse_wheel_rolling rolling, void* data) {
					scene* s = reinterpret_cast<scene*>(data);
					if (rolling == mouse_wheel_rolling::up) {
						s->main_cam->move_forward(CAMERA_ZOOM_SPEED);
					}
					else {
						s->main_cam->move_backward(CAMERA_ZOOM_SPEED);
					}
				}, this);

			input_mgr().add_on_key_down_evt([](key_code code, void* data) {
				auto device = (render_device*)data;
				if (code == key_code::F1) {
					device->r_flag = (render_flag)((int)device->r_flag ^ (int)render_flag::shaded);
				}
				else if (code == key_code::F2) {
					device->r_flag = (render_flag)((int)device->r_flag ^ (int)render_flag::scanline);
				}
				else if (code == key_code::F3) {
					device->r_flag = (render_flag)((int)device->r_flag ^ (int)render_flag::wire_frame);
				}
				else if (code == key_code::F4) {
					device->r_flag = (render_flag)((int)device->r_flag ^ (int)render_flag::depth);
				}
				else if (code == key_code::F5) {
					device->r_flag = (render_flag)((int)device->r_flag ^ (int)render_flag::uv);
				}
				else if (code == key_code::F6) {
					device->r_flag = (render_flag)((int)device->r_flag ^ (int)render_flag::vertex_color);
				}
				else if (code == key_code::F7) {
					device->r_flag = (render_flag)((int)device->r_flag ^ (int)render_flag::transparent);
				}
				}, &grapihcs());
		}

		void add(std::shared_ptr<renderer> target, bool transparent) {
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

			/*if (input_mgr().is_key_down(key_code::W)) {
				main_cam->move_forward(CAMERA_MOVE_SPEED);
			}
			if (input_mgr().is_key_down(key_code::A)) {
				main_cam->move_left(CAMERA_MOVE_SPEED);
			}
			if (input_mgr().is_key_down(key_code::S)) {
				main_cam->move_backward(CAMERA_MOVE_SPEED);
			}
			if (input_mgr().is_key_down(key_code::D)) {
				main_cam->move_right(CAMERA_MOVE_SPEED);
			}
			if (input_mgr().is_key_down(key_code::Q)) {
				main_cam->move_ascend(CAMERA_MOVE_SPEED);
			}
			if (input_mgr().is_key_down(key_code::E)) {
				main_cam->move_descend(CAMERA_MOVE_SPEED);
			}*/
			if (input_mgr().is_mouse_down(mouse_button::middle)) {
				main_cam->move_ascend(CAMERA_MOVE_SPEED);
			}
			if (input_mgr().is_mouse_down(mouse_button::middle)) {
				main_cam->move_descend(CAMERA_MOVE_SPEED);
			}

			for (auto& kv : on_update_evts) {
				auto evt = kv.first;
				auto user_data = kv.second;
				evt(user_data);
			}
		}

		void draw_camera_coords() {
			float2 offset = float2(-(window().width / 2 - 50.0f), window().height / 2 - 50.0f);
			float3 pos = main_cam->position;
			float3 forward = main_cam->forward;
			float3 right = main_cam->right;
			float3 up = main_cam->up;
			grapihcs().draw_coordinates(pos, forward, up, right, debug_cam->view_matrix(), debug_cam->projection_matrix(), offset);
			debug_cam->position = (main_cam->position + float3(1.0f, 1.0f, -1.0f) * debug_cam_distance);
			debug_cam->lookat(main_cam->position);
		}

		void draw_world_coords() {
			float2 offset = float2(-(window().width / 2 - 150.0f), -(window().height / 2 - 150.0f));
			grapihcs().draw_coordinates(float3::ZERO, float3::FORWARD * 3.0f, float3::UP * 3.0f, float3::RIGHT * 3.0f, world_debug_cam->view_matrix(), world_debug_cam->projection_matrix(), offset);
			grapihcs().draw_coordinates(float3::ZERO, float3::FORWARD * 3.0f, float3::UP * 3.0f, float3::RIGHT * 3.0f, main_cam->view_matrix(), main_cam->projection_matrix());
	
			float3 pos = main_cam->position;
			float3 forward = main_cam->forward;
			float3 right = main_cam->right;
			float3 up = main_cam->up;
			grapihcs().draw_coordinates(pos, forward, up, right, world_debug_cam->view_matrix(), world_debug_cam->projection_matrix(), offset);

			world_debug_cam->position = (float3(1.0f, 1.0f, -1.0f) * debug_world_cam_distance);
			world_debug_cam->lookat(float3::ZERO);
		}

		void render() {
			for (auto& obj : objects) {
				obj->render();
			}

			for (auto& obj : transparent_objects) {
				obj->render();
			}

			draw_world_coords();
			draw_camera_coords();
		}
	};
}
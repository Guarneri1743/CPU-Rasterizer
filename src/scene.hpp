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
		std::vector<std::unique_ptr<renderer>> objects;
		std::vector<std::unique_ptr<renderer>> transparent_objects;
		std::shared_ptr<camera> main_cam;

		// todo: serialzie & deserialize scene data
		void initialize() {
			main_cam = std::move(camera::create(float3(5.0f, 5.0f, 5.0f), window().aspect, 45.0f, 0.5f, 500.0f, projection::perspective));
			uint32_t id = main_cam->get_id();
			input_mgr().add_on_mouse_move_evt([](float2 prev, float2 pos, void* data) {
			if (input_mgr().is_mouse_down(mouse_button::right)) {
				float2 offset = (pos - prev) * float2(window().width, window().height) * CAMERA_ROTATE_SPEED;
				std::shared_ptr<camera>* cam = (std::shared_ptr<camera>*)(data);
				(*cam)->rotate(offset.x, offset.y);
			}
			}, &main_cam);

			input_mgr().add_on_key_down_evt([](key_code pos, void* data) {

			}, nullptr);
		}

		void add(std::unique_ptr<renderer> target, bool transparent) {
			if (transparent) {
				transparent_objects.push_back(std::move(target));
			}
			else {
				objects.push_back(std::move(target));
			}
		}

		void update() {
			misc_param.cam_far = main_cam->far;
			misc_param.cam_near = main_cam->near;
			misc_param.view_matrix = main_cam->view_matrix();
			misc_param.proj_matrix = main_cam->projection_matrix();
			misc_param.screen_width = window().width;
			misc_param.screen_height = window().height;

			if (input_mgr().is_key_down(key_code::W)) {
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
		}

		void render() {
			for (auto& obj : objects) {
				obj->render();
			}

			for (auto& obj : transparent_objects) {
				obj->render();
			}
		}
	};
}
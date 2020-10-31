#pragma once
#include <common.hpp>
#include <float3.hpp>
#include <mat4.hpp>

namespace guarneri {
	class camera
	{
	public:
		enum class projection {
			perspective,
			orthographic
		};

	public:
		float fov;
		float aspect;
		float near;
		float far;

	private:
		mat4 projection_matrix;
		float3 position;
		float3 lookat_target;
		float3 front;
		float3 right;
		float3 up;
		float yaw;
		float pitch;
		projection proj_type;

	public:
		void initialize(const float3& position_t, const float& aspect_t, const float& fov_t, const float& near_t, const float& far_t, const projection& proj_type_t) {
			this->position = position_t;
			this->aspect = aspect_t;
			this->fov = fov_t;
			this->near = near_t;
			this->far = far_t;
			this->yaw = -90.0f;
			this->pitch = 0.0f;
			this->proj_type = proj_type_t;
			update_camera();
			update_proj_mode();
		}

		mat4 view_matrix() {
			return mat4::lookat_matrix(this->position, this->lookat_target, float3::UP);
		}

		void set_target(const float3& target) {
			this->lookat_target = target;
		}

		const mat4& get_projection_matrix() const{
			return projection_matrix;
		}

		void set_position(const float3& pos) {
			this->position = pos;
		}

		void move(const float3& t) {
			this->position += t;
		}

		void rotate(const float& yaw_offset, const float& pitch_offset) {
			this->yaw += yaw_offset;
			this->pitch += pitch_offset;

			if (this->pitch > 89.0f)
			{
				this->pitch = 89.0f;
			}
			if (this->pitch < -89.0f)
			{
				this->pitch = -89.0f;
			}
			update_camera();
		}

		void update_camera() {
			/*float3 target_front;
			target_front.x = cos(DEGREE2RAD(this->pitch)) * cos(DEGREE2RAD(this->yaw));
			target_front.y = sin(DEGREE2RAD(this->pitch));
			target_front.z = cos(DEGREE2RAD(this->pitch)) * sin(DEGREE2RAD(this->yaw));
			target_front = float3::normalize(target_front);
			float3 target_right = float3::normalize(float3::cross(target_front, float3::UP));
			float3 target_up = float3::normalize(float3::cross(target_right, target_front));
			this->front = target_front;
			this->right = target_right;
			this->up = target_up;*/


		}

		//todo: ortho
		void update_proj_mode(){
			switch (this->proj_type) {
			case projection::perspective:
				this->projection_matrix = mat4::perspective(this->fov, this->aspect, this->near, this->far);
				break;
			case projection::orthographic:
				this->projection_matrix = mat4::perspective(this->fov, this->aspect, this->near, this->far);
				break;
			default:
				this->projection_matrix = mat4::perspective(this->fov, this->aspect, this->near, this->far);
			}
		}
	};
}
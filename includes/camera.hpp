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
		camera(const float3& position, const float& aspect, const float& fov, const float& near, const float& far, const projection& proj_type) {
			this->position = position;
			this->aspect = aspect;
			this->fov = fov;
			this->near = near;
			this->far = far;
			this->yaw = -90.0f;
			this->pitch = 0.0f;
			this->proj_type = proj_type;
			update_camera();
		}

	public:
		float fov;
		float aspect;
		float near;
		float far;

	private:
		mat4 projection_matrix;
		float3 position;
		float3 target;
		float3 front;
		float3 right;
		float3 up;
		float yaw;
		float pitch;
		projection proj_type;

	public:
		mat4 view_matrix() {
			return mat4::lookat_matrix(this->position, this->position + this->front);
		}

		const mat4& get_projection_matrix() const{
			return projection_matrix;
		}

		void update_camera() {
			float3 target_front;
			target_front.x = cos(DEGREE2RAD(this->pitch)) * cos(DEGREE2RAD(this->yaw));
			target_front.y = sin(DEGREE2RAD(this->pitch));
			target_front.z = cos(DEGREE2RAD(this->pitch)) * sin(DEGREE2RAD(this->yaw));
			target_front = float3::normalize(target_front);
			float3 target_right = float3::normalize(float3::cross(target_front, float3::UP));
			float3 targetUp = float3::normalize(float3::cross(target_right, target_front));
			this->front = target_front;
			this->right = target_right;
			this->up = targetUp;
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
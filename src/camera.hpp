#pragma once
#include <guarneri.hpp>
#include <singleton.hpp>

namespace guarneri {
	class camera : public object {
	public:
		~camera(){}

	public:
		float fov;
		float aspect;
		float near;
		float far;
		float3 position;
		float3 front;
		float3 right;
		float3 up;

	private:
		mat4 proj_matrix;
		transform local2world; //todo
		float yaw;
		float pitch;
		projection proj_type;

	public:
		static std::unique_ptr<camera> create(const float3& position_t, const float& aspect_t, const float& fov_t, const float& near_t, const float& far_t, const projection& proj_type_t) {
			auto ret = std::make_unique<camera>();
			ret->initialize(position_t, aspect_t, fov_t, near_t, far_t, proj_type_t);
			return ret;
		}

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

		mat4 view_matrix() const {
			return mat4::lookat_matrix(this->position, this->position + this->front, this->up);
		}

		void set_target(const float3& target) {
			this->front = float3::normalize(target - this->position);
		}

		const mat4 projection_matrix() const{
			return proj_matrix;
		}

		void move(const float3& t) {
			this->position += t;
		}

		void move_forward(const float& distance) {
			this->position += distance * this->front;
		}

		void move_backward(const float& distance) {
			this->position -= distance * this->front;
		}

		void move_left(const float& distance) {
			this->position -= distance * this->right;
		}

		void move_right(const float& distance) {
			this->position += distance * this->right;
		}

		void move_ascend(const float& distance) {
			this->position += distance * this->up;
		}

		void move_descend(const float& distance) {
			this->position -= distance * this->up;
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
			float3 target_front;
			target_front.x = cos(DEGREE2RAD(this->pitch)) * cos(DEGREE2RAD(this->yaw));
			target_front.y = sin(DEGREE2RAD(this->pitch));
			target_front.z = cos(DEGREE2RAD(this->pitch)) * sin(DEGREE2RAD(this->yaw));
			target_front = float3::normalize(target_front);
			float3 target_right = float3::normalize(float3::cross(target_front, float3::UP));
			float3 target_up = float3::normalize(float3::cross(target_right, target_front));
			this->front = target_front;
			this->right = target_right;
			this->up = target_up;
		}

		//todo: ortho
		void update_proj_mode(){
			switch (this->proj_type) {
			case projection::perspective:
				this->proj_matrix = mat4::perspective(this->fov, this->aspect, this->near, this->far);
				break;
			case projection::orthographic:
				this->proj_matrix = mat4::perspective(this->fov, this->aspect, this->near, this->far);
				break;
			default:
				this->proj_matrix = mat4::perspective(this->fov, this->aspect, this->near, this->far);
			}
		}

		public:
			std::string str() const {
				std::stringstream ss;
				ss << "Camera[" << this->id << "  pos: "<<this->position << "]";
				return ss.str();
			}
	};
}
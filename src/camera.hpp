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
		// todo: transform transform; 
		Vector3 forward;
		Vector3 right;
		Vector3 up;
		Vector3 position;

	private:
		Matrix4x4 p;
		Matrix4x4 v;
		float yaw;
		float pitch;
		projection proj_type;

	public:
		static std::unique_ptr<camera> create(const Vector3& position_t, const float& aspect_t, const float& fov_t, const float& near_t, const float& far_t, const projection& proj_type_t) {
			auto ret = std::make_unique<camera>();
			ret->initialize(position_t, aspect_t, fov_t, near_t, far_t, proj_type_t);
			return ret;
		}

		void initialize(const Vector3& position_t, const float& aspect_t, const float& fov_t, const float& near_t, const float& far_t, const projection& proj_type_t) {
			this->position = position_t;
			this->aspect = aspect_t;
			this->fov = fov_t;
			this->near = near_t;
			this->far = far_t;
			this->yaw = -210.0f;
			this->pitch = -45.0f;
			this->proj_type = proj_type_t;
			update_camera();
			update_proj_mode();
		}

		void rotate(const float& yaw_offset, const float& pitch_offset) {
			this->yaw -= yaw_offset;
			this->pitch -= pitch_offset;
			this->pitch = std::clamp(this->pitch, -89.0f, 89.0f);
			update_camera();
		}

		void lookat(const Vector3& target) {
			this->forward = Vector3::normalize(target - this->position);
			Vector3::calculate_right_up(forward, right, up);
		}

		void update_camera() {
			forward.x = cos(DEGREE2RAD(this->yaw)) * cos(DEGREE2RAD(this->pitch));
			forward.y = sin(DEGREE2RAD(this->pitch));
			forward.z = sin(DEGREE2RAD(this->yaw)) * cos(DEGREE2RAD(this->pitch));
			forward = Vector3::normalize(forward);
			Vector3::calculate_right_up(forward, right, up);
		}

		Matrix4x4 view_matrix() const {
			return  Matrix4x4::lookat(position, position + forward, Vector3::UP);
		}

		const Matrix4x4 projection_matrix() const{
			return p;
		}

		void move(const Vector3& t) {
			this->position += t;
		}

		void move_forward(const float& distance) {
			this->position += distance * this->forward;
		}

		void move_backward(const float& distance) {
			this->position -= distance * this->forward;
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

		//todo: ortho
		void update_proj_mode(){
			switch (this->proj_type) {
			case projection::perspective:
				this->p = Matrix4x4::perspective(this->fov, this->aspect, this->near, this->far);
				break;
			case projection::orthographic:
				this->p = Matrix4x4::perspective(this->fov, this->aspect, this->near, this->far);
				break;
			default:
				this->p = Matrix4x4::perspective(this->fov, this->aspect, this->near, this->far);
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
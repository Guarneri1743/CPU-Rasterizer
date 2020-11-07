#pragma once
#include <Guarneri.hpp>
#include <Singleton.hpp>

namespace Guarneri {
	class Camera : public Object {
	public:
		~Camera(){}

	public:
		float fov;
		float aspect;
		float near;
		float far;
		// todo: Transform Transform; 
		Vector3 forward;
		Vector3 right;
		Vector3 up;
		Vector3 position;

	private:
		Matrix4x4 p;
		Matrix4x4 v;
		float yaw;
		float pitch;
		Projection proj_type;

	public:
		static std::unique_ptr<Camera> create(const Vector3& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far, const Projection& _proj_type) {
			auto ret = std::make_unique<Camera>();
			ret->initialize(_position, _aspect, _fov, _near, _far, _proj_type);
			return ret;
		}

		void initialize(const Vector3& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far, const Projection& _proj_type) {
			this->position = _position;
			this->aspect = _aspect;
			this->fov = _fov;
			this->near = _near;
			this->far = _far;
			this->yaw = -210.0f;
			this->pitch = -45.0f;
			this->proj_type = _proj_type;
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
			case Projection::PERSPECTIVE:
				this->p = Matrix4x4::perspective(this->fov, this->aspect, this->near, this->far);
				break;
			case Projection::ORTHO:
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
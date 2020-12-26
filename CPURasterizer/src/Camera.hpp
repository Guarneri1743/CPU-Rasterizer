#ifndef _CAMERA_
#define _CAMERA_
#include <CPURasterizer.hpp>

namespace Guarneri
{
	class Camera : public Object
	{
	public:
		float fov;
		float aspect;
		float near;
		float far;
		Vector3 forward;
		Vector3 right;
		Vector3 up;
		Vector3 position;
		Matrix4x4 p;
		Matrix4x4 v;
		float yaw;
		float pitch;
		Projection projection;

	public:
		Camera();
		~Camera();
		static std::unique_ptr<Camera> create(const Vector3& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far);
		void initialize(const Vector3& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far, const Projection& _proj_type);
		void rotate(const float& yaw_offset, const float& pitch_offset);
		void lookat(const Vector3& target);
		void update_camera();
		Matrix4x4 view_matrix() const;
		const Matrix4x4 projection_matrix() const;
		void move(const Vector3& t);
		void move_forward(const float& distance);
		void move_backward(const float& distance);
		void move_left(const float& distance);
		void move_right(const float& distance);
		void move_ascend(const float& distance);
		void move_descend(const float& distance);
		void set_near(const float& _near);
		void set_far(const float& _far);
		void set_fov(const float& _fov);
		void set_projection(const Projection& proj);
		void update_proj_mode();
		std::string str() const;
	};


	Camera::Camera()
	{}

	Camera::~Camera()
	{}

	std::unique_ptr<Camera> Camera::create(const Vector3& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far)
	{
		auto ret = std::make_unique<Camera>();
		ret->initialize(_position, _aspect, _fov, _near, _far, Projection::PERSPECTIVE);
		return ret;
	}

	void Camera::initialize(const Vector3& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far, const Projection& _proj_type)
	{
		this->position = _position;
		this->aspect = _aspect;
		this->fov = _fov;
		this->near = _near;
		this->far = _far;
		this->yaw = -210.0f;
		this->pitch = -45.0f;
		this->projection = _proj_type;
		update_camera();
		update_proj_mode();
	}

	void Camera::rotate(const float& yaw_offset, const float& pitch_offset)
	{
		this->yaw -= yaw_offset;
		this->pitch -= pitch_offset;
		this->pitch = std::clamp(this->pitch, -89.0f, 89.0f);
		update_camera();
	}

	void Camera::lookat(const Vector3& target)
	{
		this->forward = Vector3::normalize(target - this->position);
		Vector3::calculate_right_up(forward, right, up);
	}

	void Camera::update_camera()
	{
		forward.x = cos(DEGREE2RAD(this->yaw)) * cos(DEGREE2RAD(this->pitch));
		forward.y = sin(DEGREE2RAD(this->pitch));
		forward.z = sin(DEGREE2RAD(this->yaw)) * cos(DEGREE2RAD(this->pitch));
		forward = Vector3::normalize(forward);
		Vector3::calculate_right_up(forward, right, up);
	}

	Matrix4x4 Camera::view_matrix() const
	{
		return  Matrix4x4::lookat(position, position + forward, Vector3::UP);
	}

	const Matrix4x4 Camera::projection_matrix() const
	{
		return p;
	}

	void Camera::move(const Vector3& t)
	{
		this->position += t;
	}

	void Camera::move_forward(const float& distance)
	{
		this->position += distance * this->forward;
	}

	void Camera::move_backward(const float& distance)
	{
		this->position -= distance * this->forward;
	}

	void Camera::move_left(const float& distance)
	{
		this->position -= distance * this->right;
	}

	void Camera::move_right(const float& distance)
	{
		this->position += distance * this->right;
	}

	void Camera::move_ascend(const float& distance)
	{
		this->position += distance * this->up;
	}

	void Camera::move_descend(const float& distance)
	{
		this->position -= distance * this->up;
	}

	void Camera::set_near(const float& _near)
	{
		near = _near;
		update_proj_mode();
	}

	void Camera::set_far(const float& _far)
	{
		far = _far;
		update_proj_mode();
	}

	void Camera::set_fov(const float& _fov)
	{
		fov = _fov;
		update_proj_mode();
	}

	void Camera::set_projection(const Projection& proj)
	{
		this->projection = proj;
		update_proj_mode();
	}

	void Camera::update_proj_mode()
	{
		switch (this->projection)
		{
		case Projection::PERSPECTIVE:
			this->p = Matrix4x4::perspective(this->fov, this->aspect, this->near, this->far);
			break;
		case Projection::ORTHO:
			this->p = Matrix4x4::ortho(-20.0f, 20.0f, -15.0f, 15.0f, this->near, this->far);
			break;
		default:
			this->p = Matrix4x4::perspective(this->fov, this->aspect, this->near, this->far);
		}
	}

	std::string Camera::str() const
	{
		std::stringstream ss;
		ss << "Camera[" << this->id << "  pos: " << this->position << "]";
		return ss.str();
	}
}
#endif
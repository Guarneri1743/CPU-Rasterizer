#include "Camera.hpp"
#include <sstream>
#include <iostream>

namespace Guarneri
{
	Camera* Camera::main_cam;

	Camera::Camera()
	{
		transform = std::make_unique<Transform>();
	}

	Camera::~Camera()
	{}

	std::unique_ptr<Camera> Camera::create(const tinymath::vec3f& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far)
	{
		auto ret = std::unique_ptr<Camera>(new Camera());
		ret->initialize(_position, _aspect, _fov, _near, _far, Projection::kPerspective);
		return ret;
	}

	void Camera::initialize(const tinymath::vec3f& position, const float& _aspect, const float& _fov, const float& _near, const float& _far, const Projection& _proj_type)
	{
		this->aspect = _aspect > 0.0f ? _aspect : 1.0f;
		this->fov = _fov;
		this->near = _near;
		this->far = _far;
		this->transform->set_world_position(position);
		this->transform->set_world_angle(0.0f, 90.0f, 0.0f);
		this->transform->set_local_scale(tinymath::kVec3fOne);
		this->projection = _proj_type;
		update_proj_mode();
	}

	tinymath::mat4x4 Camera::view_matrix() const
	{		
		return  tinymath::lookat(transform->world_position(), transform->world_position() + transform->forward(), tinymath::kVec3fUp);
	}

	const tinymath::mat4x4 Camera::projection_matrix() const
	{
		return proj_matrix;
	}

	void Camera::focus(const tinymath::vec3f& position)
	{
		this->transform->lookat(position);
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
		case Projection::kPerspective:
			this->proj_matrix = tinymath::perspective(this->fov, this->aspect, this->near, this->far);
			break;
		case Projection::kOrtho:
			this->proj_matrix = tinymath::ortho(-20.0f, 20.0f, -15.0f, 15.0f, this->near, this->far);
			break;
		default:
			this->proj_matrix = tinymath::perspective(this->fov, this->aspect, this->near, this->far);
		}
	}
}
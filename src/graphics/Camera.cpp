#include "Camera.hpp"
#include <sstream>
#include <iostream>
#include "Transform.hpp"

namespace CpuRasterizer
{
	Camera* Camera::main_cam;

	Camera::Camera()
	{
		transform = std::make_unique<Transform>();
	}

	Camera::~Camera()
	{}

	std::unique_ptr<Camera> Camera::create(const tinymath::vec3f& _position, const tinymath::FrustumParam& frustum)
	{
		auto ret = std::unique_ptr<Camera>(new Camera());
		ret->initialize(_position, frustum);
		return ret;
	}

	void Camera::initialize(const tinymath::vec3f& position, const tinymath::FrustumParam& frustum)
	{
		this->frustum_param = frustum;
		this->transform->set_world_position(position);
		this->transform->set_world_angle(0.0f, 90.0f, 0.0f);
		this->transform->set_local_scale(tinymath::kVec3fOne);
		update_projection_matrix();
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

	void Camera::set_near(float near)
	{
		if (frustum_param.projection_mode == tinymath::Projection::kPerspective)
		{
			frustum_param.perspective_param.near = near;
		}
		else
		{
			frustum_param.ortho_param.near = near;
		}
		update_projection_matrix();
	}
	
	void Camera::set_far(float far)
	{
		if (frustum_param.projection_mode == tinymath::Projection::kPerspective)
		{
			frustum_param.perspective_param.far = far;
		}
		else
		{
			frustum_param.ortho_param.far = far;
		}
		update_projection_matrix();
	}

	void Camera::set_fov(float fov)
	{
		if (frustum_param.projection_mode == tinymath::Projection::kPerspective)
		{
			frustum_param.perspective_param.fov = fov;
		}
		update_projection_matrix();
	}

	void Camera::set_projection(tinymath::Projection proj)
	{
		frustum_param.projection_mode = proj;
		update_projection_matrix();
	}

	void Camera::update_projection_matrix()
	{
		switch (this->frustum_param.projection_mode)
		{
		case tinymath::Projection::kPerspective:
		default:
			this->proj_matrix = tinymath::perspective(this->frustum_param.perspective_param.fov, 
													  this->frustum_param.perspective_param.aspect, 
													  this->frustum_param.perspective_param.near,
													  this->frustum_param.perspective_param.far);
			break;
		case tinymath::Projection::kOrtho:
			this->proj_matrix = tinymath::ortho(this->frustum_param.ortho_param.left, 
												this->frustum_param.ortho_param.right, 
												this->frustum_param.ortho_param.bottom, 
												this->frustum_param.ortho_param.top, 
												this->frustum_param.ortho_param.near, 
												this->frustum_param.ortho_param.far);
			break;
		}
	}
}
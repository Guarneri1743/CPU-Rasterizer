#include "Camera.hpp"
#include <sstream>
#include <iostream>

namespace Guarneri
{
	Camera::Camera()
	{
		transform = std::make_unique<Transform>();
	}

	Camera::~Camera()
	{}

	std::unique_ptr<Camera> Camera::create(const Vector3& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far)
	{
		auto ret = std::unique_ptr<Camera>(new Camera());
		ret->initialize(_position, _aspect, _fov, _near, _far, Projection::PERSPECTIVE);
		return ret;
	}

	void Camera::initialize(const Vector3& position, const float& _aspect, const float& _fov, const float& _near, const float& _far, const Projection& _proj_type)
	{
		this->aspect = _aspect > 0.0f ? _aspect : 1.0f;
		this->fov = _fov;
		this->near = _near;
		this->far = _far;
		this->transform->set_world_position(position);
		this->transform->set_world_angle(0.0f, 90.0f, 0.0f);
		this->transform->set_local_scale(Vector3::ONE);
		this->projection = _proj_type;
		update_proj_mode();
	}

	Matrix4x4 Camera::view_matrix() const
	{		
		return  Matrix4x4::lookat(transform->world_position(), transform->world_position() + transform->forward(), Vector3::UP);
	}

	const Matrix4x4 Camera::projection_matrix() const
	{
		return proj_matrix;
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
			this->proj_matrix = Matrix4x4::perspective(this->fov, this->aspect, this->near, this->far);
			break;
		case Projection::ORTHO:
			this->proj_matrix = Matrix4x4::ortho(-20.0f, 20.0f, -15.0f, 15.0f, this->near, this->far);
			break;
		default:
			this->proj_matrix = Matrix4x4::perspective(this->fov, this->aspect, this->near, this->far);
		}
	}

	rapidjson::Value Camera::serialize(rapidjson::Document& doc, const Camera& cam)
	{
		rapidjson::Value v;
		v.SetObject();
		v.AddMember("fov", cam.fov, doc.GetAllocator());
		v.AddMember("aspect", cam.aspect, doc.GetAllocator());
		v.AddMember("near", cam.near, doc.GetAllocator());
		v.AddMember("far", cam.far, doc.GetAllocator());
		v.AddMember("enable_msaa", cam.enable_msaa, doc.GetAllocator());
		v.AddMember("transform", Transform::serialize(doc, *cam.transform), doc.GetAllocator());
		v.AddMember("projection", (int32_t)cam.projection, doc.GetAllocator());
		v.AddMember("proj_matrix", Matrix4x4::serialize(doc, cam.proj_matrix), doc.GetAllocator()); // ??????????
		return v;
	}

	std::unique_ptr<Camera> Camera::deserialize(const rapidjson::Value& v)
	{
		std::unique_ptr<Camera> cam = std::unique_ptr<Camera>(new Camera());
		cam->fov = v["fov"].GetFloat();
		cam->aspect = v["aspect"].GetFloat();
		cam->near = v["near"].GetFloat();
		cam->far = v["far"].GetFloat();
		cam->enable_msaa = v["enable_msaa"].GetBool();
		cam->proj_matrix = Matrix4x4::deserialize(v["proj_matrix"].GetObject());
		cam->transform = std::unique_ptr<Transform>(Transform::deserialize(v["transform"].GetObject()));
		cam->projection = (Projection)v["projection"].GetInt();
		return cam;
	}

	std::string Camera::str() const
	{
		std::stringstream ss;
		ss << "Camera[" << this->id << "  pos: " << this->transform->world_position().str() << "]";
		return ss.str();
	}
}
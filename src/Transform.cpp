#include "Transform.hpp"

namespace Guarneri
{
	Transform::Transform()
	{
		rotationTheta = 0.0f;
		local2world = Matrix4x4::IDENTITY;
	}

	Transform::Transform(const Transform& other)
	{
		rotationTheta = 0.0f;
		this->local2world = other.local2world;
	}

	Vector3 Transform::forward() const
	{
		return local2world.forward();
	}

	Vector3 Transform::up() const
	{
		return local2world.up();
	}

	Vector3 Transform::right()  const
	{
		return local2world.right();
	}

	void Transform::translate(const Vector3& translation)
	{
		this->position += translation;
		sync();
	}

	void Transform::move_forward(const float& distance)
	{
		Vector3 delta = distance * this->forward();
		this->position += delta;
		sync();
	}

	void Transform::move_backward(const float& distance)
	{
		Vector3 delta = -distance * this->forward();
		this->position += delta;
		sync();
	}

	void Transform::move_left(const float& distance)
	{
		Vector3 delta = -distance * this->right();
		this->position += delta;
		sync();
	}

	void Transform::move_right(const float& distance)
	{
		Vector3 delta = distance * this->right();
		this->position += delta;
		sync();
	}

	void Transform::move_ascend(const float& distance)
	{
		Vector3 delta = distance * this->up();
		this->position += delta;
		sync();
	}

	void Transform::move_descend(const float& distance)
	{
		Vector3 delta = -distance * this->up();
		this->position += delta;
		sync();
	}

	void Transform::rotate(const Vector3& _axis, const float& angle)
	{
		this->axis = _axis;
		this->rotationTheta += angle;
		sync();
	}

	void Transform::scale(const Vector3& _scale)
	{
		this->local_scale = _scale;
		sync();
	}

	void Transform::sync()
	{
		auto r = Matrix4x4::rotation(axis, rotationTheta);
		auto t = Matrix4x4::translation(position);
		auto s = Matrix4x4::scale(local_scale);
		this->local2world = t * r * s;
	}

	Transform& Transform::operator=(const Transform& other)
	{
		this->local2world = other.local2world;
		return *this;
	}
}
#include "Transform.hpp"
#include <iostream>
#include "Marcos.h"

namespace CpuRasterizor
{
	Transform::Transform()
	{
		parent = nullptr;
		rotation_angle = 0.0f;
		rotation_axis = tinymath::kVec3fUp;
		local_rotation = tinymath::kVec3fZero;
		local_position = tinymath::kVec3fZero;
		local_scale = tinymath::kVec3fOne;
		world_trs = tinymath::kMat4x4Identity;
		selected = false;
	}

	Transform::Transform(const Transform& other)
	{
		parent = nullptr;
		local_position = other.local_position;
		local_rotation = other.local_rotation;
		local_scale = other.local_scale;
		rotation_axis = other.rotation_axis;
		rotation_angle = other.rotation_angle;
		world_trs = other.world_trs;
		selected = other.selected;
	}

	tinymath::vec3f Transform::forward() const
	{
		return world_trs.forward();
	}

	tinymath::vec3f Transform::up() const
	{
		return world_trs.up();
	}

	tinymath::vec3f Transform::right()  const
	{
		return world_trs.right();
	}

	tinymath::vec3f Transform::world_position() const
	{
		tinymath::vec3f ret = local_position;
		Transform* t = parent;
		while (t != nullptr)
		{
			ret = t->transform_pos(ret);
			t = t->parent;
		}
		return ret;
	}

	tinymath::vec3f Transform::world_euler_angles() const
	{
		tinymath::vec3f ret = local_rotation;
		Transform* t = parent;
		while (t != nullptr)
		{
			ret = t->transform_dir(ret);
			t = t->parent;
		}
		return ret;
	}

	tinymath::vec3f Transform::world_scale() const
	{
		tinymath::vec3f lossy = local_scale;
		Transform* t = parent;
		while (t != nullptr)
		{
			lossy *= t->world_trs.get_scale();
			t = t->parent;
		}
		return lossy;
	}

	tinymath::vec3f Transform::transform_pos(const tinymath::vec3f& point)
	{
		tinymath::vec4f ret = this->world_trs * tinymath::vec4f(point.x, point.y, point.z, 1.0f);
		return tinymath::vec3f(ret.x, ret.y, ret.z);
	}

	tinymath::vec3f Transform::transform_dir(const tinymath::vec3f & dir)
	{
		tinymath::mat4x4 rot = this->world_trs * tinymath::from_euler(dir);
		return tinymath::normalize(rot.to_euler());
	}

	tinymath::vec3f Transform::inverse_transform_pos(const tinymath::vec3f& point)
	{
		tinymath::vec4f ret = tinymath::inverse(this->world_trs) * tinymath::vec4f(point.x, point.y, point.z, 1.0f);
		return tinymath::vec3f(ret.x, ret.y, ret.z);
	}

	tinymath::vec3f Transform::inverse_transform_dir(const tinymath::vec3f& dir)
	{
		tinymath::mat4x4 rot = tinymath::inverse(this->world_trs) * tinymath::from_euler(dir);
		return tinymath::normalize(rot.to_euler());
	}

	void Transform::lookat(const tinymath::vec3f& target)
	{
		this->world_trs = tinymath::lookat(this->world_position(), target, tinymath::kVec3fUp);
	}

	void Transform::set_parent(Transform* p)
	{
		if (this->parent == p) return;
		this->parent = p;
		if (this->parent != nullptr)
		{
			this->parent->children.push_back(this);
		}
	}

	void Transform::set_world_position(const tinymath::vec3f& world_position)
	{
		tinymath::vec3f local_pos = world_position;
		Transform* t = parent;
		while (t != nullptr)
		{
			local_pos = t->inverse_transform_pos(local_pos);
			t = t->parent;
		}
		local_position = local_pos;
	}

	void Transform::set_world_angle(float pitch, float yaw, float roll)
	{
		set_world_rotation({ DEGREE2RAD(pitch), DEGREE2RAD(yaw), DEGREE2RAD(roll) });
	}

	void Transform::set_world_rotation(const tinymath::vec3f & world_rotation)
	{
		tinymath::vec3f local_euler = world_rotation;
		Transform* t = parent;
		while (t != nullptr)
		{
			local_euler = t->inverse_transform_dir(local_euler);
			t = t->parent;
		}
		local_rotation = local_euler; 
		sync();
	}

	void Transform::set_local_position(const tinymath::vec3f& position)
	{
		this->local_position = position;
		sync();
	}

	void Transform::set_local_rotation(const tinymath::vec3f & rotation)
	{
		this->local_rotation = rotation;
		sync();
	}

	void Transform::set_local_scale(const tinymath::vec3f& scale)
	{
		this->local_scale = scale;
		sync();
	}

	void Transform::move_forward(float distance)
	{
		tinymath::vec3f delta = distance * this->forward();
		this->local_position += delta;
		sync();
	}

	void Transform::move_backward(float distance)
	{
		tinymath::vec3f delta = -distance * this->forward();
		this->local_position += delta;
		sync();
	}

	void Transform::move_left(float distance)
	{
		tinymath::vec3f delta = -distance * this->right();
		this->local_position += delta;
		sync();
	}

	void Transform::move_right(float distance)
	{
		tinymath::vec3f delta = distance * this->right();
		this->local_position += delta;
		sync();
	}

	void Transform::move_ascend(float distance)
	{
		tinymath::vec3f delta = distance * this->up();
		this->local_position += delta;
		sync();
	}

	void Transform::move_descend(float distance)
	{
		tinymath::vec3f delta = -distance * this->up();
		this->local_position += delta;
		sync();
	}

	void Transform::rotate(float pitch_offset, float yaw_offset, float roll_offset)
	{
		this->local_rotation.x += DEGREE2RAD(pitch_offset);
		this->local_rotation.y += DEGREE2RAD(yaw_offset);
		this->local_rotation.z += DEGREE2RAD(roll_offset);
		sync();
	}

	void Transform::rotate(const tinymath::vec3f& axis, float angle)
	{
		//todo
		UNUSED(axis);
		UNUSED(angle);
	}

	void Transform::rotate_self(float yaw_offset, float pitch_offset, float roll_offset)
	{
		this->local_rotation.y -= yaw_offset;
		this->local_rotation.x -= pitch_offset;
		this->local_rotation.x = std::clamp(this->local_rotation.x, -89.0f, 89.0f);
		this->local_rotation.z -= roll_offset;
		this->local_rotation.z = std::clamp(this->local_rotation.z, -89.0f, 89.0f);
		tinymath::vec3f forward;
		tinymath::vec3f pos = world_position();
		tinymath::vec3f world_euler = world_euler_angles();
		forward.x = cos(world_euler.y) * cos(world_euler.x);
		forward.y = sin(world_euler.x);
		forward.z = sin(world_euler.y) * cos(world_euler.x);
		forward = tinymath::normalize(forward);
		tinymath::vec3f right, up;
		tinymath::calculate_right_up(forward, right, up);
		auto tr = tinymath::lookat(pos, pos + forward, up);
		auto s = tinymath::scale(world_scale());
		this->world_trs = tr * s;
	}

	void Transform::sync()
	{
		auto t = tinymath::translation(world_position());
		auto r = tinymath::from_euler(world_euler_angles());
		auto s = tinymath::scale(world_scale());
		this->world_trs = t * r * s;
	}

	Transform& Transform::operator=(const Transform& other)
	{
		this->world_trs = other.world_trs;
		return *this;
	}
}
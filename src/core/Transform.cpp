#include "Transform.hpp"
#include <iostream>

namespace Guarneri
{
	Transform::Transform()
	{
		parent = nullptr;
		rotation_angle = 0.0f;
		rotation_axis = Vector3::UP;
		local_rotation = Vector3::ZERO;
		local_position = Vector3::ZERO;
		local_scale = Vector3::ONE;
		world_trs = Matrix4x4::IDENTITY;
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

	Vector3 Transform::forward() const
	{
		return world_trs.forward();
	}

	Vector3 Transform::up() const
	{
		return world_trs.up();
	}

	Vector3 Transform::right()  const
	{
		return world_trs.right();
	}

	Vector3 Transform::world_position() const
	{
		Vector3 ret = local_position;
		Transform* t = parent;
		while (t != nullptr)
		{
			ret = t->transform_pos(ret);
			t = t->parent;
		}
		return ret;
	}

	Vector3 Transform::world_euler_angles() const
	{
		Vector3 ret = local_rotation;
		Transform* t = parent;
		while (t != nullptr)
		{
			ret = t->transform_dir(ret);
			t = t->parent;
		}
		return ret;
	}

	Vector3 Transform::world_scale() const
	{
		Vector3 lossy = local_scale;
		Transform* t = parent;
		while (t != nullptr)
		{
			lossy *= t->world_trs.get_scale();
			t = t->parent;
		}
		return lossy;
	}

	Vector3 Transform::transform_pos(const Vector3& point)
	{
		Vector4 ret = this->world_trs * Vector4(point.x, point.y, point.z, 1.0f);
		return Vector3(ret.x, ret.y, ret.z);
	}

	Vector3 Transform::transform_dir(const Vector3 & dir)
	{
		Matrix4x4 rot = this->world_trs * Matrix4x4::from_euler(dir);
		return rot.to_euler().normalized();
	}

	Vector3 Transform::inverse_transform_pos(const Vector3& point)
	{
		Vector4 ret = this->world_trs.inverse() * Vector4(point.x, point.y, point.z, 1.0f);
		return Vector3(ret.x, ret.y, ret.z);
	}

	Vector3 Transform::inverse_transform_dir(const Vector3& dir)
	{
		Matrix4x4 rot = this->world_trs.inverse() * Matrix4x4::from_euler(dir);
		return rot.to_euler().normalized();
	}

	void Transform::lookat(const Vector3& target)
	{
		this->world_trs = Matrix4x4::lookat(this->world_position(), target, Vector3::UP);
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

	void Transform::set_world_position(const Vector3& world_position)
	{
		Vector3 local_pos = world_position;
		Transform* t = parent;
		while (t != nullptr)
		{
			local_pos = t->inverse_transform_pos(local_pos);
			t = t->parent;
		}
		local_position = local_pos;
	}

	void Transform::set_world_angle(const float& pitch, const float& yaw, const float& roll)
	{
		set_world_rotation({ DEGREE2RAD(pitch), DEGREE2RAD(yaw), DEGREE2RAD(roll) });
	}

	void Transform::set_world_rotation(const Vector3 & world_rotation)
	{
		Vector3 local_euler = world_rotation;
		Transform* t = parent;
		while (t != nullptr)
		{
			local_euler = t->inverse_transform_dir(local_euler);
			t = t->parent;
		}
		local_rotation = local_euler; 
		sync();
	}

	void Transform::set_local_position(const Vector3& position)
	{
		this->local_position = position;
		sync();
	}

	void Transform::set_local_rotation(const Vector3 & rotation)
	{
		this->local_rotation = rotation;
		sync();
	}

	void Transform::set_local_scale(const Vector3& scale)
	{
		this->local_scale = scale;
		sync();
	}

	void Transform::move_forward(const float& distance)
	{
		Vector3 delta = distance * this->forward();
		this->local_position += delta;
		sync();
	}

	void Transform::move_backward(const float& distance)
	{
		Vector3 delta = -distance * this->forward();
		this->local_position += delta;
		sync();
	}

	void Transform::move_left(const float& distance)
	{
		Vector3 delta = -distance * this->right();
		this->local_position += delta;
		sync();
	}

	void Transform::move_right(const float& distance)
	{
		Vector3 delta = distance * this->right();
		this->local_position += delta;
		sync();
	}

	void Transform::move_ascend(const float& distance)
	{
		Vector3 delta = distance * this->up();
		this->local_position += delta;
		sync();
	}

	void Transform::move_descend(const float& distance)
	{
		Vector3 delta = -distance * this->up();
		this->local_position += delta;
		sync();
	}

	void Transform::rotate(const float& pitch_offset, const float& yaw_offset, const float& roll_offset)
	{
		this->local_rotation.x += DEGREE2RAD(pitch_offset);
		this->local_rotation.y += DEGREE2RAD(yaw_offset);
		this->local_rotation.z += DEGREE2RAD(roll_offset);
		sync();
	}

	void Transform::rotate(const Vector3& axis, const float& angle)
	{
		//todo
		UNUSED(axis);
		UNUSED(angle);
	}

	void Transform::rotate_self(const float& yaw_offset, const float& pitch_offset, const float& roll_offset)
	{
		this->local_rotation.y -= yaw_offset;
		this->local_rotation.x -= pitch_offset;
		this->local_rotation.x = std::clamp(this->local_rotation.x, -89.0f, 89.0f);
		this->local_rotation.z -= roll_offset;
		this->local_rotation.z = std::clamp(this->local_rotation.z, -89.0f, 89.0f);
		Vector3 forward;
		Vector3 pos = world_position();
		Vector3 world_euler = world_euler_angles();
		forward.x = cos(world_euler.y) * cos(world_euler.x);
		forward.y = sin(world_euler.x);
		forward.z = sin(world_euler.y) * cos(world_euler.x);
		forward = Vector3::normalize(forward);
		Vector3 right, up;
		Vector3::calculate_right_up(forward, right, up);
		auto tr = Matrix4x4::lookat(pos, pos + forward, up);
		auto s = Matrix4x4::scale(world_scale());
		this->world_trs = tr * s;
	}

	void Transform::sync()
	{
		auto t = Matrix4x4::translation(world_position());
		auto r = Matrix4x4::from_euler(world_euler_angles());
		auto s = Matrix4x4::scale(world_scale());
		this->world_trs = t * r * s;
	}

	Transform& Transform::operator=(const Transform& other)
	{
		this->world_trs = other.world_trs;
		return *this;
	}

	rapidjson::Value Transform::serialize(rapidjson::Document& doc, const Transform& t)
	{
		rapidjson::Value serialized_transform;
		serialized_transform.SetObject();
		rapidjson::Value name;
		name.SetString(t.name.c_str(), doc.GetAllocator());
		rapidjson::Value local_position = Vector3::serialize(doc, t.local_position);
		rapidjson::Value local_rotation = Vector3::serialize(doc, t.local_rotation);
		rapidjson::Value local_scale = Vector3::serialize(doc, t.local_scale);
		rapidjson::Value world_trs = Matrix4x4::serialize(doc, t.world_trs);
		rapidjson::Value rotation_axis = Vector3::serialize(doc, t.rotation_axis);
		rapidjson::Value rotation_angle; rotation_angle.SetFloat(t.rotation_angle);
		serialized_transform.AddMember("name", name, doc.GetAllocator());
		serialized_transform.AddMember("local_position", local_position, doc.GetAllocator());
		serialized_transform.AddMember("local_rotation", local_rotation, doc.GetAllocator());
		serialized_transform.AddMember("local_scale", local_scale, doc.GetAllocator());
		serialized_transform.AddMember("world_trs", world_trs, doc.GetAllocator());
		serialized_transform.AddMember("rotation_axis", rotation_axis, doc.GetAllocator());
		serialized_transform.AddMember("rotation_angle", rotation_angle, doc.GetAllocator());

		rapidjson::Value serialized_children;
		serialized_children.SetArray();
		if (t.children.size() > 0)
		{
			for (auto& child : t.children)
			{
				if (child == nullptr) continue;
				rapidjson::Value cv = Transform::serialize(doc, *child);
				serialized_children.PushBack(cv, doc.GetAllocator());
			}
		}
		serialized_transform.AddMember("children", serialized_children, doc.GetAllocator());
		return serialized_transform;
	}

	Transform* Transform::deserialize(const rapidjson::Value& v)
	{
		auto t = new Transform();
		t->name = v["name"].GetString();
		t->local_position = Vector3::deserialize(v["local_position"].GetObject());
		t->local_rotation = Vector3::deserialize(v["local_rotation"].GetObject());
		t->local_scale = Vector3::deserialize(v["local_scale"].GetObject());
		t->world_trs = Matrix4x4::deserialize(v["world_trs"].GetObject());
		t->rotation_axis = Vector3::deserialize(v["rotation_axis"].GetObject());
		t->rotation_angle = v["rotation_angle"].GetFloat();
		const rapidjson::Value& children_objs = v["children"].GetArray();
		for (rapidjson::SizeType idx = 0; idx < children_objs.Size(); idx++)
		{
			const rapidjson::Value& child_obj = children_objs[idx];
			Transform* child = Transform::deserialize(child_obj);
			child->set_parent(t);
		}
		return t;
	}
}
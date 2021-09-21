#ifndef _TRANSFORM_
#define _TRANSFORM_
#include <vector>
#include <string>
#include "Vector3.hpp"
#include "Matrix4x4.hpp"
#include "rapidjson/document.h"

namespace Guarneri
{
	class Model;
	class Transform
	{
	public:
		std::string name;
		Vector3 local_position;
		Vector3 local_rotation;
		Vector3 local_scale;
		Matrix4x4 world_trs;
		Vector3 rotation_axis;
		float rotation_angle;
		bool selected;

	private:
		Model* model; 
		Transform* parent;
		std::vector<Transform*> children;

	public:
		Transform();
		Transform(const Transform& other);
		Vector3 forward() const;
		Vector3 up() const;
		Vector3 right()  const;
		Vector3 world_position() const;
		Vector3 world_euler_angles() const;
		Vector3 world_scale() const;
		Vector3 transform_pos(const Vector3& point);
		Vector3 transform_dir(const Vector3& dir);
		Vector3 inverse_transform_pos(const Vector3& point);
		Vector3 inverse_transform_dir(const Vector3& dir);
		void set_model(Model* _model) { this->model = _model; }
		Model* get_model() const { return this->model; }
		void lookat(const Vector3& target);
		void set_parent(Transform* parent);
		void set_world_position(const Vector3& world_position);
		void set_world_angle(const float& pitch, const float& yaw, const float& roll);
		void set_world_rotation(const Vector3& world_rotation);
		void set_local_position(const Vector3& position);
		void set_local_rotation(const Vector3& rotation);
		void set_local_scale(const Vector3& _scale);
		void move_forward(const float& distance);
		void move_backward(const float& distance);
		void move_left(const float& distance);
		void move_right(const float& distance);
		void move_ascend(const float& distance);
		void move_descend(const float& distance);
		void rotate(const float& pitch_offset, const float& yaw_offset, const float& roll_offset);
		void rotate(const Vector3& _axis, const float& angle);
		void rotate_self(const float& yaw_offset, const float& pitch_offset, const float& roll_offset);
		void sync();
		Transform* access_child(const size_t& index) const { assert(index < children.size()); return children[index]; }
		size_t child_count() const { return children.size(); }
		Transform& operator =(const Transform& other);

		static rapidjson::Value serialize(rapidjson::Document& doc, const Transform& t);
		static Transform* deserialize(const rapidjson::Value& v);
	};
}
#endif
#pragma once
#include <vector>
#include <string>
#include "Object.hpp"
#include "tinymath.h"

namespace CpuRasterizor
{
	class Model;
	class Transform : public Object
	{
	public:
		std::string name;
		tinymath::vec3f local_position;
		tinymath::vec3f local_rotation;
		tinymath::vec3f local_scale;
		tinymath::mat4x4 world_trs;
		tinymath::vec3f rotation_axis;
		float rotation_angle;
		bool selected;

	private:
		Model* model; 
		Transform* parent;
		std::vector<Transform*> children;

	public:
		Transform();
		Transform(const Transform& other);
		tinymath::vec3f forward() const;
		tinymath::vec3f up() const;
		tinymath::vec3f right()  const;
		tinymath::vec3f world_position() const;
		tinymath::vec3f world_euler_angles() const;
		tinymath::vec3f world_scale() const;
		tinymath::vec3f transform_pos(const tinymath::vec3f& point);
		tinymath::vec3f transform_dir(const tinymath::vec3f& dir);
		tinymath::vec3f inverse_transform_pos(const tinymath::vec3f& point);
		tinymath::vec3f inverse_transform_dir(const tinymath::vec3f& dir);
		void set_model(Model* _model) { this->model = _model; }
		Model* get_model() const { return this->model; }
		void lookat(const tinymath::vec3f& target);
		void set_parent(Transform* parent);
		void set_world_position(const tinymath::vec3f& world_position);
		void set_world_angle(const float& pitch, const float& yaw, const float& roll);
		void set_world_rotation(const tinymath::vec3f& world_rotation);
		void set_local_position(const tinymath::vec3f& position);
		void set_local_rotation(const tinymath::vec3f& rotation);
		void set_local_scale(const tinymath::vec3f& _scale);
		void move_forward(const float& distance);
		void move_backward(const float& distance);
		void move_left(const float& distance);
		void move_right(const float& distance);
		void move_ascend(const float& distance);
		void move_descend(const float& distance);
		void rotate(const float& pitch_offset, const float& yaw_offset, const float& roll_offset);
		void rotate(const tinymath::vec3f& _axis, const float& angle);
		void rotate_self(const float& yaw_offset, const float& pitch_offset, const float& roll_offset);
		void sync();
		Transform* access_child(const size_t& index) const { return children[index]; }
		size_t child_count() const { return children.size(); }
		Transform& operator =(const Transform& other);
	};
}
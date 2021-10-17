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
		void set_world_angle(float pitch, float yaw, float roll);
		void set_world_rotation(const tinymath::vec3f& world_rotation);
		void set_local_position(const tinymath::vec3f& position);
		void set_local_rotation(const tinymath::vec3f& rotation);
		void set_local_scale(const tinymath::vec3f& _scale);
		void move_forward(float distance);
		void move_backward(float distance);
		void move_left(float distance);
		void move_right(float distance);
		void move_ascend(float distance);
		void move_descend(float distance);
		void rotate(float pitch_offset, float yaw_offset, float roll_offset);
		void rotate(const tinymath::vec3f& _axis, float angle);
		void rotate_self(float yaw_offset, float pitch_offset, float roll_offset);
		void sync();
		Transform* access_child(size_t index) const { return children[index]; }
		size_t child_count() const { return children.size(); }
		Transform& operator =(const Transform& other);
	};
}
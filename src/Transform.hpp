#ifndef _TRANSFORM_
#define _TRANSFORM_
#include <Guarneri.hpp>

namespace Guarneri {
	//todo: hierarchical Transform
	struct Transform {
	public:
		Transform() {
			local2world = Matrix4x4::IDENTITY;
		}

		Transform(const Transform& other) {
			this->local2world = other.local2world;
		}

	public:
		Vector3 position;
		Vector3 axis;
		float rotationTheta;
		Vector3 local_scale;
		Matrix4x4 local2world;

	public:
		Vector3 forward() const {
			return local2world.forward();
		}

		Vector3 up() const {
			return local2world.up();
		}

		Vector3 right()  const {
			return local2world.right();
		}

		void translate(const Vector3& translation){
			this->position += translation;
			sync();
		}

		void move_forward(const float& distance) {
			Vector3 delta = distance * this->forward();
			this->position += delta;
			sync();
		}

		void move_backward(const float& distance) {
			Vector3 delta = - distance * this->forward();
			this->position += delta;
			sync();
		}

		void move_left(const float& distance) {
			Vector3 delta = - distance * this->right();
			this->position += delta;
			sync();
		}

		void move_right(const float& distance) {
			Vector3 delta = distance * this->right();
			this->position += delta;
			sync();
		}

		void move_ascend(const float& distance) {
			Vector3 delta = distance * this->up();
			this->position += delta;
			sync();
		}

		void move_descend(const float& distance) {
			Vector3 delta = - distance * this->up();
			this->position += delta;
			sync();
		}

		void rotate(const Vector3& _axis, const float& angle) {
			this->axis = _axis;
			this->rotationTheta += angle;
			sync();
		}

		void scale(const Vector3& _scale) {
			this->local_scale = _scale;
			sync();
		}

		void sync() {
			auto r = Matrix4x4::rotation(axis, rotationTheta);
			auto t = Matrix4x4::translation(position);
			auto s = Matrix4x4::scale(local_scale);
			this->local2world = t * r * s;
		}

		Transform& operator =(const Transform& other) {
			this->local2world = other.local2world;
		}
	};
}
#endif
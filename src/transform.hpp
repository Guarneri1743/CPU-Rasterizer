#pragma once
#include <guarneri.hpp>

namespace guarneri {
	//todo: hierarchical transform
	struct transform {
	public:
		transform() {
			local2world = Matrix4x4::IDENTITY;
		}

		transform(const transform& other) {
			this->local2world = other.local2world;
		}

	public:
		Matrix4x4 local2world;

	public:
		void lookat(const Vector3& pos){
			Vector3 eye = this->position();
			this->local2world = Matrix4x4::lookat(eye, pos, Vector3::UP);
		}

		void set_position(const Vector3& eye) {
			this->local2world = Matrix4x4::lookat(eye, eye + this->forward(), Vector3::UP);
		}

		Vector3 position() const {
			return local2world.position();
		}

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
			Matrix4x4 t = Matrix4x4::translation(translation);
			local2world = t * local2world;
		}

		void move_forward(const float& distance) {
			Vector3 delta = distance * this->forward();
			local2world = Matrix4x4::translation(delta) * local2world;
		}

		void move_backward(const float& distance) {
			Vector3 delta = - distance * this->forward();
			local2world = Matrix4x4::translation(delta) * local2world;
		}

		void move_left(const float& distance) {
			Vector3 delta = - distance * this->right();
			local2world = Matrix4x4::translation(delta) * local2world;
		}

		void move_right(const float& distance) {
			Vector3 delta = distance * this->right();
			local2world = Matrix4x4::translation(delta) * local2world;
		}

		void move_ascend(const float& distance) {
			Vector3 delta = distance * this->up();
			local2world = Matrix4x4::translation(delta) * local2world;
		}

		void move_descend(const float& distance) {
			Vector3 delta = - distance * this->up();
			local2world = Matrix4x4::translation(delta) * local2world;
		}

		void rotate(const Vector3& axis, const float& angle) {
			Matrix4x4 r = Matrix4x4::rotation(axis, angle);
			local2world = r * local2world;
		}

		void scale(const Vector3& scale) {
			Matrix4x4 s = Matrix4x4::scale(scale);
			local2world = s * local2world;
		}

		transform& operator =(const transform& other) {
			this->local2world = other.local2world;
		}
	};
}
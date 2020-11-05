#pragma once
#include <guarneri.hpp>

namespace guarneri {
	//todo: hierarchical transform
	struct transform {
	public:
		transform() {
			local2world = mat4::IDENTITY;
		}

		transform(const transform& other) {
			this->local2world = other.local2world;
		}

	public:
		mat4 local2world;

	public:
		void lookat(const float3& pos){
			float3 eye = this->position();
			this->local2world = mat4::lookat(eye, pos, float3::UP);
		}

		void set_position(const float3& eye) {
			this->local2world = mat4::lookat(eye, eye + this->forward(), float3::UP);
		}

		float3 position() const {
			return local2world.position();
		}

		float3 forward() const {
			return local2world.forward();
		}

		float3 up() const {
			return local2world.up();
		}

		float3 right()  const {
			return local2world.right();
		}

		void translate(const float3& translation){
			mat4 t = mat4::translation(translation);
			local2world = t * local2world;
		}

		void move_forward(const float& distance) {
			float3 delta = distance * this->forward();
			local2world = mat4::translation(delta) * local2world;
		}

		void move_backward(const float& distance) {
			float3 delta = - distance * this->forward();
			local2world = mat4::translation(delta) * local2world;
		}

		void move_left(const float& distance) {
			float3 delta = - distance * this->right();
			local2world = mat4::translation(delta) * local2world;
		}

		void move_right(const float& distance) {
			float3 delta = distance * this->right();
			local2world = mat4::translation(delta) * local2world;
		}

		void move_ascend(const float& distance) {
			float3 delta = distance * this->up();
			local2world = mat4::translation(delta) * local2world;
		}

		void move_descend(const float& distance) {
			float3 delta = - distance * this->up();
			local2world = mat4::translation(delta) * local2world;
		}

		void rotate(const float3& axis, const float& angle) {
			mat4 r = mat4::rotation(axis, angle);
			local2world = r * local2world;
		}

		void scale(const float3& scale) {
			mat4 s = mat4::scale(scale);
			local2world = s * local2world;
		}

		transform& operator =(const transform& other) {
			this->local2world = other.local2world;
		}
	};
}
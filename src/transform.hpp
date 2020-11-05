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
		float3 position() {
			float3 pos;
			pos.x = local2world.at(0, 3);
			pos.y = local2world.at(1, 3);
			pos.z = local2world.at(2, 3);
			return pos;
		}

		float3 forward() {
			float3 f;
			f.x = local2world.at(2, 0);
			f.y = local2world.at(2, 1);
			f.z = local2world.at(2, 2);
			return f;
		}

		float3 up() {
			float3 u;
			u.x = local2world.at(1, 0);
			u.y = local2world.at(1, 1);
			u.z = local2world.at(1, 2);
			return u;
		}

		float3 right() {
			float3 r;
			r.x = local2world.at(0, 0);
			r.y = local2world.at(0, 1);
			r.z = local2world.at(0, 2);
			return r;
		}

		void translate(const float3& translation){
			mat4 t = mat4::translation(translation);
			local2world = t * local2world;
		}

		void rotate(const float3& axis, const float& angle) {
			mat4 r = mat4::rotation(axis, angle);
			local2world = r * local2world;
		}

		void scale(const float3& scale) {
			mat4 s = mat4::scale(scale);
			local2world = s * local2world;
		}

		void operator =(const transform& other) {
			this->local2world = other.local2world;
		}
	};
}
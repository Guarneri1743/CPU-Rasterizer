#pragma once
#include <mat4.hpp>

namespace guarneri {
	//todo: hierarchical transform
	struct transform {
	public:
		transform() {
			local2world = mat4::IDENTITY;
		}

	public:
		mat4 local2world;

	public:
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
	};
}
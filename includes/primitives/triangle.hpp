#pragma once
#include <vertex.hpp>
#include <aabb2d.hpp>

namespace guarneri {
	struct triangle {
	public:
		triangle(const vertex& v1, const vertex& v2, const vertex& v3) {
			vertices[0] = v1;
			vertices[1] = v2;
			vertices[2] = v3;
			cal_aabb();
		}

	public:
		vertex vertices[3];
		aabb2d aabb;

	public:
		aabb2d cal_aabb() {
			float2 v_min = FLT_MAX;
			float2 v_max = FLT_MIN;
			for (int i = 0; i < 3; i++) {
				float x = vertices[i].position.x;
				float y = vertices[i].position.y;
				if (x < v_min.x) {
					v_min.x = x;
				}
				if (y < v_min.y) {
					v_min.y = y;
				}
				if (x > v_max.x) {
					v_max.x = x;
				}
				if (y > v_max.y) {
					v_max.y = y;
				}
			}
			return aabb2d((v_max + v_min) / 2.0f, v_max - v_min);
		}

		vertex& operator[](const unsigned int i) { return vertices[i]; }

		const vertex& operator[](const unsigned int i) const { return vertices[i]; }
	};
}
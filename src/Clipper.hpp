#ifndef _CLIPPER_
#define _CLIPPER_
#include <Guarneri.hpp>

namespace Guarneri {
	class Clipper {
	public:
		static bool cvv_clipping(const Vector4& c1, const Vector4& c2, const Vector4& c3) {
			// z: [-w, w](GL) [0, w](DX)
			// x: [-w, w]
			// y: [-w, w]
			return cvv_clipping(c1) && cvv_clipping(c2) && cvv_clipping(c3);
		}

		static bool cvv_clipping(const Vector4& v) {
			float x, y, z, w;
			x = v.x; y = v.y; z = v.z; w = v.w;
			if (w < misc_param.cam_near) return true;
			if (w > misc_param.cam_far) return true;
			if (x < -w) return true;
			if (x > w) return true;
			if (y < -w) return true;
			if (y > w) return true;
			if (z < -w) return true;
			if (z > w) return true;
			return false;
		}

		// SutherlandHodgeman
		static std::vector<Vertex> poly_clipping(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
			auto frustum = Frustum::create_ndc();
			std::vector<Vertex> list_out = {v1, v2, v3};
			for (int i = 0; i < 6; i++) {
				std::vector<Vertex> list_in(list_out);
				list_out.clear();

				auto plane = frustum[i];

				for (uint32_t cur_idx = 0; cur_idx < list_in.size(); cur_idx++) {
					uint32_t last_idx = (cur_idx + list_in.size() - 1) % list_in.size();

					Vertex cur = list_in[cur_idx];
					Vertex last = list_in[last_idx];

					float d1, d2;
					d1 = plane.distance(cur.position.xyz());
					d2 = plane.distance(last.position.xyz());

					// clip segment
					if (d1 >= 0) {
						list_out.push_back(cur);
						if (d2 < 0) {
							float t = d1 / (d1 - d2);
							list_out.push_back(Vertex::interpolate(cur, last, t));
						}
					}
					else if(d2 >= 0){
						float t = d2 / (d2 - d1);
						list_out.push_back(Vertex::interpolate(last, cur, t));
					}
				}
			}
			return list_out;
		}

		static bool backface_culling(const Vector4& v1, const Vector4& v2, const Vector4& v3) {
			auto v1v2 = v2 - v2;
			auto v1v3 = v3 - v1;
			float ndv = Vector3::dot(Vector3::cross(v1v2.xyz(), v1v3.xyz()), Vector3::BACK);
			return ndv < 0; // front face: ndv > 0
		}
	};
}
#endif
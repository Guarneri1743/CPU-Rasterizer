#ifndef _CLIPPER_
#define _CLIPPER_
#include <Guarneri.hpp>

namespace Guarneri {
	class Clipper {
	public:
		// near plane clipping
		static std::vector<Triangle> poly_clipping(const Frustum& frustum, const Vertex& v1, const Vertex& v2, const Vertex& v3) {
			std::vector<Vertex> list_in = {v1, v2, v3};

			std::vector<Vertex> list_out;
			auto plane = frustum.near;
			for (uint32_t cur_idx = 0; cur_idx < list_in.size(); cur_idx++) {
				uint32_t last_idx = (cur_idx + list_in.size() - 1) % list_in.size();

				Vertex cur = list_in[cur_idx];
				Vertex last = list_in[last_idx];

				float d1, d2;
				d1 = plane.distance(cur.position.xyz());
				d2 = plane.distance(last.position.xyz());
				float t = d1 / (d1 - d2);

				// clip segment
				if (d1 >= 0) {
					if (d2 < 0) {
						list_out.push_back(Vertex::interpolate(cur, last, t));
					}
					list_out.push_back(cur);
				}
				else if(d2 >= 0){
					list_out.push_back(Vertex::interpolate(cur, last, t));
				}
			}

			assert((list_out.size() == 0 || list_out.size() == 3 || list_out.size() == 4));

			std::vector<Triangle> triangles;
			
			if (list_out.size() == 3) {
				triangles.push_back(Triangle(list_out[0], list_out[1], list_out[2]));
			}
			else if(list_out.size() == 4){
				triangles.push_back(Triangle(list_out[0], list_out[1], list_out[2]));
				triangles.push_back(Triangle(list_out[2], list_out[3], list_out[0]));
			}
			else {
				std::cerr << "clipping error, size of vertices: " << list_out.size() << std::endl;
			}
			
			return triangles;
		}

		static bool cvv_clipping(const Vector4& c1, const Vector4& c2, const Vector4& c3) {
			// z: [-w, w](GL) [0, w](DX)
			// x: [-w, w]
			// y: [-w, w]
			return cvv_clipping(c1) && cvv_clipping(c2) && cvv_clipping(c3);
		}

		static bool cvv_clipping(const Vector4& v) {
			float x, y, z, w;
			x = v.x; y = v.y; z = v.z; w = v.w;
			if (x < -w) return true;
			if (x > w) return true;
			if (y < -w) return true;
			if (y > w) return true;
			if (z < -w) return true;
			if (z > w) return true;
			return false;
		}

		static bool backface_culling(const Vector4& v1, const Vector4& v2, const Vector4& v3) {
			auto v1v2 = v2 - v2;
			auto v1v3 = v3 - v1;
			float ndv = Vector3::dot(Vector3::cross(v1v2.xyz(), v1v3.xyz()), Vector3::BACK);
			// front face: ndv >= 0
			// back face: ndv < 0
			return ndv < 0; 
		}
	};
}
#endif
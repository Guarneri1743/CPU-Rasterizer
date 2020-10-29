#pragma once
#include <common.hpp>
#include <vertex.hpp>

namespace guarneri {
	struct triangle {
	public:
		triangle(const vertex verts[3]) {
			for (int i = 0; i < 3; i++) {
				vertices[i] = verts[i];
			}
		}

		triangle(const vertex& v1, const vertex& v2, const vertex& v3) {
			vertices[0] = v1;
			vertices[1] = v2;
			vertices[2] = v3;
		}

	public:
		vertex vertices[3];

	public:
		// scan top/bottom triangle
		// top-left-right | bottom-left-right(flipped)
		//===================================================
		//       top[0]
		//        /\
		//	   	 /  \
		//----------------------------- scanline[screen_y]
		//	   /	  \
		//    /        \
		//    ----------
		// left[1]    right[2]
		//
		// left[1]    right[2]
		//    ----------
		//    \        /
		//	   \      /
		//----------------------------- scanline[screen_y]
		//	     \  /
		//        \/
		//		bottom[0]
		//====================================================
		void interpolate(const float& screen_y, vertex& lhs, vertex& rhs, bool flip) {
			float len = this->vertices[0].position.y - this->vertices[2].position.y;
			len = flip ? len : -len;
			float dy = flip ? screen_y - this->vertices[2].position.y : screen_y - this->vertices[0].position.y;
			float t = dy / len;
			assert(t > 0 && dy > 0);
			int l0, l1, r0, r1;
			l0 = flip ? 1 : 0;
			r0 = flip ? 2 : 0;
			l1 = flip ? 0 : 1;
			r1 = flip ? 0 : 2;
			lhs = vertex::interpolate(this->vertices[l0], this->vertices[l1], t);
			rhs = vertex::interpolate(this->vertices[r0], this->vertices[r1], t);
		}

		// split a triangle to 1-2 triangles
		//===================================================
		//       top[0]
		//        /\
		//	   	 /  \
		//	    /	 \
		//	   /      \
		//     --------
		// left[1]    right[2]
		//
		// left[1]    right[2]
		//     --------
		//	   \      /
		//	    \    /
		//	     \  /
		//        \/
		//	    bottom[0]
		//====================================================
		std::vector<triangle> horizontal_split() {
			std::vector<triangle> ret;

			int max_idx, min_idx;
			float max_y = FLT_MIN;
			float min_y = FLT_MAX;

			// sort
			for (int i = 0; i < 3; i++) {
				float4 p = vertices[i].position;
				if (p.y < min_y) {
					min_y = p.y;
					min_idx = i;
				}
				if (p.y > max_y) {
					max_y = p.y;
					max_idx = i;
				}
			}

			int mid = min_idx == 0 ? (max_idx == 1 ? 2 : 1) : (max_idx == 0 ? (min_idx == 1 ? 2 : 1) : 0);

			vertex sorted[3];

			sorted[0] = vertices[min_idx];
			sorted[1] = vertices[mid];
			sorted[2] = vertices[max_idx];

			// line
			if (sorted[0].position.y == sorted[1].position.y == sorted[2].position.y) {
				return ret;
			}

			// top flat triangle
			if (sorted[0].position.y == sorted[1].position.y) {
				ret.push_back(triangle(sorted));
				return ret;
			}

			// bottom flat triangle
			if (sorted[1].position.y == sorted[2].position.y) {
				ret.push_back(triangle(sorted));
				return ret;
			}
			
			// split triangles
			float mid_y = sorted[1].position.y;

			float t = (mid_y - sorted[0].position.y)/ (sorted[2].position.y - sorted[0].position.y);

			// interpolate new vertex
			vertex v = vertex::interpolate(sorted[0], sorted[2], t);

			// top triangle: top-left-right
			if (v.position.x >= sorted[1].position.x) {
				ret.push_back(triangle(sorted[0], sorted[1], v));
			}
			else {
				ret.push_back(triangle(sorted[0], v, sorted[1]));
			}

			// bottom triangle: bottom-left-right
			if (v.position.x >= sorted[1].position.x) {
				ret.push_back(triangle(sorted[2], sorted[1], v));
			}
			else {
				ret.push_back(triangle(sorted[2], v, sorted[1]));
			}

			for (auto& iter = ret.begin(); iter != ret.end(); iter++) {
				auto& tri = *iter;
				bool valid = EQUALS(tri[0].position.y, tri[1].position.y) || EQUALS(tri[1].position.y, tri[2].position.y);
				if (!valid) {
					std::cerr << tri[0].position.y << ", " << tri[1].position.y << ", " << tri[2].position.y << std::endl;
					std::cerr << std::to_string(EQUALS(tri[0].position.y, tri[1].position.y)) << ", " << std::to_string(EQUALS(tri[1].position.y, tri[2].position.y)) << std::endl;
				}
			}

			return ret;
		}

		vertex& operator[](const unsigned int i) { return vertices[i]; }

		const vertex& operator[](const unsigned int i) const { return vertices[i]; }

		public:
			std::string str() const {
				std::stringstream ss;
				ss << "triangle: [v0: " << this->vertices[0] << ", v1: " << this->vertices[1] << ", v2: " << this->vertices[2] << "]";
				return ss.str();
			}
	};

	static std::ostream& operator << (std::ostream& stream, const triangle& tri) {
		stream << tri.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const triangle& tri) {
		stream << tri.str();
		return stream;
	}
}
#ifndef _TRIANGLE_
#define _TRIANGLE_
#include <Guarneri.hpp>

namespace Guarneri {
	struct Triangle {
	public:
		Triangle(const Vertex verts[3]) {
			for (int i = 0; i < 3; i++) {
				vertices[i] = verts[i];
			}
			flip = false;
		}

		Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
			vertices[0] = v1;
			vertices[1] = v2;
			vertices[2] = v3;
			flip = false;
		}

		Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, const bool& flip) {
			vertices[0] = v1;
			vertices[1] = v2;
			vertices[2] = v3;
			this->flip = flip;
		}

	public:
		Vertex vertices[3];
		bool flip;

	public:
		// scan top/bottom Triangle
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
		void interpolate(const float& screen_y, Vertex& lhs, Vertex& rhs) {
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
			lhs = Vertex::interpolate(this->vertices[l0], this->vertices[l1], t);
			rhs = Vertex::interpolate(this->vertices[r0], this->vertices[r1], t);
		}

		// split a Triangle to 1-2 triangles
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
		std::vector<Triangle> horizontally_split() {
			std::vector<Triangle> ret;

			std::vector<Vertex> sorted;
			sorted.push_back(vertices[0]);
			sorted.push_back(vertices[1]);
			sorted.push_back(vertices[2]);
			std::sort(sorted.begin(), sorted.end(), [](const Vertex& lhs, const Vertex& rhs) { return lhs.position.y < rhs.position.y; });

			assert(sorted[0].position.y <= sorted[1].position.y);
			assert(sorted[1].position.y <= sorted[2].position.y);

			// Line
			if (sorted[0].position.y == sorted[1].position.y && sorted[1].position.y == sorted[2].position.y) {
				return ret;
			}

			// top Triangle
			if (sorted[1].position.y == sorted[2].position.y) {
				if (sorted[1].position.x >= sorted[2].position.x) {
					ret.push_back(Triangle(sorted[0], sorted[2], sorted[1]));
				}
				else {
					ret.push_back(Triangle(sorted[0], sorted[1], sorted[2]));
				}
				return ret;
			}

			// bottom Triangle
			if (sorted[0].position.y == sorted[1].position.y) {
				if (sorted[0].position.x >= sorted[1].position.x) {
					ret.push_back(Triangle(sorted[2], sorted[1], sorted[0], true));
				}
				else {
					ret.push_back(Triangle(sorted[2], sorted[0], sorted[1], true));
				}
				return ret;
			}
			
			// split triangles
			float mid_y = sorted[1].position.y;

			float t = (mid_y - sorted[0].position.y)/ (sorted[2].position.y - sorted[0].position.y);
		
			// interpolate new Vertex
			Vertex v = Vertex::interpolate(sorted[0], sorted[2], t);

			// top Triangle: top-left-right
			if (v.position.x >= sorted[1].position.x) {
				ret.push_back(Triangle(sorted[0], sorted[1], v));
			}
			else {
				ret.push_back(Triangle(sorted[0], v, sorted[1]));
			}

			auto tri = ret[0];
			//assert(EQUALS(tri[1].position.y, tri[2].position.y));
			if (!EQUALS(tri[1].position.y, tri[2].position.y)) {
				//std::cerr << tri[1].position << ",   " << tri[2].position << std::endl;
			}

			// bottom Triangle: bottom-left-right
			if (v.position.x >= sorted[1].position.x) {
				ret.push_back(Triangle(sorted[2], sorted[1], v, true));
			}
			else {
				ret.push_back(Triangle(sorted[2], v, sorted[1], true));
			}

			tri = ret[1];
			//assert(EQUALS(tri[1].position.y, tri[2].position.y));
			if (!EQUALS(tri[1].position.y, tri[2].position.y)) {
				//std::cerr << tri[1].position << ",   " << tri[2].position << std::endl;
			}

			return ret;
		}

		Vertex& operator[](const uint32_t i) { return vertices[i]; }

		const Vertex& operator[](const uint32_t i) const { return vertices[i]; }

		public:
			std::string str() const {
				std::stringstream ss;
				ss << "Triangle: [v0: " << this->vertices[0] << ", v1: " << this->vertices[1] << ", v2: " << this->vertices[2] << "]";
				return ss.str();
			}
	};

	static std::ostream& operator << (std::ostream& stream, const Triangle& tri) {
		stream << tri.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Triangle& tri) {
		stream << tri.str();
		return stream;
	}
}
#endif
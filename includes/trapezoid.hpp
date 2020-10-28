#pragma once
#include <vertex.hpp>

namespace guarneri {
	struct edge {
		vertex v, v1, v2;
	};


	struct trapezoid {
		float top;
		float bottom;
		edge left;
		edge right;

	public:
		void interpolate_lr_edge(float t) {
			float s1 = this->left.v2.position.y - this->left.v1.position.y;
			float s2 = this->right.v2.position.y - this->right.v1.position.y;
			float t1 = (t - this->left.v1.position.y) / s1;
			float t2 = (t - this->right.v1.position.y) / s2;
			this->left.v = vertex::interpolate(this->left.v1, this->left.v2, t1);
			this->right.v = vertex::interpolate(this->right.v1, this->right.v2, t2);
		}

		static int generate_trapezoid(vertex& p1, vertex& p2, vertex& p3, std::vector<trapezoid>& ret) {
			assert(ret.size() == 2);
			float k, x;

			if (p1.position.y > p2.position.y) std::swap(p1, p2);
			if (p1.position.y > p3.position.y) std::swap(p1, p3);
			if (p2.position.y > p3.position.y) std::swap(p2, p3);
			if (p1.position.y == p2.position.y && p1.position.y == p3.position.y) return 0;
			if (p1.position.x == p2.position.x && p1.position.x == p3.position.x) return 0;

			if (p1.position.y == p2.position.y) {	// triangle down
				if (p1.position.x > p2.position.x) std::swap(p1, p2);
				ret[0].top = p1.position.y;
				ret[0].bottom = p3.position.y;
				ret[0].left.v1 = p1;
				ret[0].left.v2 = p3;
				ret[0].right.v1 = p2;
				ret[0].right.v2 = p3;
				return (ret[0].top < ret[0].bottom) ? 1 : 0;
			}

			if (p2.position.y == p3.position.y) {	// triangle up
				if (p2.position.x > p3.position.x) std::swap(p2, p3);
				ret[0].top = p1.position.y;
				ret[0].bottom = p3.position.y;
				ret[0].left.v1 = p1;
				ret[0].left.v2 = p2;
				ret[0].right.v1 = p1;
				ret[0].right.v2 = p3;
				return (ret[0].top < ret[0].bottom) ? 1 : 0;
			}

			ret[0].top = p1.position.y;
			ret[0].bottom = p2.position.y;
			ret[1].top = p2.position.y;
			ret[1].bottom = p3.position.y;

			k = (p3.position.y - p1.position.y) / (p2.position.y - p1.position.y);
			x = p1.position.x + (p2.position.x - p1.position.x) * k;

			if (x <= p3.position.x) {		// triangle left
				ret[0].left.v1 = p1;
				ret[0].left.v2 = p2;
				ret[0].right.v1 = p1;
				ret[0].right.v2 = p3;
				ret[1].left.v1 = p2;
				ret[1].left.v2 = p3;
				ret[1].right.v1 = p1;
				ret[1].right.v2 = p3;
			}
			else {					// triangle right
				ret[0].left.v1 = p1;
				ret[0].left.v2 = p3;
				ret[0].right.v1 = p1;
				ret[0].right.v2 = p2;
				ret[1].left.v1 = p1;
				ret[1].left.v2 = p3;
				ret[1].right.v1 = p2;
				ret[1].right.v2 = p3;
			}

			return 2;
		}
	};
}
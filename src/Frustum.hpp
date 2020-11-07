#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	struct Frustum {
	public:
		Frustum(const Plane& l, const Plane& r, const Plane& t, const Plane& b, const Plane& n, const Plane& f) {
			left = l;
			right = r;
			top = t;
			bottom = b;
			near = n;
			far = f;
		}

	public:
		Plane left;
		Plane right;
		Plane top;
		Plane bottom;
		Plane near;
		Plane far;

		// http://www8.cs.umu.se/kurser/5DV180/VT18/lab/plane_extraction.pdf
		// represent cvv clipping equation by this combination of vp matrix elements
		static Frustum create(const Matrix4x4& v, const Matrix4x4& p) {
			auto vp = p * v;

			float a, b, c, d;

			a = vp.at(3, 0) + vp.at(0, 0);
			b = vp.at(3, 1) + vp.at(0, 1);
			c = vp.at(3, 2) + vp.at(0, 2);
			d = vp.at(3, 3) + vp.at(0, 3);

			Plane left = Plane(a, b, c, d);

			a = vp.at(3, 0) - vp.at(0, 0);
			b = vp.at(3, 1) - vp.at(0, 1);
			c = vp.at(3, 2) - vp.at(0, 2);
			d = vp.at(3, 3) - vp.at(0, 3);

			Plane right = Plane(a, b, c, d);

			a = vp.at(3, 0) + vp.at(1, 0);
			b = vp.at(3, 1) + vp.at(1, 1);
			c = vp.at(3, 2) + vp.at(1, 2);
			d = vp.at(3, 3) + vp.at(1, 3);

			Plane bottom = Plane(a, b, c, d);

			a = vp.at(3, 0) - vp.at(1, 0);
			b = vp.at(3, 1) - vp.at(1, 1);
			c = vp.at(3, 2) - vp.at(1, 2);
			d = vp.at(3, 3) - vp.at(1, 3);

			Plane top = Plane(a, b, c, d);

			a = vp.at(3, 0) + vp.at(2, 0);
			b = vp.at(3, 1) + vp.at(2, 1);
			c = vp.at(3, 2) + vp.at(2, 2);
			d = vp.at(3, 3) + vp.at(2, 3);

			Plane near = Plane(a, b, c, d);

			a = vp.at(3, 0) - vp.at(2, 0);
			b = vp.at(3, 1) - vp.at(2, 1);
			c = vp.at(3, 2) - vp.at(2, 2);
			d = vp.at(3, 3) - vp.at(2, 3);

			Plane far = Plane(a, b, c, d);

			return Frustum(left, right, top, bottom, near, far);
		}

		const Plane& operator[](const int& index) const {
			switch (index)
			{
			case 0:
				return left;
			case 1:
				return right;
			case 2:
				return top;
			case 3:
				return bottom;
			case 4:
				return near;
			case 5:
				return far;
			default:
				std::cerr << "index out of range: " << index << std::endl;
				return near;
			}
		}

		Plane& operator[](const int& index) {
			switch (index)
			{
			case 0:
				return left;
			case 1:
				return right;
			case 2:
				return top;
			case 3:
				return bottom;
			case 4:
				return near;
			case 5:
				return far;
			default:
				std::cerr << "index out of range: " << index << std::endl;
				return near;
			}
		}
	};
}
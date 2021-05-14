#ifndef _FRUSTUM_
#define _FRUSTUM_
#include <iostream>
#include "Math.hpp"
#include "Plane.hpp"

#undef near
#undef far

namespace Guarneri
{
	struct Frustum
	{
	public:
		Plane left;
		Plane right;
		Plane top;
		Plane bottom;
		Plane near;
		Plane far;

	public:
		Frustum(const Plane& l, const Plane& r, const Plane& t, const Plane& b, const Plane& n, const Plane& f);
		~Frustum();
		static Frustum create(const Matrix4x4& mat);
		static Frustum create_clip(const float& w);
		static Frustum create_ndc();
		const Plane& operator[](const int& index) const;
		Plane& operator[](const int& index);
	};


	Frustum::Frustum(const Plane& l, const Plane& r, const Plane& t, const Plane& b, const Plane& n, const Plane& f)
	{
		left = l;
		right = r;
		top = t;
		bottom = b;
		near = n;
		far = f;
	}

	Frustum::~Frustum()
	{}

	// http://www8.cs.umu.se/kurser/5DV180/VT18/lab/plane_extraction.pdf
	// represent cvv clipping equation by this combination of vp matrix elements
	Frustum Frustum::create(const Matrix4x4& mat)
	{
		float a, b, c, d;

		a = mat.at(3, 0) + mat.at(0, 0);
		b = mat.at(3, 1) + mat.at(0, 1);
		c = mat.at(3, 2) + mat.at(0, 2);
		d = mat.at(3, 3) + mat.at(0, 3);

		Plane left = Plane(a, b, c, d);

		a = mat.at(3, 0) - mat.at(0, 0);
		b = mat.at(3, 1) - mat.at(0, 1);
		c = mat.at(3, 2) - mat.at(0, 2);
		d = mat.at(3, 3) - mat.at(0, 3);

		Plane right = Plane(a, b, c, d);

		a = mat.at(3, 0) + mat.at(1, 0);
		b = mat.at(3, 1) + mat.at(1, 1);
		c = mat.at(3, 2) + mat.at(1, 2);
		d = mat.at(3, 3) + mat.at(1, 3);

		Plane bottom = Plane(a, b, c, d);

		a = mat.at(3, 0) - mat.at(1, 0);
		b = mat.at(3, 1) - mat.at(1, 1);
		c = mat.at(3, 2) - mat.at(1, 2);
		d = mat.at(3, 3) - mat.at(1, 3);

		Plane top = Plane(a, b, c, d);

		a = mat.at(3, 0) + mat.at(2, 0);
		b = mat.at(3, 1) + mat.at(2, 1);
		c = mat.at(3, 2) + mat.at(2, 2);
		d = mat.at(3, 3) + mat.at(2, 3);

		Plane near = Plane(a, b, c, d);

		a = mat.at(3, 0) - mat.at(2, 0);
		b = mat.at(3, 1) - mat.at(2, 1);
		c = mat.at(3, 2) - mat.at(2, 2);
		d = mat.at(3, 3) - mat.at(2, 3);

		Plane far = Plane(a, b, c, d);

		return Frustum(left, right, top, bottom, near, far);
	}

	Frustum Frustum::create_clip(const float& w)
	{
		Plane left = Plane(w, 0.0f, 0.0f, 1.0f);
		Plane right = Plane(-w, 0.0f, 0.0f, 1.0f);
		Plane bottom = Plane(0.0f, w, 0.0f, 1.0f);
		Plane top = Plane(0.0f, -w, 0.0f, 1.0f);
		Plane near = Plane(0.0f, 0.0f, w, 1.0f);
		Plane far = Plane(0.0f, 0.0f, -w, 1.0f);
		return Frustum(left, right, top, bottom, near, far);
	}

	Frustum Frustum::create_ndc()
	{
		Plane left = Plane(1.0f, 0.0f, 0.0f, 1.0f);
		Plane right = Plane(-1.0f, 0.0f, 0.0f, 1.0f);
		Plane bottom = Plane(0.0f, 1.0f, 0.0f, 1.0f);
		Plane top = Plane(0.0f, -1.0f, 0.0f, 1.0f);
		Plane near = Plane(0.0f, 0.0f, 1.0f, 1.0f);
		Plane far = Plane(0.0f, 0.0f, -1.0f, 1.0f);
		return Frustum(left, right, top, bottom, near, far);
	}

	const Plane& Frustum::operator[](const int& index) const
	{
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

	Plane& Frustum::operator[](const int& index)
	{
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
}
#endif
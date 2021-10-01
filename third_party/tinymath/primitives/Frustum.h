#pragma once
#include "tinymath/primitives/Plane.h"
#include "tinymath/Matrix4x4.h"

TINYMATH_NAMESPACE

struct Frustum
{
public:
	tinymath::Plane left;
	tinymath::Plane right;
	tinymath::Plane top;
	tinymath::Plane bottom;
	tinymath::Plane near;
	tinymath::Plane far;

public:
	TMATH_INLINE Frustum(const tinymath::Plane& l, const tinymath::Plane& r, const tinymath::Plane& t, const tinymath::Plane& b, const tinymath::Plane& n, const tinymath::Plane& f);
	TMATH_INLINE ~Frustum();
	TMATH_INLINE const tinymath::Plane& operator[](const int& index) const;
	TMATH_INLINE tinymath::Plane& operator[](const int& index);

	TMATH_INLINE static Frustum create(const tinymath::mat4x4& mat);
	TMATH_INLINE static Frustum homogenous_volume();
};

// Implementation
Frustum::Frustum(const tinymath::Plane& l, const tinymath::Plane& r, const tinymath::Plane& t, const tinymath::Plane& b, const tinymath::Plane& n, const tinymath::Plane& f)
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

// http://www8.cs.umu.se/kurser/5DV180/VT18/lab/tinymath::Plane_extraction.pdf
// represent cvv clipping equation by this combination of vp matrix elements
Frustum Frustum::create(const tinymath::mat4x4& mat)
{
	float a, b, c, d;

	a = mat.at(3, 0) + mat.at(0, 0);
	b = mat.at(3, 1) + mat.at(0, 1);
	c = mat.at(3, 2) + mat.at(0, 2);
	d = mat.at(3, 3) + mat.at(0, 3);

	tinymath::Plane left = tinymath::Plane(a, b, c, d);

	a = mat.at(3, 0) - mat.at(0, 0);
	b = mat.at(3, 1) - mat.at(0, 1);
	c = mat.at(3, 2) - mat.at(0, 2);
	d = mat.at(3, 3) - mat.at(0, 3);

	tinymath::Plane right = tinymath::Plane(a, b, c, d);

	a = mat.at(3, 0) + mat.at(1, 0);
	b = mat.at(3, 1) + mat.at(1, 1);
	c = mat.at(3, 2) + mat.at(1, 2);
	d = mat.at(3, 3) + mat.at(1, 3);

	tinymath::Plane bottom = tinymath::Plane(a, b, c, d);

	a = mat.at(3, 0) - mat.at(1, 0);
	b = mat.at(3, 1) - mat.at(1, 1);
	c = mat.at(3, 2) - mat.at(1, 2);
	d = mat.at(3, 3) - mat.at(1, 3);

	tinymath::Plane top = tinymath::Plane(a, b, c, d);

	a = mat.at(3, 0) + mat.at(2, 0);
	b = mat.at(3, 1) + mat.at(2, 1);
	c = mat.at(3, 2) + mat.at(2, 2);
	d = mat.at(3, 3) + mat.at(2, 3);

	tinymath::Plane near = tinymath::Plane(a, b, c, d);

	a = mat.at(3, 0) - mat.at(2, 0);
	b = mat.at(3, 1) - mat.at(2, 1);
	c = mat.at(3, 2) - mat.at(2, 2);
	d = mat.at(3, 3) - mat.at(2, 3);

	tinymath::Plane far = tinymath::Plane(a, b, c, d);

	return Frustum(left, right, top, bottom, near, far);
}

Frustum Frustum::homogenous_volume()
{
	tinymath::Plane left = tinymath::Plane(1.0f, 0.0f, 0.0f, 1.0f);
	tinymath::Plane right = tinymath::Plane(-1.0f, 0.0f, 0.0f, 1.0f);
	tinymath::Plane bottom = tinymath::Plane(0.0f, 1.0f, 0.0f, 1.0f);
	tinymath::Plane top = tinymath::Plane(0.0f, -1.0f, 0.0f, 1.0f);
	tinymath::Plane near = tinymath::Plane(0.0f, 0.0f, 1.0f, 1.0f);
	tinymath::Plane far = tinymath::Plane(0.0f, 0.0f, -1.0f, 1.0f);
	return Frustum(left, right, top, bottom, near, far);
}

const tinymath::Plane& Frustum::operator[](const int& index) const
{
	assert(index >= 0 && index <= 5);
	switch (index)
	{
	case 0:
		return near;
	case 1:
		return far;
	case 2:
		return top;
	case 3:
		return bottom;
	case 4:
		return left;
	case 5:
		return right;
	}
	return near;
}

tinymath::Plane& Frustum::operator[](const int& index)
{
	assert(index >= 0 && index <= 5);
	switch (index)
	{
	case 0:
		return near;
	case 1:
		return far;
	case 2:
		return top;
	case 3:
		return bottom;
	case 4:
		return left;
	case 5:
		return right;
	}
	return near;
}

END_NAMESPACE
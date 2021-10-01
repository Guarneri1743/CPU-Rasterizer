#pragma once
#include <string>
#include "tinymath/Vector3.h"
#include "tinymath/primitives/Line.h"

TINYMATH_NAMESPACE

struct Cylinder
{
public:
	Line axis;
	float radius;
	float height;

public:
	TMATH_INLINE Cylinder();
	TMATH_INLINE Cylinder(const Line& axis_t, const float& _radius, const float& _height);
	TMATH_INLINE Cylinder(const Cylinder& other);
};

// Implementation
Cylinder::Cylinder()
{
	axis = Line();
	radius = 0;
	height = 0;
}

Cylinder::Cylinder(const Line& axis_t, const float& _radius, const float& _height)
{
	this->axis = axis_t;
	this->radius = _radius;
	this->height = _height;
}

Cylinder::Cylinder(const Cylinder& other)
{
	this->axis = other.axis;
	this->radius = other.radius;
	this->height = other.height;
}

END_NAMESPACE
#pragma once
#include <string>
#include "tinymath/MathDefine.h"
#include "tinymath/Vector3.h"

TINYMATH_NAMESPACE

struct Sphere
{
public:
	tinymath::vec3f center;
	float radius;

public:
	TMATH_INLINE Sphere();
	TMATH_INLINE Sphere(const tinymath::vec3f& _center, float _radius);
	TMATH_INLINE Sphere(const Sphere& other);
};

// Implementation
Sphere::Sphere()
{
	center = tinymath::vec3f();
	radius = 0;
}

Sphere::Sphere(const tinymath::vec3f& _center, float _radius)
{
	this->center = _center;
	this->radius = _radius;
}

Sphere::Sphere(const Sphere& other)
{
	this->center = other.center;
	this->radius = other.radius;
}

END_NAMESPACE
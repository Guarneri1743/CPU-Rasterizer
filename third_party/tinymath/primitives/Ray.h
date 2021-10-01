#pragma once
#include <string>
#include "tinymath/MathDefine.h"
#include "tinymath/Vector3.h"

TINYMATH_NAMESPACE

struct Ray
{
public:
	tinymath::vec3f origin;
	tinymath::vec3f direction;
	tinymath::vec3f inversed_direction;

public:
	TMATH_INLINE Ray();
	TMATH_INLINE Ray(const tinymath::vec3f& origin, const tinymath::vec3f& direction);
	TMATH_INLINE Ray(const Ray& other);
};

// Implementation
Ray::Ray()
{
	origin = tinymath::vec3f();
	direction = tinymath::vec3f();
	inversed_direction = tinymath::vec3f();
}

Ray::Ray(const tinymath::vec3f& origin, const tinymath::vec3f& direction)
{
	this->origin = origin;
	this->direction = direction;
	this->inversed_direction = tinymath::vec3f(1 / direction.x, 1 / direction.y, 1 / direction.z);
}

Ray::Ray(const Ray& other)
{
	this->origin = other.origin;
	this->direction = other.direction;
	this->inversed_direction = other.inversed_direction;
}

END_NAMESPACE
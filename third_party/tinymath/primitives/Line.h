#pragma once
#include <string>
#include "tinymath/MathDefine.h"
#include "tinymath/Vector3.h"

TINYMATH_NAMESPACE

struct Line
{
public:
	tinymath::vec3f origin;
	tinymath::vec3f direction;

public:
	TMATH_INLINE Line();
	TMATH_INLINE Line(const tinymath::vec3f& origin_t, const tinymath::vec3f& direction_t);
	TMATH_INLINE Line(const Line& other);
};

// Implementation
Line::Line()
{
	origin = tinymath::vec3f();
	direction = tinymath::vec3f();
}

Line::Line(const tinymath::vec3f& origin_t, const tinymath::vec3f& direction_t)
{
	this->origin = origin_t;
	this->direction = direction_t;
}

Line::Line(const Line& other)
{
	this->origin = other.origin;
	this->direction = other.direction;
}

END_NAMESPACE
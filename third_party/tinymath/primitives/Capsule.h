#pragma once
#include <string>
#include "tinymath/MathDefine.h"
#include "tinymath/Vector3.h"
#include "tinymath/primitives/Segment.h"

TINYMATH_NAMESPACE

struct Capsule
{
public:
	Segment segment;
	float radius;

public:
	TMATH_INLINE Capsule();
	TMATH_INLINE Capsule(const tinymath::vec3f& center, const tinymath::vec3f& dir, const float& extent, const float& radius_t);
	TMATH_INLINE Capsule(const tinymath::vec3f& start, const tinymath::vec3f& end, const float& radius);
	TMATH_INLINE Capsule(const Segment& Segment, const float& radius);
	TMATH_INLINE Capsule(const Capsule& other);
};

// Implementation
Capsule::Capsule()
{
	segment = Segment::Segment();
	radius = 0;
}

Capsule::Capsule(const tinymath::vec3f& center, const tinymath::vec3f& dir, const float& extent, const float& radius_t)
{
	this->segment = Segment::Segment(center, dir, extent);
	this->radius = radius_t;
}

Capsule::Capsule(const tinymath::vec3f& start, const tinymath::vec3f& end, const float& radius)
{
	this->segment = Segment::Segment(start, end);
	this->radius = radius;
}

Capsule::Capsule(const Segment& Segment, const float& radius)
{
	this->segment = Segment;
	this->radius = radius;
}

Capsule::Capsule(const Capsule& other)
{
	this->segment = other.segment;
	this->radius = other.radius;
}

END_NAMESPACE
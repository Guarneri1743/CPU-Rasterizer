#ifndef _CAPSULE_
#define _CAPSULE_
#include <string>
#include "tinymath.h"
#include "Segment.hpp"

namespace Guarneri
{
	struct Capsule
	{
	public:
		Segment segment;
		float radius;

	public:
		Capsule();
		Capsule(const tinymath::vec3f& center, const tinymath::vec3f& dir, const float& extent, const float& radius_t);
		Capsule(const tinymath::vec3f& start, const tinymath::vec3f& end, const float& radius);
		Capsule(const Segment& Segment, const float& radius);
		Capsule(const Capsule& other);
	};
}
#endif
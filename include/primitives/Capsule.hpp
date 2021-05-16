#ifndef _CAPSULE_
#define _CAPSULE_
#include <string>
#include "Vector3.hpp"
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
		Capsule(const Guarneri::Vector3& center, const Guarneri::Vector3& dir, const float& extent, const float& radius_t);
		Capsule(const Guarneri::Vector3& start, const Guarneri::Vector3& end, const float& radius);
		Capsule(const Segment& Segment, const float& radius);
		Capsule(const Capsule& other);
		std::string str() const;
	};
}
#endif
#include "Capsule.hpp"
#include "Marcos.h"
#include <iomanip>

namespace Guarneri
{
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
}
#include "Capsule.hpp"
#include <iomanip>
#include <sstream>

namespace Guarneri
{
	Capsule::Capsule()
	{
		segment = Segment::Segment();
		radius = 0;
	}

	Capsule::Capsule(const Guarneri::Vector3& center, const Guarneri::Vector3& dir, const float& extent, const float& radius_t)
	{
		this->segment = Segment::Segment(center, dir, extent);
		this->radius = radius_t;
	}

	Capsule::Capsule(const Guarneri::Vector3& start, const Guarneri::Vector3& end, const float& radius)
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

	std::string Capsule::str() const
	{
		std::stringstream ss;
		ss << "Capsule: [center: " << this->segment.str() << ", size: " << std::setprecision(FLOAT_LOG_PRECISION) << this->radius << "]";
		return ss.str();
	}
}
#ifndef _CAPSULE_
#define _CAPSULE_
#include <Guarneri.hpp>

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
		ss << "Capsule: [center: " << this->segment << ", size: " << std::setprecision(FLOAT_LOG_PRECISION) << this->radius << "]";
		return ss.str();
	}
	static std::ostream& operator << (std::ostream& stream, const Capsule& Capsule)
	{
		stream << Capsule.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Capsule& Capsule)
	{
		stream << Capsule.str();
		return stream;
	}
}
#endif
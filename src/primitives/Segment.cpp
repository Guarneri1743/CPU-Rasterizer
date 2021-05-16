#include "Segment.hpp"
#include <sstream>

namespace Guarneri
{
	Segment::Segment()
	{
		start = Vector3::ZERO;
		end = Vector3::ZERO;
	}

	Segment::Segment(const Vector3& center, const Vector3& dir, const float& extent)
	{
		this->start = center - extent * dir;
		this->end = center + extent * dir;
	}

	Segment::Segment(const Vector3& start, const Vector3& end)
	{
		this->start = start;
		this->end = end;
	}

	Segment::Segment(const Segment& other)
	{
		this->start = other.start;
		this->end = other.end;
	}

	Vector3 Segment::center() const
	{
		return (this->start + this->end) / 2;
	}

	float Segment::extent() const
	{
		return Vector3::length(end, start) / 2;
	}

	Vector3 Segment::dir() const
	{
		return Vector3::normalize(end - start);
	}

	std::string Segment::str() const
	{
		std::stringstream ss;
		ss << "Segment: [start: " << this->start.str() << ", end: " << this->end.str() << "]";
		return ss.str();
	}
}

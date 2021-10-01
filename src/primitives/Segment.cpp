#include "Segment.hpp"

namespace Guarneri
{
	Segment::Segment()
	{
		start = tinymath::kVec3fZero;
		end = tinymath::kVec3fZero;
	}

	Segment::Segment(const tinymath::vec3f& center, const tinymath::vec3f& dir, const float& extent)
	{
		this->start = center - extent * dir;
		this->end = center + extent * dir;
	}

	Segment::Segment(const tinymath::vec3f& start, const tinymath::vec3f& end)
	{
		this->start = start;
		this->end = end;
	}

	Segment::Segment(const Segment& other)
	{
		this->start = other.start;
		this->end = other.end;
	}

	tinymath::vec3f Segment::center() const
	{
		return (this->start + this->end) / 2.0f;
	}

	float Segment::extent() const
	{
		return tinymath::length(end, start) / 2;
	}

	tinymath::vec3f Segment::dir() const
	{
		return tinymath::normalize(end - start);
	}
}

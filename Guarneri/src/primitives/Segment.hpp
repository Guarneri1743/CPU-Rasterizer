#ifndef _SEGMENT_
#define _SEGMENT_
#include <Guarneri.hpp>

namespace Guarneri
{
	struct Segment
	{
	public:
		Vector3 start;
		Vector3 end;

	public:
		Segment();
		Segment(const Vector3& center, const Vector3& dir, const float& extent);
		Segment(const Vector3& start, const Vector3& end);
		Segment(const Segment& other);
		Vector3 center() const;
		float extent() const;
		Vector3 dir() const;
		std::string str() const;
	};


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
		ss << "Segment: [start: " << this->start << ", end: " << this->end << "]";
		return ss.str();
	}

	static std::ostream& operator << (std::ostream& stream, const Segment& seg)
	{
		stream << seg.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Segment& seg)
	{
		stream << seg.str();
		return stream;
	}
}
#endif
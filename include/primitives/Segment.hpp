#ifndef _SEGMENT_
#define _SEGMENT_
#include <string>
#include <ostream>
#include <sstream>
#include "Vector3.hpp"

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
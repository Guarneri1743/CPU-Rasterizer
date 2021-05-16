#ifndef _SEGMENT_
#define _SEGMENT_
#include <string>
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
}
#endif
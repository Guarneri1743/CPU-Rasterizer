#ifndef _SEGMENT_
#define _SEGMENT_
#include <string>
#include "TinyMath.h"

namespace Guarneri
{
	struct Segment
	{
	public:
		tinymath::vec3f start;
		tinymath::vec3f end;

	public:
		Segment();
		Segment(const tinymath::vec3f& center, const tinymath::vec3f& dir, const float& extent);
		Segment(const tinymath::vec3f& start, const tinymath::vec3f& end);
		Segment(const Segment& other);
		tinymath::vec3f center() const;
		float extent() const;
		tinymath::vec3f dir() const;
	};
}
#endif
#ifndef _PRIM_LINE_
#define _PRIM_LINE_
#include <string>
#include "tinymath.h"

namespace Guarneri
{
	struct Line
	{
	public:
		tinymath::vec3f origin;
		tinymath::vec3f direction;

	public:
		Line();
		Line(const tinymath::vec3f& origin_t, const tinymath::vec3f& direction_t);
		Line(const Line& other);
	};
}
#endif
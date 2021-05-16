#ifndef _PRIM_LINE_
#define _PRIM_LINE_
#include <string>
#include "Vector3.hpp"

namespace Guarneri
{
	struct Line
	{
	public:
		Vector3 origin;
		Vector3 direction;

	public:
		Line();
		Line(const Vector3& origin_t, const Vector3& direction_t);
		Line(const Line& other);
		std::string str() const;
	};
}
#endif
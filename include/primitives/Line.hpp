#ifndef _LINE_
#define _LINE_
#include <string>
#include <ostream>
#include <sstream>
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

	static std::ostream& operator << (std::ostream& stream, const Line& Line)
	{
		stream << Line.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Line& Line)
	{
		stream << Line.str();
		return stream;
	}
}
#endif
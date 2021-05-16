#ifndef _CYLINDER_
#define _CYLINDER_
#include <string>
#include <sstream>
#include <ostream>
#include "Vector3.hpp"
#include "Line.hpp"

namespace Guarneri
{
	struct Cylinder
	{
	public:
		Line axis;
		float radius;
		float height;

	public:
		Cylinder();
		Cylinder(const Line& axis_t, const float& _radius, const float& _height);
		Cylinder(const Cylinder& other);
		std::string str() const;
	};

	static std::ostream& operator << (std::ostream& stream, const Cylinder& Cylinder)
	{
		stream << Cylinder.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Cylinder& Cylinder)
	{
		stream << Cylinder.str();
		return stream;
	}
}
#endif
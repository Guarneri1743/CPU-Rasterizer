#ifndef _CYLINDER_
#define _CYLINDER_
#include <string>
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
}
#endif
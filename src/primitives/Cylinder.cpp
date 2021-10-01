#include "Cylinder.hpp"
#include "Marcos.h"
#include <iomanip>

namespace Guarneri
{
	Cylinder::Cylinder()
	{
		axis = Line();
		radius = 0;
		height = 0;
	}

	Cylinder::Cylinder(const Line& axis_t, const float& _radius, const float& _height)
	{
		this->axis = axis_t;
		this->radius = _radius;
		this->height = _height;
	}

	Cylinder::Cylinder(const Cylinder& other)
	{
		this->axis = other.axis;
		this->radius = other.radius;
		this->height = other.height;
	}
}
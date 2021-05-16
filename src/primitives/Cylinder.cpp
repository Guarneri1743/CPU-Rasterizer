#include "Cylinder.hpp"
#include "Marcos.h"
#include <iomanip>
#include <sstream>

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

	std::string Cylinder::str() const
	{
		std::stringstream ss;
		ss << "Cylinder: [axis: " << this->axis.str() << ", radius: " << std::setprecision(FLOAT_LOG_PRECISION) << this->radius << ", height" << std::setprecision(FLOAT_LOG_PRECISION) << this->height << "]";
		return ss.str();
	}
}
#include "Sphere.hpp"
#include "Marcos.h"
#include <iomanip>

namespace Guarneri
{
	Sphere::Sphere()
	{
		center = tinymath::vec3f();
		radius = 0;
	}

	Sphere::Sphere(const tinymath::vec3f& _center, const float& _radius)
	{
		this->center = _center;
		this->radius = _radius;
	}

	Sphere::Sphere(const Sphere& other)
	{
		this->center = other.center;
		this->radius = other.radius;
	}
}
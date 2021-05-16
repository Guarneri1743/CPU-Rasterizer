#include "Sphere.hpp"
#include <iomanip>
#include <sstream>

namespace Guarneri
{
	Sphere::Sphere()
	{
		center = Vector3();
		radius = 0;
	}

	Sphere::Sphere(const Vector3& _center, const float& _radius)
	{
		this->center = _center;
		this->radius = _radius;
	}

	Sphere::Sphere(const Sphere& other)
	{
		this->center = other.center;
		this->radius = other.radius;
	}

	std::string Sphere::str() const
	{
		std::stringstream ss;
		ss << "Sphere: [origin: " << this->center.str() << ", radius: " << std::setprecision(FLOAT_LOG_PRECISION) << this->radius << "]";
		return ss.str();
	}
}
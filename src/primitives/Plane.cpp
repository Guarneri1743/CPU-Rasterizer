#include "Plane.hpp"
#include "Marcos.h"
#include <iomanip>

namespace Guarneri
{
	Plane::Plane()
	{
		normal = tinymath::vec3f();
		constant = 0;
	}

	Plane::Plane(const tinymath::vec3f& normal, const float& constant)
	{
		this->normal = normal;
		this->constant = constant;
	}

	Plane::Plane(const float& a, const float& b, const float& c, const float& d)
	{
		this->normal = tinymath::vec3f(a, b, c);
		this->constant = d;
	}

	Plane::Plane(const tinymath::vec3f& normal, const tinymath::vec3f& p)
	{
		this->normal = normal;
		this->constant = tinymath::dot(normal, p);
	}

	Plane::Plane(const Plane& other)
	{
		this->normal = other.normal;
		this->constant = other.constant;
	}

	float Plane::distance(const tinymath::vec3f& pt) const
	{
		return tinymath::dot(normal, pt) + constant;
	}

	float Plane::homo_distance(const tinymath::vec4f& pt) const
	{
		return normal.x * pt.x + normal.y * pt.y + normal.z * pt.z + constant * pt.w;
	}
}
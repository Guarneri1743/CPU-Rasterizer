#include "Plane.hpp"
#include <sstream>
#include <iomanip>

namespace Guarneri
{
	Plane::Plane()
	{
		normal = Vector3();
		constant = 0;
	}

	Plane::Plane(const Vector3& normal, const float& constant)
	{
		this->normal = normal;
		this->constant = constant;
	}

	Plane::Plane(const float& a, const float& b, const float& c, const float& d)
	{
		this->normal = Vector3(a, b, c);
		this->constant = d;
	}

	Plane::Plane(const Vector3& normal, const Vector3& p)
	{
		this->normal = normal;
		this->constant = Vector3::dot(normal, p);
	}

	Plane::Plane(const Plane& other)
	{
		this->normal = other.normal;
		this->constant = other.constant;
	}

	float Plane::distance(const Vector3& pt) const
	{
		return Vector3::dot(normal, pt) + constant;
	}

	float Plane::homo_distance(const Vector4& pt) const
	{
		return normal.x * pt.x + normal.y * pt.y + normal.z * pt.z + constant * pt.w;
	}

	std::string Plane::str() const
	{
		std::stringstream ss;
		ss << "Plane: [normal: " << this->normal.str() << std::setprecision(FLOAT_LOG_PRECISION) << ", constant: " << this->constant << "]";
		return ss.str();
	}
}
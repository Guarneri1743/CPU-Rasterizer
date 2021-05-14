#ifndef _PLANE_
#define _PLANE_
#include "Vector3.hpp"

namespace Guarneri
{
	struct Plane
	{
	public:
		Guarneri::Vector3 normal;
		float constant;

	public:
		Plane();
		Plane(const Guarneri::Vector3& normal, const float& constant);
		Plane(const float& a, const float& b, const float& c, const float& d);
		Plane(const Guarneri::Vector3& normal, const Guarneri::Vector3& p);
		Plane(const Plane& other);
		float distance(const Vector3& pt) const;
		std::string str() const;
	};


	Plane::Plane()
	{
		normal = Guarneri::Vector3();
		constant = 0;
	}

	Plane::Plane(const Guarneri::Vector3& normal, const float& constant)
	{
		this->normal = normal;
		this->constant = constant;
	}

	Plane::Plane(const float& a, const float& b, const float& c, const float& d)
	{
		this->normal = Vector3(a, b, c);
		this->constant = d;
	}

	Plane::Plane(const Guarneri::Vector3& normal, const Guarneri::Vector3& p)
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

	std::string Plane::str() const
	{
		std::stringstream ss;
		ss << "Plane: [normal: " << this->normal << ", constant: " << this->constant << "]";
		return ss.str();
	}

	static std::ostream& operator << (std::ostream& stream, const Plane& Plane)
	{
		stream << Plane.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Plane& Plane)
	{
		stream << Plane.str();
		return stream;
	}
}
#endif
#ifndef _SPHERE_
#define _SPHERE_
#include <Guarneri.hpp>

namespace Guarneri
{
	struct Sphere
	{
	public:
		Guarneri::Vector3 center;
		float radius;

	public:
		Sphere();
		Sphere(const Guarneri::Vector3& _center, const float& _radius);
		Sphere(const Sphere& other);
		std::string str() const;
	};


	Sphere::Sphere()
	{
		center = Guarneri::Vector3();
		radius = 0;
	}

	Sphere::Sphere(const Guarneri::Vector3& _center, const float& _radius)
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
		ss << "Sphere: [origin: " << this->center << ", radius: " << std::setprecision(FLOAT_LOG_PRECISION) << this->radius << "]";
		return ss.str();
	}

	static std::ostream& operator << (std::ostream& stream, const Sphere& Sphere)
	{
		stream << Sphere.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Sphere& Sphere)
	{
		stream << Sphere.str();
		return stream;
	}
}
#endif
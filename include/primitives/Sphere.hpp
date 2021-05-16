#ifndef _SPHERE_
#define _SPHERE_
#include <string>
#include <sstream>
#include <ostream>
#include "Vector3.hpp"

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
#ifndef _SPHERE_
#define _SPHERE_
#include <string>
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
}
#endif
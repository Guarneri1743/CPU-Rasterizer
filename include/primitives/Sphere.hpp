#ifndef _SPHERE_
#define _SPHERE_
#include <string>
#include "TinyMath.h"

namespace Guarneri
{
	struct Sphere
	{
	public:
		tinymath::vec3f center;
		float radius;

	public:
		Sphere();
		Sphere(const tinymath::vec3f& _center, const float& _radius);
		Sphere(const Sphere& other);
	};
}
#endif
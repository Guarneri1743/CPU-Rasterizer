#include "Ray.hpp"

namespace Guarneri
{
	Ray::Ray()
	{
		origin = tinymath::vec3f();
		direction = tinymath::vec3f();
		inversed_direction = tinymath::vec3f();
	}

	Ray::Ray(const tinymath::vec3f& origin, const tinymath::vec3f& direction)
	{
		this->origin = origin;
		this->direction = direction;
		this->inversed_direction = tinymath::vec3f(1 / direction.x, 1 / direction.y, 1 / direction.z);
	}

	Ray::Ray(const Ray& other)
	{
		this->origin = other.origin;
		this->direction = other.direction;
		this->inversed_direction = other.inversed_direction;
	}
}
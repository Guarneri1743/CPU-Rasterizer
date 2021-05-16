#include "Ray.hpp"

namespace Guarneri
{
	Ray::Ray()
	{
		origin = Vector3();
		direction = Vector3();
		inversed_direction = Vector3();
	}

	Ray::Ray(const Vector3& origin, const Vector3& direction)
	{
		this->origin = origin;
		this->direction = direction;
		this->inversed_direction = Vector3(1 / direction.x, 1 / direction.y, 1 / direction.z);
	}

	Ray::Ray(const Ray& other)
	{
		this->origin = other.origin;
		this->direction = other.direction;
		this->inversed_direction = other.inversed_direction;
	}

	std::string Ray::str() const
	{
		std::stringstream ss;
		ss << "Ray: [origin: " << this->origin << ", direction: " << this->direction << ", inversed_direction: " << inversed_direction << "]";
		return ss.str();
	}
}
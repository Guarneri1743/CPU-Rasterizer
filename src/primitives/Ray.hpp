#ifndef _RAY_
#define _RAY_
#include "Vector3.hpp"

namespace Guarneri
{
	struct Ray
	{
	public:
		Guarneri::Vector3 origin;
		Guarneri::Vector3 direction;
		Guarneri::Vector3 inversed_direction;

	public:
		Ray();
		Ray(const Guarneri::Vector3& origin, const Guarneri::Vector3& direction);
		Ray(const Ray& other);
		std::string str() const;
	};


	Ray::Ray()
	{
		origin = Guarneri::Vector3();
		direction = Guarneri::Vector3();
		inversed_direction = Guarneri::Vector3();
	}

	Ray::Ray(const Guarneri::Vector3& origin, const Guarneri::Vector3& direction)
	{
		this->origin = origin;
		this->direction = direction;
		this->inversed_direction = Guarneri::Vector3(1 / direction.x, 1 / direction.y, 1 / direction.z);
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

	static std::ostream& operator << (std::ostream& stream, const Ray& Ray)
	{
		stream << Ray.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Ray& Ray)
	{
		stream << Ray.str();
		return stream;
	}
}
#endif
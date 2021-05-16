#ifndef _RAY_
#define _RAY_
#include <string>
#include <ostream>
#include <sstream>
#include "Vector3.hpp"

namespace Guarneri
{
	struct Ray
	{
	public:
		Vector3 origin;
		Vector3 direction;
		Vector3 inversed_direction;

	public:
		Ray();
		Ray(const Vector3& origin, const Vector3& direction);
		Ray(const Ray& other);
		std::string str() const;
	};
	
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
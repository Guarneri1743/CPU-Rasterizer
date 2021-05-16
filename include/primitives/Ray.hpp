#ifndef _RAY_
#define _RAY_
#include <string>
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
}
#endif
#ifndef _PLANE_
#define _PLANE_
#include <string>
#include <ostream>
#include <sstream>
#include "Vector3.hpp"

namespace Guarneri
{
	struct Plane
	{
	public:
		Vector3 normal;
		float constant;

	public:
		Plane();
		Plane(const Vector3& normal, const float& constant);
		Plane(const float& a, const float& b, const float& c, const float& d);
		Plane(const Vector3& normal, const Vector3& p);
		Plane(const Plane& other);
		float distance(const Vector3& pt) const;
		std::string str() const;
	};

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
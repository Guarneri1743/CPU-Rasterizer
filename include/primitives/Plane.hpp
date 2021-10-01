#ifndef _PLANE_
#define _PLANE_
#include <string>
#include "TinyMath.h"

namespace Guarneri
{
	struct Plane
	{
	public:
		tinymath::vec3f normal;
		float constant;

	public:
		Plane();
		Plane(const tinymath::vec3f& normal, const float& constant);
		Plane(const float& a, const float& b, const float& c, const float& d);
		Plane(const tinymath::vec3f& normal, const tinymath::vec3f& p);
		Plane(const Plane& other);
		float distance(const tinymath::vec3f& pt) const;
		float homo_distance(const tinymath::vec4f& pt) const;
	};
}
#endif
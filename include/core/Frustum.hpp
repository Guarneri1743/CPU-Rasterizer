#ifndef _FRUSTUM_
#define _FRUSTUM_
#include "tinymath.h"
#include "Plane.hpp"

namespace Guarneri
{
	struct Frustum
	{
	public:
		Plane left;
		Plane right;
		Plane top;
		Plane bottom;
		Plane near;
		Plane far;

	public:
		Frustum(const Plane& l, const Plane& r, const Plane& t, const Plane& b, const Plane& n, const Plane& f);
		~Frustum();
		static Frustum create(const tinymath::mat4x4& mat);
		static Frustum homogenous_volume();
		const Plane& operator[](const int& index) const;
		Plane& operator[](const int& index);
	};
}
#endif
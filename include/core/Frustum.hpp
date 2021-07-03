#ifndef _FRUSTUM_
#define _FRUSTUM_
#include "Matrix4x4.hpp"
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
		static Frustum create(const Matrix4x4& mat);
		static Frustum create_homogeneous_cube();
		const Plane& operator[](const int& index) const;
		Plane& operator[](const int& index);
	};
}
#endif
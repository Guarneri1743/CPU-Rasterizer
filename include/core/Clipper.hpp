#ifndef _CLIPPER_
#define _CLIPPER_
#include <vector>
#include <assert.h>
#include "Vertex.hpp"
#include "Vector4.hpp"
#include "Triangle.hpp"
#include "Plane.hpp"
#include "Frustum.hpp"
#include "Sphere.hpp"

namespace Guarneri
{
	class Clipper
	{
	public:
		static std::vector<Triangle> cvv_clipping(const float& near_plane, const Frustum& cvv, const Vertex& c1, const Vertex& c2, const Vertex& c3);
		static void screen_clipping(Vertex& lhs, Vertex& rhs, const int& width);
		static bool cvv_culling(const Vector4& c1, const Vector4& c2, const Vector4& c3);
		static bool cvv_culling(const Vector4& v);
		static bool backface_culling_ndc(const Vector3& c1, const Vector3& c2, const Vector3& c3);
		static bool frustum_culling_sphere(const Frustum& frustum, const Sphere& bounding_sphere);
		static bool conservative_frustum_culling(const Frustum& frustum, const Vertex& v1, const Vertex& v2, const Vertex& v3);
	};
}
#endif
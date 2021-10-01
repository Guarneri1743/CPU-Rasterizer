#ifndef _CLIPPER_
#define _CLIPPER_
#include <vector>
#include <assert.h>
#include "Vertex.hpp"
#include "TinyMath.h"
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
		static bool inside_cvv(const tinymath::vec4f& c1, const tinymath::vec4f& c2, const tinymath::vec4f& c3);
		static bool inside_cvv(const tinymath::vec4f& v);
		static bool backface_culling_ndc(const tinymath::vec3f& c1, const tinymath::vec3f& c2, const tinymath::vec3f& c3);
		static bool frustum_culling_sphere(const Frustum& frustum, const Sphere& bounding_sphere);
		static bool conservative_frustum_culling(const Frustum& frustum, const Vertex& v1, const Vertex& v2, const Vertex& v3);
	};
}
#endif
#pragma once
#include <vector>
#include <assert.h>
#include "RasterAttributes.hpp"
#include "tinymath.h"
#include "Triangle.hpp"

namespace CpuRasterizer
{
	class Clipper
	{
	public:
		static bool clip_segment(float near_plane, const tinymath::vec4f& c1, const tinymath::vec4f& c2, tinymath::vec4f& out_c1, tinymath::vec4f& out_c2);
		static std::vector<Triangle> clip_triangle(float near_plane, const tinymath::Frustum& cvv, const Vertex& c1, const Vertex& c2, const Vertex& c3);
		static void clip_horizontally(Vertex& lhs, Vertex& rhs, size_t width);
		static bool inside_cvv(const tinymath::vec4f& c1, const tinymath::vec4f& c2, const tinymath::vec4f& c3);
		static bool inside_cvv(const tinymath::vec4f& v);
		static bool backface_culling_ndc(const tinymath::vec3f& normal);
		static bool backface_culling_ndc(const tinymath::vec3f& c1, const tinymath::vec3f& c2, const tinymath::vec3f& c3);
		static bool frustum_culling_sphere(const tinymath::Frustum& frustum, const tinymath::Sphere& bounding_sphere);
		static bool conservative_frustum_culling(const tinymath::Frustum& frustum, const Vertex& v1, const Vertex& v2, const Vertex& v3);
	};
}
#pragma once
#include <vector>
#include <assert.h>
#include "RasterAttributes.hpp"
#include "tinymath.h"
#include "Triangle.hpp"

namespace Clipping
{
	std::vector<Triangle> cvv_clipping(const float& near_plane, const tinymath::Frustum& cvv, const Vertex& c1, const Vertex& c2, const Vertex& c3);
	void screen_clipping(Vertex& lhs, Vertex& rhs, const size_t& width);
	bool inside_cvv(const tinymath::vec4f& c1, const tinymath::vec4f& c2, const tinymath::vec4f& c3);
	bool inside_cvv(const tinymath::vec4f& v);
	bool backface_culling_ndc(const tinymath::vec3f& c1, const tinymath::vec3f& c2, const tinymath::vec3f& c3);
	bool frustum_culling_sphere(const tinymath::Frustum& frustum, const tinymath::Sphere& bounding_sphere);
	bool conservative_frustum_culling(const tinymath::Frustum& frustum, const Vertex& v1, const Vertex& v2, const Vertex& v3);
}
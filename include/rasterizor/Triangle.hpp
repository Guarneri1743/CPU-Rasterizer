#pragma once
#include <vector>
#include <string>
#include "tinymath/MathDefine.h"
#include "tinymath/Vector2.h"
#include "tinymath/Vector3.h"
#include "tinymath/primitives/Rect.h"
#include "Vertex.hpp"

namespace Guarneri
{
	struct Triangle
	{
		Vertex vertices[3];
		bool flip;
		bool culled;
		float cached_area;

	public:
		Triangle();
		Triangle(const Vertex verts[3]);
		Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);
		Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, const bool& flip);
		void interpolate(const float& screen_y, Vertex& lhs, Vertex& rhs) const;
		std::vector<Triangle> horizontally_split() const;
		tinymath::Rect get_bounds() const;
		float area() const;
		float area_double() const;
		bool barycentric_interpolate(const tinymath::vec2f& pos, Vertex& interpolated_vert) const;
		void update_area();
		static float area_double(const tinymath::vec2f& v1, const tinymath::vec2f& v2, const tinymath::vec2f& v3);
		static float area_double(const tinymath::vec3f& v1, const tinymath::vec3f& v2, const tinymath::vec3f& v3);
		static float area(const tinymath::vec3f& v1, const tinymath::vec3f& v2, const tinymath::vec3f& v3);
		Vertex& operator[](const uint32_t i);
		const Vertex& operator[](const uint32_t i) const;
	};
}
#ifndef _TRIANGLE_
#define _TRIANGLE_
#include <vector>
#include <string>
#include <ostream>
#include <sstream>
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vertex.hpp"

namespace Guarneri
{
	struct Triangle
	{
	public:
		Vertex vertices[3];
		bool flip;
		bool culled;

	public:
		Triangle();
		Triangle(const Vertex verts[3]);
		Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);
		Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, const bool& flip);
		void interpolate(const float& screen_y, Vertex& lhs, Vertex& rhs) const;
		std::vector<Triangle> horizontally_split() const;
		float area() const;
		float area_double() const;
		static float area_double(const Vector2& v1, const Vector2& v2, const Vector2& v3);
		static float area_double(const Vector3& v1, const Vector3& v2, const Vector3& v3);
		static float area(const Vector3& v1, const Vector3& v2, const Vector3& v3);
		Vertex& operator[](const uint32_t i);
		const Vertex& operator[](const uint32_t i) const;
		std::string str() const;
	};

	static std::ostream& operator << (std::ostream& stream, const Triangle& tri)
	{
		stream << tri.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Triangle& tri)
	{
		stream << tri.str();
		return stream;
	}
}
#endif
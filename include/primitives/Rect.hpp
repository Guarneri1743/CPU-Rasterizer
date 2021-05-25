#ifndef _Rect_
#define _Rect_
#include <string>
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace Guarneri
{
	struct Rect
	{
	public:
		Vector2 center;
		Vector2 extents;

	public:
		Rect();
		Rect(float x, float y, float w, float h);
		Rect(const Vector2& center, const Vector2& size);
		Rect(const Rect& b);
		Rect(const Vector2& v1, const Vector2& v2, const Vector2& v3) { from_triangle(v1, v2, v3); }
		void from_triangle(const Vector2& v1, const Vector2& v2, const Vector2& v3);
		Vector2 size() const;
		Vector2 min() const;
		Vector2 max() const;
		float x() const { return center.x - extents.x; }
		float y() const { return center.y - extents.y; }
		float w() const { return extents.x * 2; }
		float h() const { return extents.y * 2; }
		void set_min_max(const Vector2& min, const Vector2& max);
		void set_min(const Vector2& m);
		void set_max(const Vector2& m);
		Vector2 corner(const int& n) const;
		bool contains(const Vector2& pos) const;
		float approx(const float& a, const float& b) const;
		void expand(const Vector2& p);
		void expand(const Rect& p);
		Vector2 offset(const Vector2& p) const;
		Vector2 inv_offset(const Vector2& p) const;
		std::string str() const;
	};
}
#endif
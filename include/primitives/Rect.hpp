#ifndef _Rect_
#define _Rect_
#include <string>
#include "TinyMath.h"

namespace Guarneri
{
	struct Rect
	{
	public:
		tinymath::vec2f center;
		tinymath::vec2f extents;

	public:
		Rect();
		Rect(float x, float y, float w, float h);
		Rect(const tinymath::vec2f& center, const tinymath::vec2f& size);
		Rect(const Rect& b);
		Rect(const tinymath::vec2f& v1, const tinymath::vec2f& v2, const tinymath::vec2f& v3) { from_triangle(v1, v2, v3); }
		void from_triangle(const tinymath::vec2f& v1, const tinymath::vec2f& v2, const tinymath::vec2f& v3);
		tinymath::vec2f size() const;
		tinymath::vec2f min() const;
		tinymath::vec2f max() const;
		float x() const { return center.x - extents.x; }
		float y() const { return center.y - extents.y; }
		float w() const { return extents.x * 2; }
		float h() const { return extents.y * 2; }
		void set_min_max(const tinymath::vec2f& min, const tinymath::vec2f& max);
		void set_min(const tinymath::vec2f& m);
		void set_max(const tinymath::vec2f& m);
		tinymath::vec2f corner(const int& n) const;
		bool contains(const tinymath::vec2f& pos) const;
		float approx(const float& a, const float& b) const;
		void expand(const tinymath::vec2f& p);
		void expand(const Rect& p);
		tinymath::vec2f offset(const tinymath::vec2f& p) const;
		tinymath::vec2f inv_offset(const tinymath::vec2f& p) const;
	};
}
#endif
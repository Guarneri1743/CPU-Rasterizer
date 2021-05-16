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
		Guarneri::Vector2 center;
		Guarneri::Vector2 extents;

	public:
		Rect();
		Rect(const Guarneri::Vector2& center, const Guarneri::Vector2& size);
		Rect(const Guarneri::Vector2& p);
		Rect(const Rect& b);
		Rect(const Vector4& v1, const Vector4& v2, const Vector4& v3);
		Rect(const Vector3& v1, const Vector3& v2, const Vector3& v3);
		Rect(const Vector2& v1, const Vector2& v2, const Vector2& v3);
		void from_triangle(const Vector2& v1, const Vector2& v2, const Vector2& v3);
		Guarneri::Vector2 size() const;
		Guarneri::Vector2 min() const;
		Guarneri::Vector2 max() const;
		void set_min_max(const Guarneri::Vector2& min, const Guarneri::Vector2& max);
		void set_min(const Guarneri::Vector2& m);
		void set_max(const Guarneri::Vector2& m);
		Guarneri::Vector2 corner(const int& n) const;
		bool contains(const Guarneri::Vector2& pos) const;
		float approx(const float& a, const float& b) const;
		void expand(const Guarneri::Vector2& p);
		void expand(const Rect& p);
		Guarneri::Vector2 offset(const Guarneri::Vector2& p) const;
		Guarneri::Vector2 inv_offset(const Guarneri::Vector2& p) const;
		std::string str() const;
	};
}
#endif
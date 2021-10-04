#pragma once
#include <string>
#include "tinymath/MathDefine.h"
#include "tinymath/Vector2Int.h"

TINYMATH_NAMESPACE

struct Rect
{
public:
	tinymath::vec2i center;
	tinymath::vec2i extents;

public:
	TMATH_INLINE Rect();
	TMATH_INLINE Rect(int x, int y, int w, int h);
	TMATH_INLINE Rect(const tinymath::vec2i& center, const tinymath::vec2i& size);
	TMATH_INLINE Rect(const Rect& b);
	TMATH_INLINE Rect(const tinymath::vec2i& v1, const tinymath::vec2i& v2, const tinymath::vec2i& v3) { from_triangle(v1, v2, v3); }
	TMATH_INLINE void from_triangle(const tinymath::vec2i& v1, const tinymath::vec2i& v2, const tinymath::vec2i& v3);
	TMATH_INLINE tinymath::vec2i size() const;
	TMATH_INLINE tinymath::vec2i min() const;
	TMATH_INLINE tinymath::vec2i max() const;
	TMATH_INLINE int x() const { return center.x - extents.x; }
	TMATH_INLINE int y() const { return center.y - extents.y; }
	TMATH_INLINE int w() const { return extents.x * 2; }
	TMATH_INLINE int h() const { return extents.y * 2; }
	TMATH_INLINE void set_min_max(const tinymath::vec2i& min, const tinymath::vec2i& max);
	TMATH_INLINE void set_min(const tinymath::vec2i& m);
	TMATH_INLINE void set_max(const tinymath::vec2i& m);
	TMATH_INLINE tinymath::vec2i corner(const int& n) const;
	TMATH_INLINE bool contains(const tinymath::vec2i& pos) const;
	TMATH_INLINE int approx(const int& a, const int& b) const;
	TMATH_INLINE void expand(const tinymath::vec2i& p);
	TMATH_INLINE void expand(const Rect& p);
	TMATH_INLINE tinymath::vec2i offset(const tinymath::vec2i& p) const;
	TMATH_INLINE tinymath::vec2i inv_offset(const tinymath::vec2i& p) const;
};

// Implementation
Rect::Rect()
{
	center = tinymath::vec2i();
	extents = tinymath::vec2i();
}

Rect::Rect(int x, int y, int w, int h)
{
	this->extents = tinymath::vec2i(w, h) / 2;
	this->center = tinymath::vec2i(x, y) + extents;
}

Rect::Rect(const tinymath::vec2i& center, const tinymath::vec2i& size)
{
	this->center = center;
	this->extents = size / 2;
}

Rect::Rect(const Rect& b)
{
	this->center = b.center;
	this->extents = b.extents;
}

void Rect::from_triangle(const tinymath::vec2i& v1, const tinymath::vec2i& v2, const tinymath::vec2i& v3)
{
	int min_x = INT_MAX;
	int max_x = INT_MIN;
	int min_y = INT_MAX;
	int max_y = INT_MIN;

	if (v1.x < min_x)
	{
		min_x = v1.x;
	}
	if (v2.x < min_x)
	{
		min_x = v2.x;
	}
	if (v3.x < min_x)
	{
		min_x = v3.x;
	}

	if (v1.y < min_y)
	{
		min_y = v1.y;
	}
	if (v2.y < min_y)
	{
		min_y = v2.y;
	}
	if (v3.y < min_y)
	{
		min_y = v3.y;
	}

	if (v1.x > max_x)
	{
		max_x = v1.x;
	}
	if (v2.x > max_x)
	{
		max_x = v2.x;
	}
	if (v3.x > max_x)
	{
		max_x = v3.x;
	}

	if (v1.y > max_y)
	{
		max_y = v1.y;
	}
	if (v2.y > max_y)
	{
		max_y = v2.y;
	}
	if (v3.y > max_y)
	{
		max_y = v3.y;
	}

	this->set_min_max(tinymath::vec2i(min_x, min_y), tinymath::vec2i(max_x, max_y));
}

tinymath::vec2i Rect::size() const
{
	return extents * 2;
}

tinymath::vec2i Rect::min() const
{
	return center - extents;
}

tinymath::vec2i Rect::max() const
{
	return center + extents;
}

void Rect::set_min_max(const tinymath::vec2i& min, const tinymath::vec2i& max)
{
	this->extents = (max - min) / 2;
	this->center = min + this->extents;
}

void Rect::set_min(const tinymath::vec2i& m)
{
	set_min_max(m, this->max());
}

void Rect::set_max(const tinymath::vec2i& m)
{
	set_min_max(this->min(), m);
}

tinymath::vec2i Rect::corner(const int& n) const
{
	tinymath::vec2i p;
	p.x = ((n & 1) ? max().x : min().x);
	p.y = ((n & 1) ? max().y : min().y);
	return p;
}

bool Rect::contains(const tinymath::vec2i& pos) const
{
	if (pos.x < min().x)
	{
		return false;
	}
	if (pos.x > max().x)
	{
		return false;
	}
	if (pos.y < min().y)
	{
		return false;
	}
	if (pos.y > max().y)
	{
		return false;
	}
	return true;
}

int Rect::approx(const int& a, const int& b) const
{
	return tinymath::abs(a - b) < 1e-5f;
}

void Rect::expand(const tinymath::vec2i& p)
{
	auto mi = min();
	auto ma = max();
	mi.x = tinymath::min(mi.x, p.x);
	mi.y = tinymath::min(mi.y, p.y);

	ma.x = tinymath::max(ma.x, p.x);
	ma.y = tinymath::max(ma.y, p.y);

	set_min_max(mi, ma);

	center.x = (mi.x + ma.x) / 2;
	center.y = (mi.y + ma.y) / 2;
}

void Rect::expand(const Rect& p)
{
	auto pmi = p.min();
	auto pma = p.max();
	auto mi = min();
	auto ma = max();
	mi.x = tinymath::min(mi.x, pmi.x);
	mi.y = tinymath::min(mi.y, pmi.y);

	ma.x = tinymath::max(ma.x, pma.x);
	ma.y = tinymath::max(ma.y, pma.y);

	set_min_max(mi, ma);

	center.x = (mi.x + ma.x) / 2;
	center.y = (mi.y + ma.y) / 2;
}

tinymath::vec2i Rect::offset(const tinymath::vec2i& p) const
{
	auto mi = min();
	auto ma = max();
	tinymath::vec2i o = p - mi;
	if (ma.x > mi.x) o.x /= ma.x - mi.x;
	if (ma.y > mi.y) o.y /= ma.y - mi.y;
	return o;
}

tinymath::vec2i Rect::inv_offset(const tinymath::vec2i& p) const
{
	tinymath::vec2i o;
	o.x = p.x * (max().x - min().x) + min().x;
	o.y = p.y * (max().y - min().y) + min().y;
	return o;
}

END_NAMESPACE
#pragma once
#include <string>
#include "tinymath/MathDefine.h"
#include "tinymath/Vector2.h"

TINYMATH_NAMESPACE

struct Rect
{
public:
	tinymath::vec2f center;
	tinymath::vec2f extents;

public:
	TMATH_INLINE Rect();
	TMATH_INLINE Rect(float x, float y, float w, float h);
	TMATH_INLINE Rect(const tinymath::vec2f& center, const tinymath::vec2f& size);
	TMATH_INLINE Rect(const Rect& b);
	TMATH_INLINE Rect(const tinymath::vec2f& v1, const tinymath::vec2f& v2, const tinymath::vec2f& v3) { from_triangle(v1, v2, v3); }
	TMATH_INLINE void from_triangle(const tinymath::vec2f& v1, const tinymath::vec2f& v2, const tinymath::vec2f& v3);
	TMATH_INLINE tinymath::vec2f size() const;
	TMATH_INLINE tinymath::vec2f min() const;
	TMATH_INLINE tinymath::vec2f max() const;
	TMATH_INLINE float x() const { return center.x - extents.x; }
	TMATH_INLINE float y() const { return center.y - extents.y; }
	TMATH_INLINE float w() const { return extents.x * 2; }
	TMATH_INLINE float h() const { return extents.y * 2; }
	TMATH_INLINE void set_min_max(const tinymath::vec2f& min, const tinymath::vec2f& max);
	TMATH_INLINE void set_min(const tinymath::vec2f& m);
	TMATH_INLINE void set_max(const tinymath::vec2f& m);
	TMATH_INLINE tinymath::vec2f corner(const int& n) const;
	TMATH_INLINE bool contains(const tinymath::vec2f& pos) const;
	TMATH_INLINE float approx(const float& a, const float& b) const;
	TMATH_INLINE void expand(const tinymath::vec2f& p);
	TMATH_INLINE void expand(const Rect& p);
	TMATH_INLINE tinymath::vec2f offset(const tinymath::vec2f& p) const;
	TMATH_INLINE tinymath::vec2f inv_offset(const tinymath::vec2f& p) const;
};

// Implementation
Rect::Rect()
{
	center = tinymath::vec2f();
	extents = tinymath::vec2f();
}

Rect::Rect(float x, float y, float w, float h)
{
	this->extents = tinymath::vec2f(w, h) / 2.f;
	this->center = tinymath::vec2f(x, y) + extents;
}

Rect::Rect(const tinymath::vec2f& center, const tinymath::vec2f& size)
{
	this->center = center;
	this->extents = size / 2.f;
}

Rect::Rect(const Rect& b)
{
	this->center = b.center;
	this->extents = b.extents;
}

void Rect::from_triangle(const tinymath::vec2f& v1, const tinymath::vec2f& v2, const tinymath::vec2f& v3)
{
	float min_x = FLT_MAX;
	float max_x = FLT_MIN;
	float min_y = FLT_MAX;
	float max_y = FLT_MIN;

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

	this->set_min_max(tinymath::vec2f(min_x, min_y), tinymath::vec2f(max_x, max_y));
}

tinymath::vec2f Rect::size() const
{
	return extents * 2.f;
}

tinymath::vec2f Rect::min() const
{
	return center - extents;
}

tinymath::vec2f Rect::max() const
{
	return center + extents;
}

void Rect::set_min_max(const tinymath::vec2f& min, const tinymath::vec2f& max)
{
	this->extents = (max - min) * 0.5f;
	this->center = min + this->extents;
}

void Rect::set_min(const tinymath::vec2f& m)
{
	set_min_max(m, this->max());
}

void Rect::set_max(const tinymath::vec2f& m)
{
	set_min_max(this->min(), m);
}

tinymath::vec2f Rect::corner(const int& n) const
{
	tinymath::vec2f p;
	p.x = ((n & 1) ? max().x : min().x);
	p.y = ((n & 1) ? max().y : min().y);
	return p;
}

bool Rect::contains(const tinymath::vec2f& pos) const
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

float Rect::approx(const float& a, const float& b) const
{
	return tinymath::abs(a - b) < 1e-5f;
}

void Rect::expand(const tinymath::vec2f& p)
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

tinymath::vec2f Rect::offset(const tinymath::vec2f& p) const
{
	auto mi = min();
	auto ma = max();
	tinymath::vec2f o = p - mi;
	if (ma.x > mi.x) o.x /= ma.x - mi.x;
	if (ma.y > mi.y) o.y /= ma.y - mi.y;
	return o;
}

tinymath::vec2f Rect::inv_offset(const tinymath::vec2f& p) const
{
	tinymath::vec2f o;
	o.x = p.x * (max().x - min().x) + min().x;
	o.y = p.y * (max().y - min().y) + min().y;
	return o;
}

END_NAMESPACE
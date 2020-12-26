#ifndef _BOUNDINGBOX2D_
#define _BOUNDINGBOX2D_
#include <CPURasterizer.hpp>

namespace Guarneri
{
	struct BoundingBox2D
	{
	public:
		Guarneri::Vector2 center;
		Guarneri::Vector2 extents;

	public:
		BoundingBox2D();
		BoundingBox2D(const Guarneri::Vector2& center, const Guarneri::Vector2& size);
		BoundingBox2D(const Guarneri::Vector2& p);
		BoundingBox2D(const BoundingBox2D& b);
		BoundingBox2D(const Vector4& v1, const Vector4& v2, const Vector4& v3);
		BoundingBox2D(const Vector3& v1, const Vector3& v2, const Vector3& v3);
		BoundingBox2D(const Vector2& v1, const Vector2& v2, const Vector2& v3);
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
		void expand(const BoundingBox2D& p);
		Guarneri::Vector2 offset(const Guarneri::Vector2& p) const;
		Guarneri::Vector2 inv_offset(const Guarneri::Vector2& p) const;
		bool intersect(Ray2D ray, float& dnear, float& dfar);
		std::string str() const;
	};


	BoundingBox2D::BoundingBox2D()
	{
		center = Guarneri::Vector2();
		extents = Guarneri::Vector2();
	}

	BoundingBox2D::BoundingBox2D(const Guarneri::Vector2& center, const Guarneri::Vector2& size)
	{
		this->center = center;
		this->extents = size / 2;
	}

	BoundingBox2D::BoundingBox2D(const Guarneri::Vector2& p)
	{
		this->center = p;
		this->extents = p;
	}

	BoundingBox2D::BoundingBox2D(const BoundingBox2D& b)
	{
		this->center = b.center;
		this->extents = b.extents;
	}

	BoundingBox2D::BoundingBox2D(const Vector4& v1, const Vector4& v2, const Vector4& v3)
	{
		from_triangle(Vector2(v1.x, v1.y), Vector2(v2.x, v2.y), Vector2(v3.x, v3.y));
	}

	BoundingBox2D::BoundingBox2D(const Vector3& v1, const Vector3& v2, const Vector3& v3)
	{
		from_triangle(Vector2(v1.x, v1.y), Vector2(v2.x, v2.y), Vector2(v3.x, v3.y));
	}

	BoundingBox2D::BoundingBox2D(const Vector2& v1, const Vector2& v2, const Vector2& v3)
	{
		from_triangle(v1, v2, v3);
	}

	void BoundingBox2D::from_triangle(const Vector2& v1, const Vector2& v2, const Vector2& v3)
	{
		float min_x = FLT_MAX;
		float max_x = FLT_MIN;
		float min_y = FLT_MAX;
		float max_y = FLT_MIN;

		if (LESS_THAN(v1.x, min_x))
		{
			min_x = v1.x;
		}
		if (LESS_THAN(v2.x, min_x))
		{
			min_x = v2.x;
		}
		if (LESS_THAN(v3.x, min_x))
		{
			min_x = v3.x;
		}

		if (LESS_THAN(v1.y, min_y))
		{
			min_y = v1.y;
		}
		if (LESS_THAN(v2.y, min_y))
		{
			min_y = v2.y;
		}
		if (LESS_THAN(v3.y, min_y))
		{
			min_y = v3.y;
		}

		if (GREATER_THAN(v1.x, max_x))
		{
			max_x = v1.x;
		}
		if (GREATER_THAN(v2.x, max_x))
		{
			max_x = v2.x;
		}
		if (GREATER_THAN(v3.x, max_x))
		{
			max_x = v3.x;
		}

		if (GREATER_THAN(v1.y, max_y))
		{
			max_y = v1.y;
		}
		if (GREATER_THAN(v2.y, max_y))
		{
			max_y = v2.y;
		}
		if (GREATER_THAN(v3.y, max_y))
		{
			max_y = v3.y;
		}

		this->set_min_max(Vector2(min_x, min_y), Vector2(max_x, max_y));
	}

	Guarneri::Vector2 BoundingBox2D::size() const
	{
		return extents * 2;
	}

	Guarneri::Vector2 BoundingBox2D::min() const
	{
		return center - extents;
	}

	Guarneri::Vector2 BoundingBox2D::max() const
	{
		return center + extents;
	}

	void BoundingBox2D::set_min_max(const Guarneri::Vector2& min, const Guarneri::Vector2& max)
	{
		this->extents = (max - min) * 0.5f;
		this->center = min + this->extents;
	}

	void BoundingBox2D::set_min(const Guarneri::Vector2& m)
	{
		set_min_max(m, this->max());
	}

	void BoundingBox2D::set_max(const Guarneri::Vector2& m)
	{
		set_min_max(this->min(), m);
	}

	Guarneri::Vector2 BoundingBox2D::corner(const int& n) const
	{
		Guarneri::Vector2 p;
		p.x = ((n & 1) ? max().x : min().x);
		p.y = ((n & 1) ? max().y : min().y);
		return p;
	}

	bool BoundingBox2D::contains(const Guarneri::Vector2& pos) const
	{
		if (LESS_THAN(pos.x, min().x))
		{
			return false;
		}
		if (GREATER_THAN(pos.x, max().x))
		{
			return false;
		}
		if (LESS_THAN(pos.y, min().y))
		{
			return false;
		}
		if (GREATER_THAN(pos.y, max().y))
		{
			return false;
		}
		return true;
	}

	float BoundingBox2D::approx(const float& a, const float& b) const
	{
		return std::abs(a - b) < 1e-5f;
	}

	void BoundingBox2D::expand(const Guarneri::Vector2& p)
	{
		auto mi = min();
		auto ma = max();
		mi.x = std::min(mi.x, p.x);
		mi.y = std::min(mi.y, p.y);

		ma.x = std::max(ma.x, p.x);
		ma.y = std::max(ma.y, p.y);

		set_min_max(mi, ma);

		center.x = (mi.x + ma.x) / 2;
		center.y = (mi.y + ma.y) / 2;
	}

	void BoundingBox2D::expand(const BoundingBox2D& p)
	{
		auto pmi = p.min();
		auto pma = p.max();
		auto mi = min();
		auto ma = max();
		mi.x = std::min(mi.x, pmi.x);
		mi.y = std::min(mi.y, pmi.y);

		ma.x = std::max(ma.x, pma.x);
		ma.y = std::max(ma.y, pma.y);

		set_min_max(mi, ma);

		center.x = (mi.x + ma.x) / 2;
		center.y = (mi.y + ma.y) / 2;
	}

	Guarneri::Vector2 BoundingBox2D::offset(const Guarneri::Vector2& p) const
	{
		auto mi = min();
		auto ma = max();
		Guarneri::Vector2 o = p - mi;
		if (ma.x > mi.x) o.x /= ma.x - mi.x;
		if (ma.y > mi.y) o.y /= ma.y - mi.y;
		return o;
	}

	Guarneri::Vector2 BoundingBox2D::inv_offset(const Guarneri::Vector2& p) const
	{
		Guarneri::Vector2 o;
		o.x = p.x * (max().x - min().x) + min().x;
		o.y = p.y * (max().y - min().y) + min().y;
		return o;
	}

	bool BoundingBox2D::intersect(Ray2D ray, float& dnear, float& dfar)
	{
		float tmin;
		float tmax;
		float div = ray.inversed_direction.x;
		if (div >= 0.0f)
		{
			tmin = (min().x - ray.origin.x) * div;
			tmax = (max().x - ray.origin.x) * div;
		}
		else
		{
			tmin = (max().x - ray.origin.x) * div;
			tmax = (min().x - ray.origin.x) * div;
		}
		dnear = tmin;
		dfar = tmax;
		if (dnear > dfar || dfar < 0.0f)
		{
			return false;
		}
		div = ray.inversed_direction.y;
		if (div >= 0.0f)
		{
			tmin = (min().y - ray.origin.y) * div;
			tmax = (max().y - ray.origin.y) * div;
		}
		else
		{
			tmin = (max().y - ray.origin.y) * div;
			tmax = (min().y - ray.origin.y) * div;
		}
		if (tmin > dnear)
		{
			dnear = tmin;
		}
		if (tmax < dfar)
		{
			dfar = tmax;
		}
		if (dnear > dfar || dfar < 0.0f)
		{
			return false;
		}
		return true;
	}

	std::string BoundingBox2D::str() const
	{
		std::stringstream ss;
		ss << "BoundingBox2D: [center: " << this->center << ", size: " << this->size() << "]";
		return ss.str();
	}

	static std::ostream& operator << (std::ostream& stream, const BoundingBox2D& BoundingBox2D)
	{
		stream << BoundingBox2D.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const BoundingBox2D& BoundingBox2D)
	{
		stream << BoundingBox2D.str();
		return stream;
	}
}
#endif
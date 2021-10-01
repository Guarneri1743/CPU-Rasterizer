#include "BoundingBox.hpp"
#include <sstream>

namespace Guarneri
{
	BoundingBox::BoundingBox()
	{
		center = tinymath::vec3f();
		extents = tinymath::vec3f();
	}

	BoundingBox::BoundingBox(const tinymath::vec3f& _center, const tinymath::vec3f& size)
	{
		this->center = _center;
		this->extents = size / 2.0f;
	}

	BoundingBox::BoundingBox(const tinymath::vec3f& p)
	{
		this->center = p;
		this->extents = p;
	}

	BoundingBox::BoundingBox(const BoundingBox& b)
	{
		this->center = b.center;
		this->extents = b.extents;
	}

	tinymath::vec3f BoundingBox::size() const
	{
		return extents * 2.0f;
	}

	tinymath::vec3f BoundingBox::min() const
	{
		return center - extents;
	}

	tinymath::vec3f BoundingBox::max() const
	{
		return center + extents;
	}

	void BoundingBox::set_min_max(const tinymath::vec3f& min, const tinymath::vec3f& max)
	{
		this->extents = (max - min) * 0.5f;
		this->center = min + this->extents;
	}

	void BoundingBox::set_min(const tinymath::vec3f& m)
	{
		set_min_max(m, this->max());
	}

	void BoundingBox::set_max(const tinymath::vec3f& m)
	{
		set_min_max(this->min(), m);
	}

	tinymath::vec3f BoundingBox::corner(const int& n) const
	{
		tinymath::vec3f p;
		p.x = ((n & 1) ? max().x : min().x);
		p.y = ((n & 1) ? max().y : min().y);
		p.z = ((n & 1) ? max().z : min().z);
		return p;
	}

	bool BoundingBox::contains(const tinymath::vec3f& pos) const
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
		if (pos.z < min().z)
		{
			return false;
		}
		if (pos.z > max().z)
		{
			return false;
		}
		return true;
	}

	int BoundingBox::maximum_extent() const
	{
		tinymath::vec3f d = size();
		if (d.x > d.y && d.x > d.z)
			return 0;
		else if (d.y > d.z)
			return 1;
		else
			return 2;
	}

	float BoundingBox::surface() const
	{
		tinymath::vec3f d = size();
		return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
	}

	float BoundingBox::approx(const float& a, const float& b) const
	{
		return std::abs(a - b) < 1e-5f;
	}

	tinymath::vec3f BoundingBox::get_normal(const tinymath::vec3f& hit_position) const
	{
		tinymath::vec3f v = hit_position - min();
		if (approx(v.x, 0.0f) || approx(v.y, 0.0f) || approx(v.z, 0.0f))
		{
			if (approx(v.x, 0.0f))
			{
				return tinymath::kVec3fLeft;
			}
			else if (approx(v.y, 0.0f))
			{
				return tinymath::kVec3fDown;
			}
			else if (approx(v.z, 0.0f))
			{
				return tinymath::kVec3fBack;
			}
		}
		else
		{
			v = max() - hit_position;
			if (approx(v.x, 0.0f))
			{
				return tinymath::kVec3fRight;
			}
			else if (approx(v.y, 0.0f))
			{
				return tinymath::kVec3fUp;
			}
			else if (approx(v.z, 0.0f))
			{
				return tinymath::kVec3fForward;
			}
		}
		return tinymath::kVec3fZero;
	}

	void BoundingBox::expand(const tinymath::vec3f& p)
	{
		auto mi = min();
		auto ma = max();
		mi.x = std::min(mi.x, p.x);
		mi.y = std::min(mi.y, p.y);
		mi.z = std::min(mi.z, p.z);

		ma.x = std::max(ma.x, p.x);
		ma.y = std::max(ma.y, p.y);
		ma.z = std::max(ma.z, p.z);

		set_min_max(mi, ma);

		center.x = (mi.x + ma.x) / 2;
		center.y = (mi.y + ma.y) / 2;
		center.z = (mi.z + ma.z) / 2;
	}

	void BoundingBox::expand(const BoundingBox& p)
	{
		auto pmi = p.min();
		auto pma = p.max();
		auto mi = min();
		auto ma = max();
		mi.x = std::min(mi.x, pmi.x);
		mi.y = std::min(mi.y, pmi.y);
		mi.z = std::min(mi.z, pmi.z);

		ma.x = std::max(ma.x, pma.x);
		ma.y = std::max(ma.y, pma.y);
		ma.z = std::max(ma.z, pma.z);

		set_min_max(mi, ma);

		center.x = (mi.x + ma.x) / 2;
		center.y = (mi.y + ma.y) / 2;
		center.z = (mi.z + ma.z) / 2;
	}

	tinymath::vec3f BoundingBox::offset(const tinymath::vec3f& p) const
	{
		auto mi = min();
		auto ma = max();
		tinymath::vec3f o = p - mi;
		if (ma.x > mi.x) o.x /= ma.x - mi.x;
		if (ma.y > mi.y) o.y /= ma.y - mi.y;
		if (ma.z > mi.z) o.z /= ma.z - mi.z;
		return o;
	}

	tinymath::vec3f BoundingBox::inv_offset(const tinymath::vec3f& p) const
	{
		tinymath::vec3f o;
		o.x = p.x * (max().x - min().x) + min().x;
		o.y = p.y * (max().y - min().y) + min().y;
		o.z = p.z * (max().z - min().z) + min().z;
		return o;
	}
}
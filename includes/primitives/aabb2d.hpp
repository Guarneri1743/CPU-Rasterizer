#pragma once
#include <common.hpp>
#include <float2.hpp>

namespace guarneri {
	struct aabb2d {
	public:
		aabb2d() {
			center = guarneri::float2();
			extents = guarneri::float2();
		}

		aabb2d(const guarneri::float2& center, const guarneri::float2& size) {
			this->center = center;
			this->extents = size / 2;
		}

		aabb2d(const guarneri::float2& p) {
			this->center = p;
			this->extents = p;
		}

		aabb2d(const aabb2d& b)
		{
			this->center = b.center;
			this->extents = b.extents;
		}

	public:
		guarneri::float2 center;
		guarneri::float2 extents;

	public:
		guarneri::float2 size() const {
			return extents * 2;
		}

		guarneri::float2 min() const {
			return center - extents;
		}

		guarneri::float2 max() const {
			return center + extents;
		}

		void set_min_max(const guarneri::float2& min, const guarneri::float2& max) {
			this->extents = (max - min) * 0.5f;
			this->center = min + this->extents;
		}

		void set_min(const guarneri::float2& m) {
			set_min_max(m, this->max());
		}

		void set_max(const guarneri::float2& m) {
			set_min_max(this->min(), m);
		}

		guarneri::float2 corner(const int& n) const
		{
			guarneri::float2 p;
			p.x = ((n & 1) ? max().x : min().x);
			p.y = ((n & 1) ? max().y : min().y);
			return p;
		}

		bool contains(const guarneri::float2& pos) const {
			if (pos.x < min().x) {
				return false;
			}
			if (pos.x > max().x) {
				return false;
			}
			if (pos.y < min().y) {
				return false;
			}
			if (pos.y > max().y) {
				return false;
			}
			return true;
		}

		float approx(const float& a, const float& b) const {
			return std::abs(a - b) < 1e-5f;
		}

		void expand(const guarneri::float2& p)
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

		void expand(const aabb2d& p)
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

		guarneri::float2 offset(const guarneri::float2& p) const
		{
			auto mi = min();
			auto ma = max();
			guarneri::float2 o = p - mi;
			if (ma.x > mi.x) o.x /= ma.x - mi.x;
			if (ma.y > mi.y) o.y /= ma.y - mi.y;
			return o;
		}

		guarneri::float2 inv_offset(const guarneri::float2& p) const
		{
			guarneri::float2 o;
			o.x = p.x * (max().x - min().x) + min().x;
			o.y = p.y * (max().y - min().y) + min().y;
			return o;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "aabb2d: [center: " << this->center << ", size: " << this->size() << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const aabb2d& aabb2d) {
		stream << aabb2d.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const aabb2d& aabb2d) {
		stream << aabb2d.str();
		return stream;
	}
}
#pragma once
#include <common.hpp>
#include <float3.hpp>

namespace guarneri {
	struct aabb {
	public:
		aabb() {
			center = guarneri::float3();
			extents = guarneri::float3();
		}

		aabb(const guarneri::float3& center_t, const guarneri::float3& size) {
			this->center = center_t;
			this->extents = size / 2;
		}

		aabb(const guarneri::float3& p) {
			this->center = p;
			this->extents = p;
		}

		aabb(const aabb& b)
		{
			this->center = b.center;
			this->extents = b.extents;
		}

	public:
		guarneri::float3 center;
		guarneri::float3 extents;

	public:
		guarneri::float3 size() const{
			return extents * 2;
		}

		guarneri::float3 min() const {
			return center - extents;
		}

		guarneri::float3 max() const {
			return center + extents;
		}

		void set_min_max(const guarneri::float3& min, const guarneri::float3& max) {
			this->extents = (max - min) * 0.5f;
			this->center = min + this->extents;
		}

		void set_min(const guarneri::float3& m) {
			set_min_max(m, this->max());
		}

		void set_max(const guarneri::float3& m) {
			set_min_max(this->min(), m);
		}

		guarneri::float3 corner(const int& n) const
		{
			guarneri::float3 p;
			p.x = ((n & 1) ? max().x : min().x);
			p.y = ((n & 1) ? max().y : min().y);
			p.z = ((n & 1) ? max().z : min().z);
			return p;
		}

		bool contains(const guarneri::float3& pos) const {
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
			if (pos.z < min().z) {
				return false;
			}
			if (pos.z > max().z) {
				return false;
			}
			return true;
		}

		int maximum_extent() const
		{
			guarneri::float3 d = size();
			if (d.x > d.y && d.x > d.z)
				return 0;
			else if (d.y > d.z)
				return 1;
			else
				return 2;
		}

		float surface() const
		{
			guarneri::float3 d = size();
			return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
		}

	    float approx(const float& a, const float& b) const{
			return std::abs(a - b) < 1e-5f;
		}

		guarneri::float3 get_normal(const guarneri::float3& hit_position) const
		{
			guarneri::float3 v = hit_position - min();
			if (approx(v.x, 0.0f) || approx(v.y, 0.0f) || approx(v.z, 0.0f))
			{
				if (approx(v.x, 0.0f))
				{
					return guarneri::float3::LEFT;
				}
				else if (approx(v.y, 0.0f))
				{
					return guarneri::float3::DOWN;
				}
				else if (approx(v.z, 0.0f))
				{
					return guarneri::float3::BACK;
				}
			}
			else
			{
				v = max() - hit_position;
				if (approx(v.x, 0.0f))
				{
					return guarneri::float3::RIGHT;
				}
				else if (approx(v.y, 0.0f))
				{
					return guarneri::float3::UP;
				}
				else if (approx(v.z, 0.0f))
				{
					return guarneri::float3::FORWARD;
				}
			}
			return guarneri::float3::ZERO;
		}

		void expand(const guarneri::float3& p)
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

		void expand(const aabb& p)
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

		guarneri::float3 offset(const guarneri::float3& p) const
		{
			auto mi = min();
			auto ma = max();
			guarneri::float3 o = p - mi;
			if (ma.x > mi.x) o.x /= ma.x - mi.x;
			if (ma.y > mi.y) o.y /= ma.y - mi.y;
			if (ma.z > mi.z) o.z /= ma.z - mi.z;
			return o;
		}

		guarneri::float3 inv_offset(const guarneri::float3& p) const
		{
			guarneri::float3 o;
			o.x = p.x * (max().x - min().x) + min().x;
			o.y = p.y * (max().y - min().y) + min().y;
			o.z = p.z * (max().z - min().z) + min().z;
			return o;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "aabb: [center: " << this->center << ", size: " << this->size() << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const aabb& aabb) {
		stream << aabb.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const aabb& aabb) {
		stream << aabb.str();
		return stream;
	}
}
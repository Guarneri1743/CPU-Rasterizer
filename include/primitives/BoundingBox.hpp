#ifndef _BOUNDINGBOX_
#define _BOUNDINGBOX_
#include <sstream>
#include <ostream>
#include <string>
#include "Vector3.hpp"

namespace Guarneri
{
	struct BoundingBox
	{
	public:
		Guarneri::Vector3 center;
		Guarneri::Vector3 extents;

	public:
		BoundingBox();
		BoundingBox(const Guarneri::Vector3& _center, const Guarneri::Vector3& size);
		BoundingBox(const Guarneri::Vector3& p);
		BoundingBox(const BoundingBox& b);
		Guarneri::Vector3 size() const;
		Guarneri::Vector3 min() const;
		Guarneri::Vector3 max() const;
		void set_min_max(const Guarneri::Vector3& min, const Guarneri::Vector3& max);
		void set_min(const Guarneri::Vector3& m);
		void set_max(const Guarneri::Vector3& m);
		Guarneri::Vector3 corner(const int& n) const;
		bool contains(const Guarneri::Vector3& pos) const;
		int maximum_extent() const;
		float surface() const;
		float approx(const float& a, const float& b) const;
		Guarneri::Vector3 get_normal(const Guarneri::Vector3& hit_position) const;
		void expand(const Guarneri::Vector3& p);
		void expand(const BoundingBox& p);
		Guarneri::Vector3 offset(const Guarneri::Vector3& p) const;
		Guarneri::Vector3 inv_offset(const Guarneri::Vector3& p) const;
		std::string str() const;
	};

	static std::ostream& operator << (std::ostream& stream, const BoundingBox& BoundingBox)
	{
		stream << BoundingBox.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const BoundingBox& BoundingBox)
	{
		stream << BoundingBox.str();
		return stream;
	}
}
#endif
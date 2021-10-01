#ifndef _BOUNDINGBOX_
#define _BOUNDINGBOX_
#include <string>
#include "TinyMath.h"

namespace Guarneri
{
	struct BoundingBox
	{
	public:
		tinymath::vec3f center;
		tinymath::vec3f extents;

	public:
		BoundingBox();
		BoundingBox(const tinymath::vec3f& _center, const tinymath::vec3f& size);
		BoundingBox(const tinymath::vec3f& p);
		BoundingBox(const BoundingBox& b);
		tinymath::vec3f size() const;
		tinymath::vec3f min() const;
		tinymath::vec3f max() const;
		void set_min_max(const tinymath::vec3f& min, const tinymath::vec3f& max);
		void set_min(const tinymath::vec3f& m);
		void set_max(const tinymath::vec3f& m);
		tinymath::vec3f corner(const int& n) const;
		bool contains(const tinymath::vec3f& pos) const;
		int maximum_extent() const;
		float surface() const;
		float approx(const float& a, const float& b) const;
		tinymath::vec3f get_normal(const tinymath::vec3f& hit_position) const;
		void expand(const tinymath::vec3f& p);
		void expand(const BoundingBox& p);
		tinymath::vec3f offset(const tinymath::vec3f& p) const;
		tinymath::vec3f inv_offset(const tinymath::vec3f& p) const;
	};
}
#endif
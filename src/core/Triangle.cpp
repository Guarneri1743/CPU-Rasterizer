#include "Triangle.hpp"
#include <assert.h>
#include "Pipeline.hpp"

namespace CpuRasterizer
{
	Triangle::Triangle()
	{
		culled = false;
		flip = false;
	}

	Triangle::Triangle(const Vertex verts[3])
	{
		culled = false;
		flip = false;
		for (int i = 0; i < 3; i++)
		{
			vertices[i] = verts[i];
		}
	}

	Triangle::Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3)
	{
		culled = false;
		flip = false;
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] = v3;
	}

	Triangle::Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, const bool& flip)
	{
		culled = false;
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] = v3;
		this->flip = flip;
	}

	// scan top/bottom Triangle
	// top-left-right | bottom-left-right(flipped)
	//===================================================
	//       top[0]
	//        /\
			//	   	 /  \
			//----------------------------- scanline[screen_y]
			//	   /	  \
			//    /        \
			//    ----------
			// left[1]    right[2]
			//
			// left[1]    right[2]
			//    ----------
			//    \        /
			//	   \      /
			//----------------------------- scanline[screen_y]
			//	     \  /
			//        \/
			//		bottom[0]
			//====================================================
	void Triangle::interpolate(float screen_y, Vertex& lhs, Vertex& rhs) const
	{
		float len = this->vertices[0].position.y - this->vertices[2].position.y;
		len = flip ? len : -len;
		float dy = flip ? screen_y - this->vertices[2].position.y : screen_y - this->vertices[0].position.y;
		float t = dy / len;
		assert(t > 0 && dy > 0);
		int l0, l1, r0, r1;
		l0 = flip ? 1 : 0;
		r0 = flip ? 2 : 0;
		l1 = flip ? 0 : 1;
		r1 = flip ? 0 : 2;
		lhs = Pipeline::interpolate_screen_space(this->vertices[l0], this->vertices[l1], t);
		rhs = Pipeline::interpolate_screen_space(this->vertices[r0], this->vertices[r1], t);
	}

	// split a Triangle to 1-2 triangles
	//===================================================
	//       top[0]
	//        /\
			//	   	 /  \
			//	    /	 \
			//	   /      \
			//     --------
			// left[1]    right[2]
			//
			// left[1]    right[2]
			//     --------
			//	   \      /
			//	    \    /
			//	     \  /
			//        \/
			//	    bottom[0]
			//====================================================
	std::vector<Triangle> Triangle::horizontally_split() const
	{
		std::vector<Triangle> ret;

		std::vector<Vertex> sorted;
		sorted.emplace_back(vertices[0]);
		sorted.emplace_back(vertices[1]);
		sorted.emplace_back(vertices[2]);
		std::sort(sorted.begin(), sorted.end(), [](const Vertex& lhs, const Vertex& rhs)
		{
			return lhs.position.y < rhs.position.y;
		});

		assert(sorted[0].position.y <= sorted[1].position.y);
		assert(sorted[1].position.y <= sorted[2].position.y);

		// segment
		if (sorted[0].position.y == sorted[1].position.y && sorted[1].position.y == sorted[2].position.y)
		{
			return ret;
		}

		// top triangle
		if (sorted[1].position.y == sorted[2].position.y)
		{
			if (sorted[1].position.x >= sorted[2].position.x)
			{
				ret.emplace_back(Triangle(sorted[0], sorted[2], sorted[1]));
			}
			else
			{
				ret.emplace_back(Triangle(sorted[0], sorted[1], sorted[2]));
			}
			return ret;
		}

		// bottom triangle
		if (sorted[0].position.y == sorted[1].position.y)
		{
			if (sorted[0].position.x >= sorted[1].position.x)
			{
				ret.emplace_back(Triangle(sorted[2], sorted[1], sorted[0], true));
			}
			else
			{
				ret.emplace_back(Triangle(sorted[2], sorted[0], sorted[1], true));
			}
			return ret;
		}

		// split triangles
		float mid_y = sorted[1].position.y;

		float t = (mid_y - sorted[0].position.y) / (sorted[2].position.y - sorted[0].position.y);

		// interpolate new vertex
		Vertex v = Pipeline::interpolate_screen_space(sorted[0], sorted[2], t);

		// top triangle: top-left-right
		if (v.position.x >= sorted[1].position.x)
		{
			ret.emplace_back(Triangle(sorted[0], sorted[1], v));
		}
		else
		{
			ret.emplace_back(Triangle(sorted[0], v, sorted[1]));
		}

		// bottom triangle: bottom-left-right
		if (v.position.x >= sorted[1].position.x)
		{
			ret.emplace_back(Triangle(sorted[2], sorted[1], v, true));
		}
		else
		{
			ret.emplace_back(Triangle(sorted[2], v, sorted[1], true));
		}

		return ret;
	}

	tinymath::Rect Triangle::get_bounds() const
	{
		return tinymath::Rect(tinymath::vec2i((int)vertices[0].position.x, (int)vertices[0].position.y),
							  tinymath::vec2i((int)vertices[1].position.x, (int)vertices[1].position.y),
							  tinymath::vec2i((int)vertices[2].position.x, (int)vertices[2].position.y));
	}

	float Triangle::area() const
	{
		auto e1 = vertices[1].position - vertices[0].position;
		auto e2 = vertices[2].position - vertices[0].position;
		tinymath::vec3f v1 = e1.xyz;
		tinymath::vec3f v2 = e2.xyz;
		return tinymath::magnitude(tinymath::cross(v1, v2)) / 2.0f;
	}

	float Triangle::area_double() const
	{
		auto e1 = vertices[1].position - vertices[0].position;
		auto e2 = vertices[2].position - vertices[0].position;
		tinymath::vec3f v1 = e1.xyz;
		tinymath::vec3f v2 = e2.xyz;
		return tinymath::magnitude(tinymath::cross(v1, v2));
	}

	bool Triangle::barycentric_interpolate(const tinymath::vec2f& pos, Vertex& interpolated_vert) const
	{
		int ccw_idx0 = 0;
		int ccw_idx1 = flip ? 2 : 1;
		int ccw_idx2 = flip ? 1 : 2;

		auto v0 = vertices[ccw_idx0];
		auto v1 = vertices[ccw_idx1];
		auto v2 = vertices[ccw_idx2];

		auto p0 = v0.position.xy;
		auto p1 = v1.position.xy;
		auto p2 = v2.position.xy;

		float w0 = Triangle::area_double(p1, p2, pos);
		float w1 = Triangle::area_double(p2, p0, pos);
		float w2 = Triangle::area_double(p0, p1, pos);

		float area = Triangle::area_double(p0, p1, p2);

		w0 /= area; w1 /= area; w2 /= area;
		interpolated_vert = Pipeline::barycentric_interpolate(v0, v1, v2, w0, w1, w2);

		if (w0 >= 0 && w1 >= 0 && w2 >= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	float Triangle::area_double(const tinymath::vec2f& v1, const tinymath::vec2f& v2, const tinymath::vec2f& v3)
	{
		return (v3.x - v1.x) * (v2.y - v1.y) - (v3.y - v1.y) * (v2.x - v1.x);
	}

	float Triangle::area_double(const tinymath::vec3f& v1, const tinymath::vec3f& v2, const tinymath::vec3f& v3)
	{
		auto e1 = v2 - v1;
		auto e2 = v3 - v1;
		return  tinymath::magnitude(tinymath::cross(e1, e2));
	}

	float Triangle::area(const tinymath::vec3f& v1, const tinymath::vec3f& v2, const tinymath::vec3f& v3)
	{
		auto e1 = v2 - v1;
		auto e2 = v3 - v1;
		return  tinymath::magnitude(tinymath::cross(e1, e2)) / 2.0f;
	}

	Vertex& Triangle::operator[](const uint32_t i)
	{
		return vertices[i];
	}

	const Vertex& Triangle::operator[](const uint32_t i) const
	{
		return vertices[i];
	}
}
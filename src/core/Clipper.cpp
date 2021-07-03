#include "Clipper.hpp"
#include <assert.h>
#include "Logger.hpp"

namespace Guarneri
{
	std::vector<Triangle> Clipper::near_plane_clipping(const Vertex& v1, const Vertex& v2, const Vertex& v3)
	{
		std::vector<Vertex> list_in = { v1, v2, v3 };
		std::vector<Vertex> list_out;

		char ld, cd;
		Vertex last = list_in[2];
		ld = last.position.w < 0.0f ? -1 : 1;

		for (size_t cur_idx = 0; cur_idx < list_in.size(); cur_idx++)
		{
			Vertex cur = list_in[cur_idx];
			cd = cur.position.w < 0.0f ? -1 : 1;

			if (cd != ld)
			{
				float t = cur.position.w / (cur.position.w - last.position.w + EPSILON);
				list_out.emplace_back(Vertex::interpolate(cur, last, t));
			}

			if (cd > 0)
			{
				list_out.emplace_back(cur);
			}

			ld = cd;
			last = cur;
		}

		if (list_out.size() < 3)
		{
			return {};
		}

		std::vector<Triangle> triangles;

		/*Vertex v0 = list_out[0];
		for (size_t idx = 1; idx < list_out.size() - 1; idx++)
		{
			Vertex v1 = list_out[idx];
			Vertex v2 = list_out[idx + 1];
			triangles.emplace_back(v0, v1, v2);
		}*/

		if (list_out.size() == 3)
		{
			triangles.emplace_back(Triangle(list_out[0], list_out[1], list_out[2]));
		}
		else if (list_out.size() == 4)
		{
			triangles.emplace_back(Triangle(list_out[0], list_out[1], list_out[2]));
			triangles.emplace_back(Triangle(list_out[2], list_out[3], list_out[0]));
		}

		return triangles;
	}

	std::vector<Triangle> Clipper::frustum_clipping(const Frustum& frustum, const Vertex& v1, const Vertex& v2, const Vertex& v3)
	{
		int inside_count = 0;
		int outside_count = 0;
		for (int i = 0; i < 6; i++)
		{
			auto& plane = frustum[i];
			if (plane.homo_distance(v1.position) < 0 || 
				plane.homo_distance(v2.position) < 0 || 
				plane.homo_distance(v3.position) < 0)
			{
				outside_count++;
			}

			if (plane.homo_distance(v1.position) >= 0 &&
				plane.homo_distance(v2.position) >= 0 &&
				plane.homo_distance(v3.position) >= 0)
			{
				inside_count++;
			}
		}

		if (inside_count == 6)
		{
			return { Triangle(v1, v2, v3) };
		}

		if (outside_count == 6)
		{
			return {};
		}

		std::vector<Vertex> list_out = {v1, v2, v3};
		for (int i = 0; i < 6; i++)
		{
			auto& plane = frustum[i];
			std::vector<Vertex> list_in(list_out);
			list_out.clear();
			for (size_t cur_idx = 0; cur_idx < list_in.size(); cur_idx++)
			{
				size_t last_idx = (cur_idx + list_in.size() - 1) % list_in.size();

				Vertex cur = list_in[cur_idx];
				Vertex last = list_in[last_idx];

				float d1, d2;
				d1 = plane.homo_distance(cur.position);
				d2 = plane.homo_distance(last.position);
				float t = d1 / (d1 - d2);

				if (d1 * d2 < 0)
				{
					list_out.emplace_back(Vertex::interpolate(cur, last, t));
				}

				if (d1 >= 0)
				{
					list_out.emplace_back(cur);
				}
			}
		}

		if (list_out.size() < 3)
		{
			return {};
		}

		std::vector<Triangle> triangles;

		Vertex v0 = list_out[0];
		for (size_t idx = 1; idx < list_out.size() - 1; idx++)
		{
			Vertex v1 = list_out[idx];
			Vertex v2 = list_out[idx + 1];
			triangles.emplace_back(v0, v1, v2);
		}

		return triangles;
	}

	void Clipper::screen_clipping(Vertex& lhs, Vertex& rhs, const int& width)
	{
		if (lhs.position.x <= 0.0f)
		{
			float t = -lhs.position.x / (rhs.position.x - lhs.position.x);
			lhs = Vertex::interpolate(lhs, rhs, t);
		}
		if (rhs.position.x >= width)
		{
			float t = ((float)width - lhs.position.x) / (rhs.position.x - lhs.position.x);
			rhs = Vertex::interpolate(lhs, rhs, t);
		}
	}

	bool Clipper::cvv_clipping(const Vector4& c1, const Vector4& c2, const Vector4& c3)
	{
		// z: [-w, w](GL) [0, w](DX)
		// x: [-w, w]
		// y: [-w, w]
		return cvv_clipping(c1) && cvv_clipping(c2) && cvv_clipping(c3);
	}

	bool Clipper::cvv_clipping(const Vector4& v)
	{
		float x, y, z, w;
		x = v.x; y = v.y; z = v.z; w = v.w;
		if (x < -w) return true;
		if (x > w) return true;
		if (y < -w) return true;
		if (y > w) return true;
		if (z < -w) return true;
		if (z > w) return true;
		return false;
	}

	bool Clipper::backface_culling(const Vector4& v1, const Vector4& v2, const Vector4& v3)
	{
		auto v1v2 = v2 - v1;
		auto v1v3 = v3 - v1;
		float ndv = Vector3::dot(Vector3::cross(v1v2.xyz(), v1v3.xyz()), Vector3::BACK);
		// front face: ndv >= 0
		// back face: ndv < 0
		return ndv < 0;
	}

	// todo: frustum & aabb, frustum & obb, etc.
	bool Clipper::frustum_culling_sphere(const Frustum& frustum, const Sphere& bounding_sphere)
	{
		for (int i = 0; i < 6; i++)
		{
			auto plane = frustum[i];
			auto d = plane.distance(bounding_sphere.center);
			auto r = bounding_sphere.radius;
			if (d < 0 && -d < r)
			{
				return true;
			}
		}
		return false;
	}

	bool Clipper::conservative_frustum_culling(const Frustum& frustum, const Vertex& v1, const Vertex& v2, const Vertex& v3)
	{
		for (int i = 0; i < 6; i++)
		{
			auto plane = frustum[i];
			auto d1 = plane.distance(v1.position.xyz());
			auto d2 = plane.distance(v2.position.xyz());
			auto d3 = plane.distance(v3.position.xyz());
			if (d1 < 0 && d2 < 0 && d3 < 0)
			{
				return true;
			}
		}
		return false;
	}
}
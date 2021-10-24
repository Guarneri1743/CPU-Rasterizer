#include "Clipper.hpp"
#include "Marcos.h"
#include <assert.h>
#include "Logger.hpp"
#include "Pipeline.hpp"

namespace CpuRasterizer
{
	bool Clipper::clip_segment(float near_plane, const tinymath::vec4f& c1, const tinymath::vec4f& c2, tinymath::vec4f& out_c1, tinymath::vec4f& out_c2)
	{
		int c1_inside = c1.w < near_plane ? -1 : 1;
		int c2_inside = c2.w < near_plane ? -1 : 1;

		if (c1_inside * c2_inside < 0)
		{
			float t = (c1.w - near_plane) / (c1.w - c2.w);
			auto interpolated = c1 + (c2 - c1) * t;
			if (c1_inside > 0)
			{
				out_c1 = c1;
				out_c2 = interpolated;
			}
			else
			{
				out_c1 = interpolated;
				out_c2 = c2;
			}

			return true;
		}
		else
		{
			if (c1_inside < 0)
			{
				return false;
			}
			else
			{
				out_c1 = c1;
				out_c2 = c2;
				return true;
			}
		}
	}

	/// <summary>
	/// clip triangle in certain space
	/// </summary>
	/// <param name="near_plane">camera's near plane</param>
	/// <param name="frustum">frustum in certain space, eg. an unit cube in homogenous space</param>
	/// <param name="c1">vertex in certain space</param>
	/// <param name="c2">vertex in certain space</param>
	/// <param name="c3">vertex in certain space</param>
	/// <returns></returns>
	std::vector<Triangle> Clipper::clip_triangle(float near_plane, const tinymath::Frustum& frustum, const Vertex& c1, const Vertex& c2, const Vertex& c3)
	{
		std::vector<Vertex> polygon = { c1, c2, c3 };

		// clipping against near plane
		{
			std::vector<Vertex> vertices(polygon);
			polygon.clear();
			Vertex last = c3;
			for (size_t cur_idx = 0; cur_idx < vertices.size(); cur_idx++)
			{
				Vertex cur = vertices[cur_idx];
				int cur_inside = cur.position.w < near_plane ? -1 : 1;
				int last_inside = last.position.w < near_plane ? -1 : 1;

				if (cur_inside * last_inside < 0)
				{
					float t = (cur.position.w - near_plane) / (cur.position.w - last.position.w);
					polygon.emplace_back(Pipeline::interpolate_clip_space(cur, last, t));
				}

				if (cur_inside > 0)
				{
					polygon.emplace_back(cur);
				}

				last = cur;
			}
		}

		if (polygon.size() < 3)
		{
			return {};
		}

		UNUSED(frustum);

		//// clipping against rest planes is not a must
		//for (int i = 1; i < 6; i++)
		//{
		//	auto& plane = frustum[i];
		//	if (polygon.size() >= 3)
		//	{
		//		std::vector<Vertex> vertices(polygon);
		//		polygon.clear();
		//		Vertex last = vertices[vertices.size() - 1];
		//		for (size_t cur_idx = 0; cur_idx < vertices.size(); cur_idx++)
		//		{
		//			Vertex cur = vertices[cur_idx];

		//			float d1, d2;
		//			d1 = plane.homo_distance(cur.position);
		//			d2 = plane.homo_distance(last.position);

		//			float t = d1 / (d1 - d2);

		//			if (d1 * d2 < 0.0f)
		//			{
		//				polygon.emplace_back(Vertex::interpolate_clip_space(cur, last, t));
		//			}

		//			if (d1 > 0.0f)
		//			{
		//				polygon.emplace_back(cur);
		//			}

		//			last = cur;
		//		}
		//	}
		//}

		//if (polygon.size() < 3)
		//{
		//	return {};
		//}

		// naive triagnle assembly
		// todo: strip mode
		std::vector<Triangle> triangles;

		for (size_t idx = 1; idx < polygon.size() - 1; idx++)
		{
			triangles.emplace_back(polygon[0], polygon[idx], polygon[idx + 1]);
		}

		return triangles;
	}

	/// <summary>
	/// clip in screen space
	/// </summary>
	/// <param name="lhs">left screen vertex</param>
	/// <param name="rhs">right screen vertex</param>
	/// <param name="width">screen width</param>
	void Clipper::clip_horizontally(Vertex& lhs, Vertex& rhs, size_t width)
	{
		if (lhs.position.x <= 0.0f)
		{
			float t = -lhs.position.x / (rhs.position.x - lhs.position.x);
			lhs = Pipeline::interpolate_screen_space(lhs, rhs, t);
		}
		if (rhs.position.x >= width)
		{
			float t = ((float)width - lhs.position.x) / (rhs.position.x - lhs.position.x);
			rhs = Pipeline::interpolate_screen_space(lhs, rhs, t);
		}
	}

	/// <summary>
	/// cull against cvv in homogenous clip space
	/// </summary>
	/// <param name="c1">vertex in clip space</param>
	/// <param name="c2">vertex in clip space</param>
	/// <param name="c3">vertex in clip space</param>
	/// <returns></returns>
	bool Clipper::inside_cvv(const tinymath::vec4f& c1, const tinymath::vec4f& c2, const tinymath::vec4f& c3)
	{
		return inside_cvv(c1) && inside_cvv(c2) && inside_cvv(c3);
	}

	/// <summary>
	/// cull against cvv in homogenous clip space
	/// </summary>
	/// <param name="v">vertex in clip space</param>
	/// <returns></returns>
	bool Clipper::inside_cvv(const tinymath::vec4f& v)
	{
		// z: [-w, w](GL) [0, w](DX)
		// x: [-w, w]
		// y: [-w, w]
		float x, y, z, w;
		x = v.x; y = v.y; z = v.z; w = v.w;
		if (x < -w) return false;
		if (x > w) return false;
		if (y < -w) return false;
		if (y > w) return false;
		if (z < 0.0f) return false;
		if (z > w) return false;
		return true;
	}

	bool Clipper::backface_culling_ndc(const tinymath::vec3f& normal)
	{
		float ndv = tinymath::dot(normal, tinymath::kVec3fBack);
		return ndv < 0.0f;
	}

	bool Clipper::backface_culling_ndc(const tinymath::vec3f& c1, const tinymath::vec3f& c2, const tinymath::vec3f& c3)
	{
		// front face: ndv >= 0
		// back face: ndv < 0
		auto seg1 = c2 - c1;
		auto seg2 = c3 - c1;
		float ndv = tinymath::dot(tinymath::cross(seg1, seg2), tinymath::kVec3fBack);
		return ndv < 0.0f;
	}

	// todo: frustum & aabb, frustum & obb, etc.
	bool Clipper::frustum_culling_sphere(const tinymath::Frustum& frustum, const tinymath::Sphere& bounding_sphere)
	{
		for (int i = 0; i < 6; i++)
		{
			auto& plane = frustum[i];
			auto d = plane.distance(bounding_sphere.center);
			auto r = bounding_sphere.radius;
			if (d < 0 && -d < r)
			{
				return true;
			}
		}
		return false;
	}

	bool Clipper::conservative_frustum_culling(const tinymath::Frustum& frustum, const Vertex& v1, const Vertex& v2, const Vertex& v3)
	{
		for (int i = 0; i < 6; i++)
		{
			auto& plane = frustum[i];
			auto d1 = plane.distance(v1.position.xyz);
			auto d2 = plane.distance(v2.position.xyz);
			auto d3 = plane.distance(v3.position.xyz);
			if (d1 < 0 && d2 < 0 && d3 < 0)
			{
				return true;
			}
		}
		return false;
	}
}
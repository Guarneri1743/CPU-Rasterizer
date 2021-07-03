#include "Clipper.hpp"
#include <assert.h>
#include "Logger.hpp"

namespace Guarneri
{
	/// <summary>
	/// clip against cvv in homogenous clip space
	/// </summary>
	/// <param name="near_plane">camera's near plane</param>
	/// <param name="cvv">an unit cube in homogenous space</param>
	/// <param name="c1">vertex in clip space</param>
	/// <param name="c2">vertex in clip space</param>
	/// <param name="c3">vertex in clip space</param>
	/// <returns></returns>
	std::vector<Triangle> Clipper::cvv_clipping(const float& near_plane, const Frustum& cvv, const Vertex& c1, const Vertex& c2, const Vertex& c3)
	{
		std::vector<Vertex> list_out = {c1, c2, c3};

		// clipping against near plane
		{
			std::vector<Vertex> list_in(list_out);
			list_out.clear();
			for (size_t cur_idx = 0; cur_idx < list_in.size(); cur_idx++)
			{
				size_t last_idx = (cur_idx + list_in.size() - 1) % list_in.size();

				Vertex cur = list_in[cur_idx];
				Vertex last = list_in[last_idx];

				int d1 = cur.position.w < near_plane ? -1 : 1;
				int d2 = last.position.w < near_plane ? -1 : 1;

				if (d1 * d2 < 0)
				{
					float t = (cur.position.w - near_plane) / (cur.position.w - last.position.w);
					list_out.emplace_back(Vertex::interpolate_clip_space(cur, last, t));
				}

				if (d1 > 0)
				{
					list_out.emplace_back(cur);
				}
			}
		}

		// clipping against rest planes
		for (int i = 1; i < 6; i++)
		{
			auto& plane = cvv[i];
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

				if (d1 * d2 < 0.0f)
				{
					list_out.emplace_back(Vertex::interpolate_clip_space(cur, last, t));
				}

				if (d1 > 0.0f)
				{
					list_out.emplace_back(cur);
				}
			}
		}

		if (list_out.size() < 3)
		{
			return {};
		}

		// naive triagnle assembly
		// todo: strip mode
		std::vector<Triangle> triangles;

		for (size_t idx = 1; idx < list_out.size() - 1; idx++)
		{
			triangles.emplace_back(list_out[0], list_out[idx], list_out[idx + 1]);
		}

		return triangles;
	}

	/// <summary>
	/// clip in screen space
	/// </summary>
	/// <param name="lhs">left screen vertex</param>
	/// <param name="rhs">right screen vertex</param>
	/// <param name="width">screen width</param>
	void Clipper::screen_clipping(Vertex& lhs, Vertex& rhs, const int& width)
	{
		if (lhs.position.x <= 0.0f)
		{
			float t = -lhs.position.x / (rhs.position.x - lhs.position.x);
			lhs = Vertex::interpolate_screen_space(lhs, rhs, t);
		}
		if (rhs.position.x >= width)
		{
			float t = ((float)width - lhs.position.x) / (rhs.position.x - lhs.position.x);
			rhs = Vertex::interpolate_screen_space(lhs, rhs, t);
		}
	}

	/// <summary>
	/// cull against cvv in homogenous clip space
	/// </summary>
	/// <param name="c1">vertex in clip space</param>
	/// <param name="c2">vertex in clip space</param>
	/// <param name="c3">vertex in clip space</param>
	/// <returns></returns>
	bool Clipper::cvv_culling(const Vector4& c1, const Vector4& c2, const Vector4& c3)
	{
		return cvv_culling(c1) && cvv_culling(c2) && cvv_culling(c3);
	}

	/// <summary>
	/// cull against cvv in homogenous clip space
	/// </summary>
	/// <param name="v">vertex in clip space</param>
	/// <returns></returns>
	bool Clipper::cvv_culling(const Vector4& v)
	{
		// z: [-w, w](GL) [0, w](DX)
		// x: [-w, w]
		// y: [-w, w]
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

	/// <summary>
	///  cull backface in homogenous clip space
	/// </summary>
	/// <param name="c1">vertex in clip space</param>
	/// <param name="c2">vertex in clip space</param>
	/// <param name="c3">vertex in clip space</param>
	/// <returns></returns>
	bool Clipper::backface_culling(const Vector4& c1, const Vector4& c2, const Vector4& c3)
	{
		// front face: ndv >= 0
		// back face: ndv < 0
		auto seg1 = c2 - c1;
		auto seg2 = c3 - c1;
		float ndv = Vector3::dot(Vector3::cross(seg1.xyz(), seg2.xyz()), Vector3::BACK);
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
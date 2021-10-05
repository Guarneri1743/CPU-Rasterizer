#pragma once
#include "RasterAttributes.hpp"
#include "tinymath.h"

namespace CpuRasterizor{
	class Pipeline
	{
	public:
		static Vertex barycentric_interpolate(const Vertex& v0, const Vertex& v1, const Vertex& v2, const float& w0, const float& w1, const float& w2);
		static Vertex interpolate_screen_space(const Vertex& left, const Vertex& right, const float& t);
		static Vertex interpolate_attributes(const Vertex& left, const Vertex& right, const float& t);
		static Vertex interpolate_clip_space(const Vertex& left, const Vertex& right, const float& t);
		static Vertex differential(const Vertex& lhs, const Vertex& rhs);
		static Vertex intagral(const Vertex& vert, const Vertex& differential);
		static Vertex substract(const Vertex& lhs, const Vertex& rhs);
		static Vertex clip2ndc(const Vertex& v);
		static Vertex reverse_perspective_division(const Vertex& v);
		static tinymath::vec4f clip2ndc(const tinymath::vec4f& v);
		static Vertex ndc2screen(const size_t& width, const size_t& height, const Vertex& v);
		static tinymath::vec4f ndc2screen(const size_t& width, const size_t& height, const tinymath::vec4f& v);
	};
}
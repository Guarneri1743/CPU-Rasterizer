#pragma once
#include <stdint.h>
#include "tinymath.h"
#include "RasterAttributes.hpp"

namespace CpuRasterizer{
	class Pipeline
	{
	public:
		static Vertex barycentric_interpolate(const Vertex& v0, const Vertex& v1, const Vertex& v2, float w0, float w1, float w2);
		static Vertex interpolate_screen_space(const Vertex& left, const Vertex& right, float t);
		static Vertex interpolate_attributes(const Vertex& left, const Vertex& right, float t);
		static Vertex interpolate_clip_space(const Vertex& left, const Vertex& right, float t);
		static Vertex differential(const Vertex& lhs, const Vertex& rhs);
		static Vertex intagral(const Vertex& vert, const Vertex& differential);
		static Vertex substract(const Vertex& lhs, const Vertex& rhs);
		static Vertex clip2ndc(const Vertex& v);
		static Vertex reverse_perspective_division(const Vertex& v);
		static tinymath::vec4f clip2ndc(const tinymath::vec4f& v);
		static Vertex ndc2screen(size_t width, size_t height, const Vertex& v);
		static tinymath::vec4f ndc2screen(size_t width, size_t height, const tinymath::vec4f& v);
		static float linearize_depth(float depth, float near, float far);
		static float linearize_01depth(float depth, float near, float far);
	};
}
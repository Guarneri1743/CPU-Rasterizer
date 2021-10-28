#include "Pipeline.hpp"

namespace CpuRasterizer
{
	Vertex Pipeline::barycentric_interpolate(const Vertex& v0, const Vertex& v1, const Vertex& v2, float w0, float w1, float w2)
	{
		Vertex ret;
		ret.position = v0.position * w0 + v1.position * w1 + v2.position * w2;
		ret.world_pos = v0.world_pos * w0 + v1.world_pos * w1 + v2.world_pos * w2;
		ret.color = v0.color * w0 + v1.color * w1 + v2.color * w2;
		ret.normal = v0.normal * w0 + v1.normal * w1 + v2.normal * w2;
		ret.uv = v0.uv * w0 + v1.uv * w1 + v2.uv * w2;
		ret.tangent = v0.tangent * w0 + v1.tangent * w1 + v2.tangent * w2;
		ret.bitangent = v0.bitangent * w0 + v1.bitangent * w1 + v2.bitangent * w2;

		ret.texcoord0 = v0.texcoord0 * w0 + v1.texcoord0 * w1 + v2.texcoord0 * w2;
		ret.texcoord1 = v0.texcoord1 * w0 + v1.texcoord1 * w1 + v2.texcoord1 * w2;
		ret.texcoord2 = v0.texcoord2 * w0 + v1.texcoord2 * w1 + v2.texcoord2 * w2;
		ret.texcoord3 = v0.texcoord3 * w0 + v1.texcoord3 * w1 + v2.texcoord3 * w2;
		ret.texcoord4 = v0.texcoord4 * w0 + v1.texcoord4 * w1 + v2.texcoord4 * w2;
		//ret.texcoord5 = v0.texcoord5 * w0 + v1.texcoord5 * w1 + v2.texcoord5 * w2;
		//ret.texcoord6 = v0.texcoord6 * w0 + v1.texcoord6 * w1 + v2.texcoord6 * w2;
		//ret.texcoord7 = v0.texcoord7 * w0 + v1.texcoord7 * w1 + v2.texcoord7 * w2;
		//ret.texcoord8 = v0.texcoord8 * w0 + v1.texcoord8 * w1 + v2.texcoord8 * w2;

		ret.rhw = v0.rhw * w0 + v1.rhw * w1 + v2.rhw * w2;
		return ret;
	}

	Vertex Pipeline::interpolate_screen_space(const Vertex& lhs, const Vertex& rhs, float t)
	{
		Vertex ret = interpolate_attributes(lhs, rhs, t);
		ret.rhw = lhs.rhw + (rhs.rhw - lhs.rhw) * t;
		return ret;
	}

	Vertex Pipeline::interpolate_clip_space(const Vertex& lhs, const Vertex& rhs, float t)
	{
		Vertex ret = interpolate_attributes(lhs, rhs, t);
		ret.rhw = 1.0f / ret.position.w;
		return ret;
	}

	Vertex Pipeline::interpolate_attributes(const Vertex& lhs, const Vertex& rhs, float t)
	{
		Vertex ret;
		ret.position = lhs.position + (rhs.position - lhs.position) * t;
		ret.world_pos = lhs.world_pos + (rhs.world_pos - lhs.world_pos) * t;
		ret.color = lhs.color + (rhs.color - lhs.color) * t;
		ret.normal = lhs.normal + (rhs.normal - lhs.normal) * t;
		ret.uv = lhs.uv + (rhs.uv - lhs.uv) * t;
		ret.tangent = lhs.tangent + (rhs.tangent - lhs.tangent) * t;
		ret.bitangent = lhs.bitangent + (rhs.bitangent - lhs.bitangent) * t;

		ret.texcoord0 = lhs.texcoord0 + (rhs.texcoord0 - lhs.texcoord0) * t;
		ret.texcoord1 = lhs.texcoord1 + (rhs.texcoord1 - lhs.texcoord1) * t;
		ret.texcoord2 = lhs.texcoord2 + (rhs.texcoord2 - lhs.texcoord2) * t;
		ret.texcoord3 = lhs.texcoord3 + (rhs.texcoord3 - lhs.texcoord3) * t;
		ret.texcoord4 = lhs.texcoord4 + (rhs.texcoord4 - lhs.texcoord4) * t;
		ret.texcoord5 = lhs.texcoord5 + (rhs.texcoord5 - lhs.texcoord5) * t;
		ret.texcoord6 = lhs.texcoord6 + (rhs.texcoord6 - lhs.texcoord6) * t;
		ret.texcoord7 = lhs.texcoord7 + (rhs.texcoord7 - lhs.texcoord7) * t;
		ret.texcoord8 = lhs.texcoord8 + (rhs.texcoord8 - lhs.texcoord8) * t;

		return ret;
	}

	Vertex Pipeline::differential(const Vertex& lhs, const Vertex& rhs)
	{
		float width = rhs.position.x - lhs.position.x;
		float segmentation = 1.0f / width;
		Vertex ret;
		ret.position = (rhs.position - lhs.position) * segmentation;
		ret.world_pos = (rhs.world_pos - lhs.world_pos) * segmentation;
		ret.color = (rhs.color - lhs.color) * segmentation;
		ret.uv = (rhs.uv - lhs.uv) * segmentation;
		ret.normal = (rhs.normal - lhs.normal) * segmentation;
		ret.tangent = (rhs.tangent - lhs.tangent) * segmentation;
		ret.bitangent = (rhs.bitangent - lhs.bitangent) * segmentation;
		ret.rhw = (rhs.rhw - lhs.rhw) * segmentation;

		ret.texcoord0 = (rhs.texcoord0 - lhs.texcoord0) * segmentation;
		ret.texcoord1 = (rhs.texcoord1 - lhs.texcoord1) * segmentation;
		ret.texcoord2 = (rhs.texcoord2 - lhs.texcoord2) * segmentation;
		ret.texcoord3 = (rhs.texcoord3 - lhs.texcoord3) * segmentation;
		ret.texcoord4 = (rhs.texcoord4 - lhs.texcoord4) * segmentation;
		ret.texcoord5 = (rhs.texcoord5 - lhs.texcoord5) * segmentation;
		ret.texcoord6 = (rhs.texcoord6 - lhs.texcoord6) * segmentation;
		ret.texcoord7 = (rhs.texcoord7 - lhs.texcoord7) * segmentation;
		ret.texcoord8 = (rhs.texcoord8 - lhs.texcoord8) * segmentation;

		return ret;
	}

	Vertex Pipeline::intagral(const Vertex& lhs, const Vertex& differential)
	{
		Vertex ret;
		ret.position = (lhs.position + differential.position);
		ret.world_pos = (lhs.world_pos + differential.world_pos);
		ret.color = (lhs.color + differential.color);
		ret.normal = (lhs.normal + differential.normal);
		ret.uv = (lhs.uv + differential.uv);
		ret.tangent = (lhs.tangent + differential.tangent);
		ret.bitangent = (lhs.bitangent + differential.bitangent);
		ret.rhw = (lhs.rhw + differential.rhw);

		ret.texcoord0 = (lhs.texcoord0 + differential.texcoord0);
		ret.texcoord1 = (lhs.texcoord1 + differential.texcoord1);
		ret.texcoord2 = (lhs.texcoord2 + differential.texcoord2);
		ret.texcoord3 = (lhs.texcoord3 + differential.texcoord3);
		ret.texcoord4 = (lhs.texcoord4 + differential.texcoord4);
		ret.texcoord5 = (lhs.texcoord5 + differential.texcoord5);
		ret.texcoord6 = (lhs.texcoord6 + differential.texcoord6);
		ret.texcoord7 = (lhs.texcoord7 + differential.texcoord7);
		ret.texcoord8 = (lhs.texcoord8 + differential.texcoord8);

		return ret;
	}

	Vertex Pipeline::substract(const Vertex& lhs, const Vertex& rhs)
	{
		Vertex ret;
		ret.position = (rhs.position - lhs.position);
		ret.world_pos = (rhs.world_pos - lhs.world_pos);
		ret.color = (rhs.color - lhs.color);
		ret.uv = (rhs.uv - lhs.uv);
		ret.normal = (rhs.normal - lhs.normal);
		ret.tangent = (rhs.tangent - lhs.tangent);
		ret.bitangent = (rhs.bitangent - lhs.bitangent);
		ret.rhw = (rhs.rhw - lhs.rhw);

		ret.texcoord0 = (rhs.texcoord0 - lhs.texcoord0);
		ret.texcoord1 = (rhs.texcoord1 - lhs.texcoord1);
		ret.texcoord2 = (rhs.texcoord2 - lhs.texcoord2);
		ret.texcoord3 = (rhs.texcoord3 - lhs.texcoord3);
		ret.texcoord4 = (rhs.texcoord4 - lhs.texcoord4);
		ret.texcoord5 = (rhs.texcoord5 - lhs.texcoord5);
		ret.texcoord6 = (rhs.texcoord6 - lhs.texcoord6);
		ret.texcoord7 = (rhs.texcoord7 - lhs.texcoord7);
		ret.texcoord8 = (rhs.texcoord8 - lhs.texcoord8);

		return ret;
	}

	Vertex Pipeline::clip2ndc(const Vertex& clip)
	{
		Vertex ndc = clip;
		ndc.position /= clip.position.w;
		ndc.world_pos /= clip.position.w;
		ndc.color /= clip.position.w;
		ndc.normal /= clip.position.w;
		ndc.uv /= clip.position.w;
		ndc.tangent /= clip.position.w;
		ndc.bitangent /= clip.position.w;

		ndc.texcoord0 /= clip.position.w;
		ndc.texcoord1 /= clip.position.w;
		ndc.texcoord2 /= clip.position.w;
		ndc.texcoord3 /= clip.position.w;
		ndc.texcoord4 /= clip.position.w;
		ndc.texcoord5 /= clip.position.w;
		ndc.texcoord6 /= clip.position.w;
		ndc.texcoord7 /= clip.position.w;
		ndc.texcoord8 /= clip.position.w;

		return ndc;
	}

	Vertex Pipeline::reverse_perspective_division(const Vertex& v)
	{
		Vertex ret;
		float w = 1.0f / v.rhw;
		ret.position = v.position * w;
		ret.world_pos = v.world_pos * w;
		ret.color = v.color * w;
		ret.normal = v.normal * w;
		ret.uv = v.uv * w;
		ret.tangent = v.tangent * w;
		ret.bitangent = v.bitangent * w;

		ret.texcoord0 = v.texcoord0 * w;
		ret.texcoord1 = v.texcoord1 * w;
		ret.texcoord2 = v.texcoord2 * w;
		ret.texcoord3 = v.texcoord3 * w;
		ret.texcoord4 = v.texcoord4 * w;
		ret.texcoord5 = v.texcoord5 * w;
		ret.texcoord6 = v.texcoord6 * w;
		ret.texcoord7 = v.texcoord7 * w;
		ret.texcoord8 = v.texcoord8 * w;

		return ret;
	}

	Vertex Pipeline::ndc2screen(size_t width, size_t height, const Vertex& ndc)
	{
		Vertex screen = ndc;
		screen.position = ndc2screen(width, height, ndc.position);
		return screen;
	}

	tinymath::vec4f Pipeline::clip2ndc(const tinymath::vec4f& clip)
	{
		tinymath::vec4f ndc = clip;
		ndc *= 1.0f / clip.w;
		return ndc;
	}

	tinymath::vec4f Pipeline::ndc2screen(size_t width, size_t height, const tinymath::vec4f& ndc)
	{
		tinymath::vec4f screen = ndc;
		screen.x = (ndc.x + 1.0f) * width * 0.5f;
		screen.y = (ndc.y + 1.0f) * height * 0.5f;
		screen.z = ndc.z * 0.5f + 0.5f;
		screen.w = ndc.w;
		return screen;
	}

	float Pipeline::linearize_depth(float depth, float near, float far)
	{
#ifdef GL_LIKE
		float ndc_z = depth * 2.0f - 1.0f;  // [0, 1] -> [-1, 1] (GL)
#else
		float ndc_z = depth; // [0, 1] (DX)
#endif

#ifdef LEFT_HANDED 
#ifdef GL_LIKE
		return (2.0f * near * far) / (far + near - ndc_z * (far - near));
#else
		return (far * near) / (far - (far - near) * ndc_z);
#endif
#else
#ifdef GL_LIKE
		return (2.0f * near * far) / (-(far + near) - ndc_z * (far - near));
#else
		return (far * near) / (-far - (far - near) * ndc_z);
#endif
#endif
	}

	float Pipeline::linearize_01depth(float depth, float near, float far)
	{
		return (linearize_depth(depth, near, far) - near) / (far - near);
	}
}
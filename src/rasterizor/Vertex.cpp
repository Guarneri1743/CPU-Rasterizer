#include "Vertex.hpp"
#include <sstream>
#include "Serialization.hpp"

namespace Guarneri
{
	Vertex::Vertex()
	{
		position = tinymath::vec4f();
		world_pos = tinymath::vec3f();
		shadow_coord = tinymath::vec4f();
		color = tinymath::vec4f();
		uv = tinymath::vec2f();
		normal = tinymath::vec3f();
		tangent = tinymath::vec3f();
		bitangent = tinymath::vec3f();
		this->rhw = 1.0f;
	}

	Vertex::Vertex(const tinymath::vec4f& _position, const tinymath::vec3f& _world_pos, tinymath::vec4f& _shadow_coord, const tinymath::vec4f& _color, const tinymath::vec3f& _normal, const tinymath::vec2f& _uv, const tinymath::vec3f& _tangent, const tinymath::vec3f& _bitangent)
	{
		this->position = _position;
		this->world_pos = _world_pos;
		this->shadow_coord = _shadow_coord;
		this->color = _color;
		this->normal = _normal;
		this->uv = _uv;
		this->tangent = _tangent;
		this->bitangent = _bitangent;
		this->rhw = 1.0f / this->position.w;
	}

	Vertex::Vertex(const tinymath::vec4f& _position, const tinymath::vec3f& _normal, const tinymath::vec2f& _uv)
	{
		this->position = _position;
		this->world_pos = _position.xyz;
		this->shadow_coord = tinymath::kVec4fZero;
		this->color = tinymath::kVec4fOne;
		this->normal = _normal;
		this->uv = _uv;
		this->tangent = tinymath::kVec3fZero;
		this->bitangent = tinymath::kVec3fZero;
		this->rhw = 1.0f / this->position.w;
	}

	Vertex Vertex::barycentric_interpolate(const Vertex& v0, const Vertex& v1, const Vertex& v2, const float& w0, const float& w1, const float& w2)
	{
		Vertex ret;
		ret.position = v0.position * w0 + v1.position * w1 + v2.position * w2;
		ret.world_pos = v0.world_pos * w0 + v1.world_pos * w1 + v2.world_pos * w2;
		ret.shadow_coord = v0.shadow_coord * w0 + v1.shadow_coord * w1 + v2.shadow_coord * w2;
		ret.color = v0.color * w0 + v1.color * w1 + v2.color * w2;
		ret.normal = v0.normal * w0 + v1.normal * w1 + v2.normal * w2;
		ret.uv = v0.uv * w0 + v1.uv * w1 + v2.uv * w2;
		ret.tangent = v0.tangent * w0 + v1.tangent * w1 + v2.tangent * w2;
		ret.bitangent = v0.bitangent * w0 + v1.bitangent * w1 + v2.bitangent * w2;
		ret.rhw = v0.rhw * w0 + v1.rhw * w1 + v2.rhw * w2;
		return ret;
	}

	Vertex Vertex::interpolate_screen_space(const Vertex& left, const Vertex& right, const float& t)
	{
		Vertex ret = interpolate_attributes(left, right, t);
		ret.rhw = left.rhw + (right.rhw - left.rhw) * t;
		return ret;
	}

	Vertex Vertex::interpolate_clip_space(const Vertex& left, const Vertex& right, const float& t)
	{
		Vertex ret = interpolate_attributes(left, right, t);
		ret.rhw = 1.0f / ret.position.w;
		return ret;
	}

	Vertex Vertex::interpolate_attributes(const Vertex& left, const Vertex& right, const float& t)
	{
		Vertex ret;
		ret.position = left.position + (right.position - left.position) * t;
		ret.world_pos = left.world_pos + (right.world_pos - left.world_pos) * t;
		ret.shadow_coord = left.shadow_coord + (right.shadow_coord - left.shadow_coord) * t;
		ret.color = left.color + (right.color - left.color) * t;
		ret.normal = left.normal + (right.normal - left.normal) * t;
		ret.uv = left.uv + (right.uv - left.uv) * t;
		ret.tangent = left.tangent + (right.tangent - left.tangent) * t;
		ret.bitangent = left.bitangent + (right.bitangent - left.bitangent) * t;
		return ret;
	}

	Vertex Vertex::differential(const Vertex& lhs, const Vertex& rhs)
	{
		float width = rhs.position.x - lhs.position.x;
		float segmentation = 1.0f / width;
		Vertex ret;
		ret.position = (rhs.position - lhs.position) * segmentation;
		ret.world_pos = (rhs.world_pos - lhs.world_pos) * segmentation;
		ret.shadow_coord = (rhs.shadow_coord - lhs.shadow_coord) * segmentation;
		ret.color = (rhs.color - lhs.color) * segmentation;
		ret.uv = (rhs.uv - lhs.uv) * segmentation;
		ret.normal = (rhs.normal - lhs.normal) * segmentation;
		ret.tangent = (rhs.tangent - lhs.tangent) * segmentation;
		ret.bitangent = (rhs.bitangent - lhs.bitangent) * segmentation;
		ret.rhw = (rhs.rhw - lhs.rhw) * segmentation;
		return ret;
	}

	Vertex Vertex::intagral(const Vertex& left, const Vertex& differential)
	{
		Vertex ret;
		ret.position = (left.position + differential.position);
		ret.world_pos = (left.world_pos + differential.world_pos);
		ret.shadow_coord = (left.shadow_coord + differential.shadow_coord);
		ret.color = (left.color + differential.color);
		ret.normal = (left.normal + differential.normal);
		ret.uv = (left.uv + differential.uv);
		ret.tangent = (left.tangent + differential.tangent);
		ret.bitangent = (left.bitangent + differential.bitangent);
		ret.rhw = (left.rhw + differential.rhw);
		return ret;
	}

	Vertex Vertex::clip2ndc(const Vertex& clip)
	{
		Vertex ndc = clip;
		ndc.position /= clip.position.w;
		ndc.world_pos /= clip.position.w;
		ndc.shadow_coord /= clip.position.w;
		ndc.color /= clip.position.w;
		ndc.normal /= clip.position.w;
		ndc.uv /= clip.position.w;
		ndc.tangent /= clip.position.w;
		ndc.bitangent /= clip.position.w;
		return ndc;
	}

	Vertex Vertex::ndc2screen(const size_t& width, const size_t& height, const Vertex& ndc)
	{
		Vertex screen = ndc;
		screen.position = ndc2screen(width, height, ndc.position);
		return screen;
	}

	tinymath::vec4f Vertex::clip2ndc(const tinymath::vec4f& clip)
	{
		tinymath::vec4f ndc = clip;
		ndc *= 1.0f / clip.w;
		return ndc;
	}

	tinymath::vec4f Vertex::ndc2screen(const size_t& width, const size_t& height, const tinymath::vec4f& ndc)
	{
		tinymath::vec4f screen = ndc;
		screen.x = (ndc.x + 1.0f) * width * 0.5f;
		screen.y = (ndc.y + 1.0f) * height * 0.5f;
		screen.z = ndc.z * 0.5f + 0.5f;
		screen.w = ndc.w;
		return screen;
	}
}
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

	Vertex Vertex::interpolate_screen_space(const Vertex& lhs, const Vertex& rhs, const float& t)
	{
		Vertex ret = interpolate_attributes(lhs, rhs, t);
		ret.rhw = lhs.rhw + (rhs.rhw - lhs.rhw) * t;
		return ret;
	}

	Vertex Vertex::interpolate_clip_space(const Vertex& lhs, const Vertex& rhs, const float& t)
	{
		Vertex ret = interpolate_attributes(lhs, rhs, t);
		ret.rhw = 1.0f / ret.position.w;
		return ret;
	}

	Vertex Vertex::interpolate_attributes(const Vertex& lhs, const Vertex& rhs, const float& t)
	{
		Vertex ret;
		ret.position = lhs.position + (rhs.position - lhs.position) * t;
		ret.world_pos = lhs.world_pos + (rhs.world_pos - lhs.world_pos) * t;
		ret.shadow_coord = lhs.shadow_coord + (rhs.shadow_coord - lhs.shadow_coord) * t;
		ret.color = lhs.color + (rhs.color - lhs.color) * t;
		ret.normal = lhs.normal + (rhs.normal - lhs.normal) * t;
		ret.uv = lhs.uv + (rhs.uv - lhs.uv) * t;
		ret.tangent = lhs.tangent + (rhs.tangent - lhs.tangent) * t;
		ret.bitangent = lhs.bitangent + (rhs.bitangent - lhs.bitangent) * t;
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

	Vertex Vertex::intagral(const Vertex& lhs, const Vertex& differential)
	{
		Vertex ret;
		ret.position = (lhs.position + differential.position);
		ret.world_pos = (lhs.world_pos + differential.world_pos);
		ret.shadow_coord = (lhs.shadow_coord + differential.shadow_coord);
		ret.color = (lhs.color + differential.color);
		ret.normal = (lhs.normal + differential.normal);
		ret.uv = (lhs.uv + differential.uv);
		ret.tangent = (lhs.tangent + differential.tangent);
		ret.bitangent = (lhs.bitangent + differential.bitangent);
		ret.rhw = (lhs.rhw + differential.rhw);
		return ret;
	}

	Vertex Vertex::substract(const Vertex& lhs, const Vertex& rhs)
	{
		Vertex ret;
		ret.position = (rhs.position - lhs.position);
		ret.world_pos = (rhs.world_pos - lhs.world_pos);
		ret.shadow_coord = (rhs.shadow_coord - lhs.shadow_coord);
		ret.color = (rhs.color - lhs.color);
		ret.uv = (rhs.uv - lhs.uv);
		ret.normal = (rhs.normal - lhs.normal);
		ret.tangent = (rhs.tangent - lhs.tangent);
		ret.bitangent = (rhs.bitangent - lhs.bitangent);
		ret.rhw = (rhs.rhw - lhs.rhw);
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

	Vertex Vertex::reverse_perspective_division(const Vertex& v)
	{
		Vertex ret;
		float w = 1.0f / v.rhw;
		ret.position = v.position * w;
		ret.world_pos = v.world_pos * w;
		ret.shadow_coord = v.shadow_coord * w;
		ret.color = v.color * w;
		ret.normal = v.normal * w;
		ret.uv = v.uv * w;
		ret.tangent = v.tangent * w;
		ret.bitangent = v.bitangent * w;
		return ret;
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
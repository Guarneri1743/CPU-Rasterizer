#pragma once
#include "tinymath.h"

namespace CpuRasterizor
{
	struct RasterAttributes
	{
		tinymath::vec4f position;
		tinymath::vec3f world_pos;
		tinymath::vec4f shadow_coord;
		tinymath::vec4f color;
		tinymath::vec3f normal;
		tinymath::vec2f uv;
		tinymath::vec3f tangent;
		tinymath::vec3f bitangent;
		float rhw;

		RasterAttributes::RasterAttributes()
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

		RasterAttributes::RasterAttributes(const tinymath::vec4f& _position, const tinymath::vec3f& _world_pos, tinymath::vec4f& _shadow_coord, const tinymath::vec4f& _color, const tinymath::vec3f& _normal, const tinymath::vec2f& _uv, const tinymath::vec3f& _tangent, const tinymath::vec3f& _bitangent)
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

		RasterAttributes::RasterAttributes(const tinymath::vec4f& _position, const tinymath::vec3f& _normal, const tinymath::vec2f& _uv)
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
	};

	typedef RasterAttributes Vertex;
	typedef RasterAttributes Fragment;
}
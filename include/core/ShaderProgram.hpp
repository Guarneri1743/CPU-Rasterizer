#pragma once
#include <string>
#include "Singleton.hpp"
#include "GlobalShaderParams.hpp"
#include "Light.hpp"
#include "tinymath.h"
#include "Texture.hpp"
#include "ShaderPropertyMap.hpp"
#include "RasterAttributes.hpp"

#undef near
#undef far

namespace CpuRasterizer
{
	static tinymath::Color mip_colors[kMaxMip] =
	{
		tinymath::Color(1.f, 0.f, 0.f, 1.f),
		tinymath::Color(0.f, 0.f, 1.f, 1.f),
		tinymath::Color(1.f, 0.5f, 0.f, 1.f),
		tinymath::Color(1.f, 0.f, 0.5f, 1.f),
		tinymath::Color(0.f, 0.5f, 0.5f, 1.f),
		tinymath::Color(0.f, 0.25f, 0.5f, 1.f),
		tinymath::Color(0.25f, 0.5f, 0.f, 1.f),
		tinymath::Color(0.5f, 0.f, 1.f, 1.f),
	};

	struct a2v
	{
		tinymath::vec4f position;
		tinymath::vec2f uv;
		tinymath::vec4f color;
		tinymath::vec3f normal;
		tinymath::vec3f tangent;
		tinymath::vec4f texcoord0;
		tinymath::vec4f texcoord1;
		tinymath::vec4f texcoord2;
		tinymath::vec4f texcoord3;
		tinymath::vec4f texcoord4;
		tinymath::vec4f texcoord5;
		tinymath::vec4f texcoord6;
		tinymath::vec4f texcoord7;
		tinymath::vec4f texcoord8;
	};

	struct v2f
	{
		tinymath::vec4f position;
		tinymath::vec3f world_pos;
		tinymath::vec2f uv;
		tinymath::vec4f color;
		tinymath::vec3f tangent;
		tinymath::vec3f bitangent;
		tinymath::vec3f normal;
		tinymath::vec4f texcoord0;
		tinymath::vec4f texcoord1;
		tinymath::vec4f texcoord2;
		tinymath::vec4f texcoord3;
		tinymath::vec4f texcoord4;
		tinymath::vec4f texcoord5;
		tinymath::vec4f texcoord6;
		tinymath::vec4f texcoord7; 
		tinymath::vec4f texcoord8;

		Fragment ddx;
		Fragment ddy;
	};

	inline a2v vertex_to_a2v(const Vertex& vert)
	{
		a2v ret;
		ret.position = vert.position;
		ret.uv = vert.uv;
		ret.color = vert.color;
		ret.tangent = vert.tangent;
		ret.normal = vert.normal;
		ret.texcoord0 = vert.texcoord0;
		ret.texcoord1 = vert.texcoord1;
		ret.texcoord2 = vert.texcoord2;
		ret.texcoord3 = vert.texcoord3;
		ret.texcoord4 = vert.texcoord4;
		ret.texcoord5 = vert.texcoord5;
		ret.texcoord6 = vert.texcoord6;
		ret.texcoord7 = vert.texcoord7;
		ret.texcoord8 = vert.texcoord8;
		return ret;
	}

	inline Vertex v2f_to_vertex(const v2f& vert)
	{
		Vertex ret;
		ret.position = vert.position;
		ret.world_pos = vert.world_pos;
		ret.uv = vert.uv;
		ret.color = vert.color;
		ret.tangent = vert.tangent;
		ret.bitangent = vert.bitangent;
		ret.normal = vert.normal;
		ret.texcoord0 = vert.texcoord0;
		ret.texcoord1 = vert.texcoord1;
		ret.texcoord2 = vert.texcoord2;
		ret.texcoord3 = vert.texcoord3;
		ret.texcoord4 = vert.texcoord4;
		ret.texcoord5 = vert.texcoord5;
		ret.texcoord6 = vert.texcoord6;
		ret.texcoord7 = vert.texcoord7;
		ret.texcoord8 = vert.texcoord8;
		ret.rhw = 1.0f / vert.position.w;
		return ret;
	}

	inline v2f frag_to_v2f(const Fragment& frag)
	{
		v2f ret;
		ret.position = frag.position;
		ret.world_pos = frag.world_pos;
		ret.uv = frag.uv;
		ret.color = frag.color;
		ret.tangent = frag.tangent;
		ret.bitangent = frag.bitangent;
		ret.normal = frag.normal;
		ret.texcoord0 = frag.texcoord0;
		ret.texcoord1 = frag.texcoord1;
		ret.texcoord2 = frag.texcoord2;
		ret.texcoord3 = frag.texcoord3;
		ret.texcoord4 = frag.texcoord4;
		ret.texcoord5 = frag.texcoord5;
		ret.texcoord6 = frag.texcoord6;
		ret.texcoord7 = frag.texcoord7;
		ret.texcoord8 = frag.texcoord8;
		return ret;
	}

	inline v2f vertex_to_v2f(const Vertex& vert)
	{
		v2f ret;
		ret.position = vert.position;
		ret.world_pos = vert.world_pos;
		ret.uv = vert.uv;
		ret.color = vert.color;
		ret.tangent = vert.tangent;
		ret.bitangent = vert.bitangent;
		ret.normal = vert.normal;
		ret.texcoord0 = vert.texcoord0;
		ret.texcoord1 = vert.texcoord1;
		ret.texcoord2 = vert.texcoord2;
		ret.texcoord3 = vert.texcoord3;
		ret.texcoord4 = vert.texcoord4;
		ret.texcoord5 = vert.texcoord5;
		ret.texcoord6 = vert.texcoord6;
		ret.texcoord7 = vert.texcoord7;
		ret.texcoord8 = vert.texcoord8;
		return ret;
	}

	class ShaderProgram
	{
	public:
		bool discarded = false;
		std::string name;
		ShaderPropertyMap local_properties;

	public:
		ShaderProgram(std::string _name) : name(_name) {}
		virtual ~ShaderProgram() {}

		virtual v2f vertex_shader(const a2v& input) const = 0;
		virtual tinymath::Color fragment_shader(const v2f& input) const = 0;

		inline tinymath::mat4x4 model() const { return local_properties.get_mat4x4(mat_model_prop); }
		inline tinymath::mat4x4 view() const { return local_properties.get_mat4x4(mat_view_prop); }
		inline tinymath::mat4x4 projection() const { return local_properties.get_mat4x4(mat_projection_prop); }
		inline tinymath::mat4x4 vp_matrix() const { return local_properties.get_mat4x4(mat_vp_prop); }
		inline tinymath::mat4x4 mvp_matrix() const { return local_properties.get_mat4x4(mat_mvp_prop); }
	};
}
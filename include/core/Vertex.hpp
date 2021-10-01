#ifndef _VERTEX_
#define _VERTEX_
#include <string>
#include "TinyMath.h"
#include "rapidjson/document.h"

namespace Guarneri
{
	struct Vertex
	{
	public:
		// serializable
		tinymath::vec4f position;
		tinymath::vec4f color;
		tinymath::vec2f uv;
		tinymath::vec3f tangent;
		tinymath::vec3f normal;

		// runtime
		tinymath::vec3f world_pos;
		tinymath::vec4f shadow_coord;
		tinymath::vec3f bitangent;
		float rhw;

	public:
		Vertex();
		Vertex(const tinymath::vec4f& _position, const tinymath::vec3f& _world_pos, tinymath::vec4f& _shadow_coord, const tinymath::vec4f& _color, const tinymath::vec3f& _normal, const tinymath::vec2f& _uv, const tinymath::vec3f& _tangent, const tinymath::vec3f& _bitangent);
		Vertex(const tinymath::vec4f& _position, const tinymath::vec3f& _normal, const tinymath::vec2f& _uv);
		static Vertex barycentric_interpolate(const Vertex& v0, const Vertex& v1, const Vertex& v2, const float& w0, const float& w1, const float& w2);
		static Vertex interpolate_screen_space(const Vertex& left, const Vertex& right, const float& t);
		static Vertex interpolate_attributes(const Vertex& left, const Vertex& right, const float& t);
		static Vertex interpolate_clip_space(const Vertex& left, const Vertex& right, const float& t);
		static Vertex differential(const Vertex& lhs, const Vertex& rhs);
		static Vertex intagral(const Vertex& left, const Vertex& differential);
		static Vertex clip2ndc(const Vertex& v);
		static tinymath::vec4f clip2ndc(const tinymath::vec4f& v);
		static Vertex ndc2screen(const int& width, const int& height, const Vertex& v);
		static tinymath::vec4f ndc2screen(const int& width, const int& height, const tinymath::vec4f& v);
		static rapidjson::Value serialize(rapidjson::Document& doc, const Vertex& v);
		static Vertex deserialize(const rapidjson::Value& v);
	};
}
#endif
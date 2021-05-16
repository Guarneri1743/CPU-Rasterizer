#ifndef _VERTEX_
#define _VERTEX_
#include <string>
#include <ostream>
#include <sstream>
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace Guarneri
{
	struct Vertex
	{
	public:
		Vector4 position;
		Vector3 world_pos;
		Vector4 shadow_coord;
		Vector4 color;
		Vector2 uv;
		Vector3 tangent;
		Vector3 bitangent;
		Vector3 normal;
		float rhw;

	public:
		Vertex();
		Vertex(const Vector4& _position, const Vector3& _world_pos, Vector4& _shadow_coord, const Vector4& _color, const Vector3& _normal, const Vector2& _uv, const Vector3& _tangent, const Vector3& _bitangent);
		Vertex(const Vector4& _position, const Vector3& _normal, const Vector2& _uv);
		void perspective_division();
		static Vertex barycentric_interpolate(const Vertex& v0, const Vertex& v1, const Vertex& v2, const float& w0, const float& w1, const float& w2);
		static Vertex interpolate(const Vertex& left, const Vertex& right, const float& t);
		static Vertex differential(const Vertex& lhs, const Vertex& rhs);
		static Vertex intagral(const Vertex& left, const Vertex& differential);
		std::string str() const;
	};

	static std::ostream& operator << (std::ostream& stream, const Vertex& v)
	{
		stream << v.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Vertex& v)
	{
		stream << v.str();
		return stream;
	}
}
#endif
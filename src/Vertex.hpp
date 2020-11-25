#ifndef _VERTEX_
#define _VERTEX_
#include <Guarneri.hpp>

namespace Guarneri {
	struct Vertex {
	public:
		Vertex() {
			position = Vector4();
			world_pos = Vector3();
			shadow_coord = Vector4();
			color = Vector4();
			uv = Vector2();
			normal = Vector3();
			tangent = Vector3();
			bitangent = Vector3();
			this->rhw = 1.0f;
		}

		Vertex(const Vector4& _position, const Vector3& _world_pos, Vector4& _shadow_coord, const Vector4& _color, const Vector3& _normal, const Vector2& _uv, const Vector3& _tangent, const Vector3& _bitangent) {
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

		Vertex(const Vector4& _position, const Vector3& _normal, const Vector2& _uv) {
			this->position = _position;
			this->world_pos = _position.xyz();
			this->shadow_coord = Vector4::ZERO;
			this->color = Vector4::ONE;
			this->normal = _normal;
			this->uv = _uv;
			this->tangent = Vector3::ZERO;
			this->bitangent = Vector3::ZERO;
			this->rhw = 1.0f / this->position.w;
		}

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
		void perspective_division(const float& _rhw) {
			world_pos *= _rhw;
			shadow_coord *= _rhw;
			color *= _rhw;
			normal *= _rhw;
			uv *= _rhw;
			tangent *= _rhw;
			bitangent *= _rhw;
		}

		static Vertex barycentric_interpolate(const Vertex& v0, const Vertex& v1, const Vertex& v2, const float& w0, const float& w1, const float& w2) {
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

		static Vertex interpolate(const Vertex& left, const Vertex& right, const float& t) {
			Vertex ret;
			ret.position = left.position + (right.position - left.position) * t;
			ret.world_pos = left.world_pos + (right.world_pos - left.world_pos) * t;
			ret.shadow_coord = left.shadow_coord + (right.shadow_coord - left.shadow_coord) * t;
			ret.color = left.color + (right.color - left.color) * t;
			ret.normal = left.normal + (right.normal - left.normal) * t;
			ret.uv = left.uv + (right.uv - left.uv) * t;
			ret.tangent = left.tangent + (right.tangent - left.tangent) * t;
			ret.bitangent = left.bitangent + (right.bitangent - left.bitangent) * t;
			ret.rhw = left.rhw + (right.rhw - left.rhw) * t;
			return ret;
		}

		static Vertex differential(const Vertex& lhs, const Vertex& rhs) {
			float w = rhs.position.x - lhs.position.x;
			float segmentation = 1.0f / w;
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

		static Vertex intagral(const Vertex& left, const Vertex& differential) {
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

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "Vertex: [pos: " << this->position << ", Color: " << this->color << ", uv: " << uv << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const Vertex& v) {
		stream << v.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Vertex& v) {
		stream << v.str();
		return stream;
	}
}
#endif
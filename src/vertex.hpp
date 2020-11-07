#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	struct vertex {
	public:
		vertex() {
			position = Vector4();
			world_pos = Vector3();
			Color = Vector4();
			uv = Vector2();
			normal = Vector3();
			tangent = Vector3();
			bitangent = Vector3();
			this->rhw = 1.0f;
		}

		vertex(const Vector4& position, const Vector3& world_pos, const Vector4& Color, const Vector3& normal, const Vector2& uv, const Vector3& tangent, const Vector3& bitangent) {
			this->position = position;
			this->world_pos = world_pos;
			this->Color = Color;
			this->normal = normal;
			this->uv = uv;
			this->tangent = tangent;
			this->bitangent = bitangent;
			this->rhw = 1.0f / this->position.w;
		}

		vertex(const Vector4& position, const Vector3& normal, const Vector2& uv) {
			this->position = position;
			this->world_pos = position.xyz();
			this->Color = Vector4::ONE;
			this->normal = normal;
			this->uv = uv;
			this->tangent = Vector3::ZERO;
			this->bitangent = Vector3::ZERO;
			this->rhw = 1.0f / this->position.w;
		}

	public:
		Vector4 position;
		Vector3 world_pos;
		Vector4 Color;
		Vector3 normal;
		Vector2 uv;
		Vector3 tangent;
		Vector3 bitangent;
		float rhw;

	public:
		void perspective_division(const float& rhw) {
			world_pos *= rhw;
			Color *= rhw;
			normal *= rhw;
			uv *= rhw;
			tangent *= rhw;
			bitangent *= rhw;
		}

		static vertex interpolate(const vertex& left, const vertex& right, const float& t) {
			vertex ret;
			ret.position = left.position + (right.position - left.position) * t;
			ret.position.w = 1.0f;
			ret.world_pos = left.world_pos + (right.world_pos - left.world_pos) * t;
			ret.Color = left.Color + (right.Color - left.Color) * t;
			ret.normal = left.normal + (right.normal - left.normal) * t;
			ret.uv = left.uv + (right.uv - left.uv) * t;
			ret.tangent = left.tangent + (right.tangent - left.tangent) * t;
			ret.bitangent = left.bitangent + (right.bitangent - left.bitangent) * t;
			ret.rhw = left.rhw + (right.rhw - left.rhw) * t;
			return ret;
		}

		static vertex differential(const vertex& lhs, const vertex& rhs) {
			float w = rhs.position.x - lhs.position.x;
			float segmentation = 1.0f / w;
			vertex ret;
			ret.position = (rhs.position - lhs.position) * segmentation;
			ret.world_pos = (rhs.world_pos - lhs.world_pos) * segmentation;
			ret.Color = (rhs.Color - lhs.Color) * segmentation;
			ret.uv = (rhs.uv - lhs.uv) * segmentation;
			ret.normal = (rhs.normal - lhs.normal) * segmentation;
			ret.tangent = (rhs.tangent - lhs.tangent) * segmentation;
			ret.bitangent = (rhs.bitangent - lhs.bitangent) * segmentation;
			ret.rhw = (rhs.rhw - lhs.rhw) * segmentation;
			return ret;
		}

		static vertex intagral(const vertex& left, const vertex& differential) {
			vertex ret;
			ret.position = (left.position + differential.position);
			ret.world_pos = (left.world_pos + differential.world_pos);
			ret.Color = (left.Color + differential.Color);
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
			ss << "vertex: [pos: " << this->position << ", Color: " << this->Color << ", uv: " << uv << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const vertex& v) {
		stream << v.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const vertex& v) {
		stream << v.str();
		return stream;
	}
}
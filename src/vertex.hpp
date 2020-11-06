#pragma once
#include <guarneri.hpp>

namespace guarneri {
	struct vertex {
	public:
		vertex() {
			position = float4();
			world_pos = float3();
			color = float4();
			uv = float2();
			normal = float3();
			tangent = float3();
			bitangent = float3();
			this->rhw = 1.0f;
		}

		vertex(const float4& position, const float3& world_pos, const float4& color, const float3& normal, const float2& uv, const float3& tangent, const float3& bitangent) {
			this->position = position;
			this->world_pos = world_pos;
			this->color = color;
			this->normal = normal;
			this->uv = uv;
			this->tangent = tangent;
			this->bitangent = bitangent;
			this->rhw = 1.0f / this->position.w;
		}

		vertex(const float4& position, const float3& normal, const float2& uv) {
			this->position = position;
			this->world_pos = position.xyz();
			this->color = float4::ONE;
			this->normal = normal;
			this->uv = uv;
			this->tangent = float3::ZERO;
			this->bitangent = float3::ZERO;
			this->rhw = 1.0f / this->position.w;
		}

	public:
		float4 position;
		float3 world_pos;
		float4 color;
		float3 normal;
		float2 uv;
		float3 tangent;
		float3 bitangent;
		float rhw;

	public:
		void perspective_division(const float& rhw) {
			world_pos *= rhw;
			color *= rhw;
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
			ret.color = left.color + (right.color - left.color) * t;
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
			ret.color = (rhs.color - lhs.color) * segmentation;
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
			ss << "vertex: [pos: " << this->position << ", color: " << this->color << ", uv: " << uv << "]";
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
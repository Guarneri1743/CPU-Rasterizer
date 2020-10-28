#pragma once
#include <common.hpp>
#include <float2.hpp>
#include <float4.hpp>

namespace guarneri {
	struct vertex {
	public:
		vertex() {
			position = float4();
			color = float4();
			uv = float2();
			normal = float3();
			tangent = float3();
			this->rhw = 1.0f;
		}

		vertex(const float4& position, const float4& color, const float3& normal, const float2& uv, const float3& tangent) {
			this->position = position;
			this->color = color;
			this->normal = normal;
			this->uv = uv;
			this->tangent = tangent;
			this->rhw = 1.0f / this->position.w;
		}

		vertex(const vertex& other) {
			this->position = other.position;
			this->color = other.color;
			this->uv = other.uv;
			this->normal = other.normal;
			this->tangent = other.tangent;
			this->rhw = other.rhw;
		}

	public:
		float4 position;
		float4 color;
		float3 normal;
		float2 uv;
		float3 tangent;
		float rhw;

	public:
		vertex perspective_division() {
			vertex ret = *this;
			ret.rhw = 1.0f / ret.position.w;
			ret.uv *= rhw;
			ret.color *= rhw;
			return ret;
		}

		static vertex add(const vertex& left, const vertex& right) {
			vertex ret;
			ret.position = (right.position + left.position);
			ret.color = (right.color + left.color);
			ret.normal = (right.normal + left.normal);
			ret.uv = (right.uv + left.uv);
			ret.tangent = (right.tangent + left.tangent);
			ret.rhw = (right.rhw + left.rhw);
			return ret;
		}

		static vertex interpolate(const vertex& left, const vertex& right, const float& t) {
			vertex ret;
			ret.position = left.position + (right.position - left.position) * t;
			ret.position.w = 1.0f;
			ret.color = left.color + (right.color - left.color) * t;
			ret.normal = left.normal + (right.normal - left.normal) * t;
			ret.uv = left.uv + (right.uv - left.uv) * t;
			ret.tangent = left.tangent + (right.tangent - left.tangent) * t;
			ret.rhw = left.rhw + (right.rhw - left.rhw) * t;
			return ret;
		}

		static vertex perspective_division(const vertex& left, const vertex& right, const float& w) {
			float rhw = 1.0f / w;
			vertex ret;
			ret.position = (right.position - left.position) * rhw;
			ret.color = (right.color - left.color) * rhw;
			ret.uv = (right.uv - left.uv) * rhw;
			ret.rhw = (right.rhw - left.rhw) * rhw;
			return ret;
		}

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "vertex: [pos: " << this->position << ", color: " << this->color << ", uv: " << uv << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const vertex& ray) {
		stream << ray.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const vertex& ray) {
		stream << ray.str();
		return stream;
	}
}
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
			rhw = 1.0f / position.w;
		}

		vertex(const vertex& other) {
			this->position = other.position;
			this->color = other.color;
			this->uv = other.uv;
			this->rhw = other.rhw;
		}

	public:
		float4 position;
		float4 color;
		float2 uv;
		float rhw;

	public:
		void apply_rhw() {
			this->rhw = 1.0f / position.w;
			this->uv *= this->rhw;
		}
	};
}
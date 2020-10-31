#pragma once
#include <float3.hpp>

namespace guarneri {
	struct segment{
	public:
		segment() {
			start = guarneri::float3::ZERO;
			end = guarneri::float3::ZERO;
		}

		segment(const guarneri::float3& center, const guarneri::float3& dir, const float& extent) {
			this->start = center - extent * dir;
			this->end = center + extent * dir;
		}

		segment(const guarneri::float3& start, const guarneri::float3& end) {
			this->start = start;
			this->end = end;
		}

		segment(const segment& other) {
			this->start = other.start;
			this->end = other.end;
		}

	public:
		guarneri::float3 start;
		guarneri::float3 end;

	public:
		guarneri::float3 center() const {
			return (this->start + this->end) / 2;
		}

		float extent() const {
			return float3::length(end, start) / 2;
		}

		guarneri::float3 dir() const {
			return float3::normalize(end - start);
		}

		std::string str() const {
			std::stringstream ss;
			ss << "segment: [start: " << this->start  << ", end: "<<this->end << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const segment& seg) {
		stream << seg.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const segment& seg) {
		stream << seg.str();
		return stream;
	}
}
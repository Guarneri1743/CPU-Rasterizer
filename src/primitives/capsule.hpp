#pragma once
#include <float3.hpp>
#include <segment.hpp>

namespace guarneri {
	struct capsule {
	public:
		capsule() {
			segment = segment::segment();
			radius = 0;
		}

		capsule(const guarneri::float3& center, const guarneri::float3& dir, const float& extent, const float& radius) {
			this->segment = segment::segment(center, dir, extent);
			this->radius = radius;
		}

		capsule(const guarneri::float3& start, const guarneri::float3& end, const float& radius) {
			this->segment = segment::segment(start, end);
			this->radius = radius;
		}

		capsule(const segment& segment, const float& radius) {
			this->segment = segment;
			this->radius = radius;
		}

		capsule(const capsule& other) {
			this->segment = other.segment;
			this->radius = other.radius;
		}

	public:
		segment segment;
		float radius;

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "capsule: [center: " << this->segment << ", size: " << std::setprecision(FLOAT_LOG_PRECISION) << this->radius << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const capsule& capsule) {
		stream << capsule.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const capsule& capsule) {
		stream << capsule.str();
		return stream;
	}
}
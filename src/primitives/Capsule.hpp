#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	struct Capsule {
	public:
		Capsule() {
			Segment = Segment::Segment();
			radius = 0;
		}

		Capsule(const Guarneri::Vector3& center, const Guarneri::Vector3& dir, const float& extent, const float& radius_t) {
			this->Segment = Segment::Segment(center, dir, extent);
			this->radius = radius_t;
		}

		Capsule(const Guarneri::Vector3& start, const Guarneri::Vector3& end, const float& radius) {
			this->Segment = Segment::Segment(start, end);
			this->radius = radius;
		}

		Capsule(const Segment& Segment, const float& radius) {
			this->Segment = Segment;
			this->radius = radius;
		}

		Capsule(const Capsule& other) {
			this->Segment = other.Segment;
			this->radius = other.radius;
		}

	public:
		Segment Segment;
		float radius;

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "Capsule: [center: " << this->Segment << ", size: " << std::setprecision(FLOAT_LOG_PRECISION) << this->radius << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const Capsule& Capsule) {
		stream << Capsule.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Capsule& Capsule) {
		stream << Capsule.str();
		return stream;
	}
}
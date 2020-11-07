#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	struct Segment{
	public:
		Segment() {
			start = Guarneri::Vector3::ZERO;
			end = Guarneri::Vector3::ZERO;
		}

		Segment(const Guarneri::Vector3& center, const Guarneri::Vector3& dir, const float& extent) {
			this->start = center - extent * dir;
			this->end = center + extent * dir;
		}

		Segment(const Guarneri::Vector3& start, const Guarneri::Vector3& end) {
			this->start = start;
			this->end = end;
		}

		Segment(const Segment& other) {
			this->start = other.start;
			this->end = other.end;
		}

	public:
		Guarneri::Vector3 start;
		Guarneri::Vector3 end;

	public:
		Guarneri::Vector3 center() const {
			return (this->start + this->end) / 2;
		}

		float extent() const {
			return Vector3::length(end, start) / 2;
		}

		Guarneri::Vector3 dir() const {
			return Vector3::normalize(end - start);
		}

		std::string str() const {
			std::stringstream ss;
			ss << "Segment: [start: " << this->start  << ", end: "<<this->end << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const Segment& seg) {
		stream << seg.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Segment& seg) {
		stream << seg.str();
		return stream;
	}
}
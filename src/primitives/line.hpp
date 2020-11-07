#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	struct Line {
	public:
		Line() {
			origin = Guarneri::Vector3();
			direction = Guarneri::Vector3();
		}

		Line(const Guarneri::Vector3& origin_t, const Guarneri::Vector3& direction_t) {
			this->origin = origin_t;
			this->direction = direction_t;
		}

		Line(const Line& other) {
			this->origin = other.origin;
			this->direction = other.direction;
		}

	public:
		Guarneri::Vector3 origin;
		Guarneri::Vector3 direction;

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "Line: [origin: " << this->origin << ", direction: " << this->direction << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const Line& Line) {
		stream << Line.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Line& Line) {
		stream << Line.str();
		return stream;
	}
}
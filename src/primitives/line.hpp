#pragma once
#include <guarneri.hpp>

namespace guarneri {
	struct Line {
	public:
		Line() {
			origin = guarneri::Vector3();
			direction = guarneri::Vector3();
		}

		Line(const guarneri::Vector3& origin_t, const guarneri::Vector3& direction_t) {
			this->origin = origin_t;
			this->direction = direction_t;
		}

		Line(const Line& other) {
			this->origin = other.origin;
			this->direction = other.direction;
		}

	public:
		guarneri::Vector3 origin;
		guarneri::Vector3 direction;

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
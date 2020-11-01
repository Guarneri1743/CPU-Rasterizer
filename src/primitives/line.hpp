#pragma once
#include <guarneri.hpp>

namespace guarneri {
	struct line {
	public:
		line() {
			origin = guarneri::float3();
			direction = guarneri::float3();
		}

		line(const guarneri::float3& origin_t, const guarneri::float3& direction_t) {
			this->origin = origin_t;
			this->direction = direction_t;
		}

		line(const line& other) {
			this->origin = other.origin;
			this->direction = other.direction;
		}

	public:
		guarneri::float3 origin;
		guarneri::float3 direction;

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "line: [origin: " << this->origin << ", direction: " << this->direction << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const line& line) {
		stream << line.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const line& line) {
		stream << line.str();
		return stream;
	}
}
#pragma once
#include <line.hpp>

namespace guarneri {
	struct cylinder {
	public:
		cylinder() {
			axis = line();
			radius = 0;
			height = 0;
		}

		cylinder(const line& axis, const float& radius, const float& height) {
			this->axis = axis;
			this->radius = radius;
			this->height = height;
		}

		cylinder(const cylinder& other) {
			this->axis = other.axis;
			this->radius = other.radius;
			this->height = other.height;
		}

	public:
		line axis;
		float radius;
		float height;

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "cylinder: [axis: " << this->axis << ", radius: " << std::setprecision(FLOAT_LOG_PRECISION) << this->radius << ", height" << std::setprecision(FLOAT_LOG_PRECISION) << this->height << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const cylinder& cylinder) {
		stream << cylinder.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const cylinder& cylinder) {
		stream << cylinder.str();
		return stream;
	}
}
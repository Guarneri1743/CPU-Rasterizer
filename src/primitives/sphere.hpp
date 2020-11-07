#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	struct Sphere {
	public:
		Sphere() {
			center = Guarneri::Vector3();
			radius = 0;
		}

		Sphere(const Guarneri::Vector3& center_t, const float& radius_t) {
			this->center = center_t;
			this->radius = radius_t;
		}

		Sphere(const Sphere& other) {
			this->center = other.center;
			this->radius = other.radius;
		}

	public:
		Guarneri::Vector3 center;
		float radius;	
	
	public:
			std::string str() const {
				std::stringstream ss;
				ss << "Sphere: [origin: " << this->center << ", radius: " << std::setprecision(FLOAT_LOG_PRECISION) << this->radius << "]";
				return ss.str();
			}
	};

	static std::ostream& operator << (std::ostream& stream, const Sphere& Sphere) {
		stream << Sphere.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Sphere& Sphere) {
		stream << Sphere.str();
		return stream;
	}
}
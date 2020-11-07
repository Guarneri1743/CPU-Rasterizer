#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	struct Plane {
	public:
		Plane() {
			normal = Guarneri::Vector3();
			constant = 0;
		}

		Plane(const Guarneri::Vector3& normal, const float& constant) {
			this->normal = normal;
			this->constant = constant;
		}

		Plane(const Guarneri::Vector3& normal, const Guarneri::Vector3& p) {
			this->normal = normal;
			this->constant = Vector3::dot(normal, p);
		}

		Plane(const Plane& other) {
			this->normal = other.normal;
			this->constant = other.constant;
		}

	public:
		Guarneri::Vector3 normal;
		float constant;

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "Plane: [normal: " << this->normal << ", constant: " << this->constant << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const Plane& Plane) {
		stream << Plane.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Plane& Plane) {
		stream << Plane.str();
		return stream;
	}
}
#pragma once
#include <guarneri.hpp>

namespace guarneri {
	struct plane {
	public:
		plane() {
			normal = guarneri::float3();
			constant = 0;
		}

		plane(const guarneri::float3& normal_t, const float& constant_t) {
			this->normal = normal_t;
			this->constant = constant_t;
		}

		plane(const guarneri::float3& normal, const guarneri::float3& p) {
			this->normal = normal;
			this->constant = float3::dot(normal, p);
		}

		plane(const plane& other) {
			this->normal = other.normal;
			this->constant = other.constant;
		}

	public:
		guarneri::float3 normal;
		float constant;

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "plane: [normal: " << this->normal << ", constant: " << this->constant << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const plane& plane) {
		stream << plane.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const plane& plane) {
		stream << plane.str();
		return stream;
	}
}
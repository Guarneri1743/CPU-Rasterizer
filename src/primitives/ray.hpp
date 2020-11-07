#pragma once
#include <guarneri.hpp>

namespace guarneri {
	struct Ray {
	public:
		Ray() {
			origin = guarneri::Vector3();
			direction = guarneri::Vector3();
			inversed_direction = guarneri::Vector3();
		}

		Ray(const guarneri::Vector3& origin, const guarneri::Vector3& direction){
			this->origin = origin;
			this->direction = direction;
			this->inversed_direction = guarneri::Vector3(1 / direction.x, 1 / direction.y, 1 / direction.z);
		}

		Ray(const Ray& other) {
			this->origin = other.origin;
			this->direction = other.direction;
			this->inversed_direction = other.inversed_direction;
		}

	public:
		guarneri::Vector3 origin;
		guarneri::Vector3 direction;
		guarneri::Vector3 inversed_direction;

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "Ray: [origin: " << this->origin << ", direction: " << this->direction << ", inversed_direction: " << inversed_direction << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const Ray& Ray) {
		stream << Ray.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Ray& Ray) {
		stream << Ray.str();
		return stream;
	}
}
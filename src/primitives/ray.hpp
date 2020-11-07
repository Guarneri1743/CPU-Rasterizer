#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	struct Ray {
	public:
		Ray() {
			origin = Guarneri::Vector3();
			direction = Guarneri::Vector3();
			inversed_direction = Guarneri::Vector3();
		}

		Ray(const Guarneri::Vector3& origin, const Guarneri::Vector3& direction){
			this->origin = origin;
			this->direction = direction;
			this->inversed_direction = Guarneri::Vector3(1 / direction.x, 1 / direction.y, 1 / direction.z);
		}

		Ray(const Ray& other) {
			this->origin = other.origin;
			this->direction = other.direction;
			this->inversed_direction = other.inversed_direction;
		}

	public:
		Guarneri::Vector3 origin;
		Guarneri::Vector3 direction;
		Guarneri::Vector3 inversed_direction;

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
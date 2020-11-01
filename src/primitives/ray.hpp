#pragma once
#include <guarneri.hpp>

namespace guarneri {
	struct ray {
	public:
		ray() {
			origin = guarneri::float3();
			direction = guarneri::float3();
			inversed_direction = guarneri::float3();
		}

		ray(const guarneri::float3& origin_t, const guarneri::float3& direction_t){
			this->origin = origin_t;
			this->direction = direction_t;
			this->inversed_direction = guarneri::float3(1 / direction.x, 1 / direction.y, 1 / direction.z);
		}

		ray(const ray& other) {
			this->origin = other.origin;
			this->direction = other.direction;
			this->inversed_direction = other.inversed_direction;
		}

	public:
		guarneri::float3 origin;
		guarneri::float3 direction;
		guarneri::float3 inversed_direction;

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "ray: [origin: " << this->origin << ", direction: " << this->direction << ", inversed_direction: " << inversed_direction << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const ray& ray) {
		stream << ray.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const ray& ray) {
		stream << ray.str();
		return stream;
	}
}
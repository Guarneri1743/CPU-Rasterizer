#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	struct Ray2D {
	public:
		Ray2D() {
			origin = Guarneri::Vector2();
			direction = Guarneri::Vector2();
			inversed_direction = Guarneri::Vector2();
		}

		Ray2D(const Guarneri::Vector2& origin, const Guarneri::Vector2& direction) {
			this->origin = origin;
			this->direction = direction;
			this->inversed_direction = Guarneri::Vector2(1 / direction.x, 1 / direction.y);
		}

		Ray2D(const Ray2D& other) {
			this->origin = other.origin;
			this->direction = other.direction;
			this->inversed_direction = other.inversed_direction;
		}

	public:
		Guarneri::Vector2 origin;
		Guarneri::Vector2 direction;
		Guarneri::Vector2 inversed_direction;

	public:
		std::string str() const {
			std::stringstream ss;
			ss << "Ray2D: [origin: " << this->origin << ", direction: " << this->direction << ", inversed_direction: " << inversed_direction << "]";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const Ray2D& Ray) {
		stream << Ray.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Ray2D& Ray) {
		stream << Ray.str();
		return stream;
	}
}
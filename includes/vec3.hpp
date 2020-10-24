#pragma once
#include "common.hpp"
#include <string>
#include <sstream>
#include <iomanip>

namespace guarneri {
	struct vec3 {
	public:
		vec3() { x = 0; y = 0; z = 0; }
		vec3(const float& r) { this->x = r; this->y = r; this->z = r; }
		vec3(const vec3& v) { this->x = v.x; this->y = v.y; this->z = v.z; }
		vec3(const float& x, const float& y, const float& z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}

	public:
		float x;
		float y;
		float z;

	public:
		static const vec3 ZERO;
		static const vec3 ONE;
		static const vec3 UP;
		static const vec3 DOWN;
		static const vec3 LEFT;
		static const vec3 RIGHT;
		static const vec3 FORWARD;
		static const vec3 BACK;

	public:
		float& operator[](const unsigned int i) { return (&x)[i]; }
		const float& operator[](const unsigned int i) const { return (&x)[i]; }


		bool operator == (const vec3& other) {
			return EQUALS(x, other.x) && EQUALS(y, other.y) && EQUALS(z, other.z);
		}

		bool operator != (const vec3& other) {
			return !(*this == other);
		}

		vec3& operator +=(const vec3& other) {
			x = x + other.x;
			y = y + other.y;
			z = z + other.z;
			return *this;
		}

		vec3& operator -=(const vec3& other) {
			x = x - other.x;
			y = y - other.y;
			z = z - other.z;
			return *this;
		}

		vec3& operator /=(const float& val) {
			x = x / val;
			y = y / val;
			z = z / val;
			return *this;
		}

		vec3& operator *=(const float& val) {
			x = x * val;
			y = y * val;
			z = z * val;
			return *this;
		}

		vec3 operator +(const vec3& other) const {
			vec3 ret;
			ret.x = this->x + other.x;
			ret.y = this->y + other.y;
			ret.z = this->z + other.z;
			return ret;
		}

		vec3 operator -(const vec3& other) const {
			vec3 ret;
			ret.x = this->x - other.x;
			ret.y = this->y - other.y;
			ret.z = this->z - other.z;
			return ret;
		}

		vec3 operator *(const float& scale) const {
			vec3 ret;
			ret.x = this->x * scale;
			ret.y = this->y * scale;
			ret.z = this->z * scale;
			return ret;
		}

		vec3 operator /(const float& val) const {
			vec3 ret;
			ret.x = this->x / val;
			ret.y = this->y / val;
			ret.z = this->z / val;
			return ret;
		}

		vec3 operator /(const vec3& val) const {
			vec3 ret;
			ret.x = this->x / val.x;
			ret.y = this->y / val.y;
			ret.z = this->z / val.z;
			return ret;
		}

		vec3 operator -() const {
			return vec3(-x, -y, -z);
		}

		vec3 operator *(const vec3& b) const {
			return vec3(x * b.x, y * b.y, z * b.z);
		}

		static float dot(const vec3& lhs, const vec3& rhs) {
			return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
		}

		static vec3 cross(const vec3& lhs, const vec3& rhs)
		{
			return vec3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
		}

		static float magnitude(const vec3& vector) {
			return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
		}

		static float length(const vec3& a, const vec3& b) {
			return magnitude(a - b);
		}

		static vec3 max(const vec3& a, const vec3& b) {
			return vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
		}

		static vec3 min(const vec3& a, const vec3& b) {
			return vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
		}

		static vec3 abs(const vec3& v) {
			return vec3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
		}

		static vec3 normalize(const vec3& value) {
			float num = magnitude(value);
			vec3 retult;
			if (num > 1e-05f) {
				retult = value / num;
			}
			else {
				retult = vec3();
			}
			return retult;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "(" << std::setprecision(6) << this->x << ", " << std::setprecision(6) << this->y << ", " << std::setprecision(6) << this->z << ")";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const vec3& vec) {
		stream << vec.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const vec3& vec) {
		stream << vec.str();
		return stream;
	}

	const vec3 vec3::ZERO = vec3(0.0f, 0.0f, 0.0f);
	const vec3 vec3::ONE = vec3(1.0f, 1.0f, 1.0f);
	const vec3 vec3::UP = vec3(0.f, 1.0f, 0.f);
	const vec3 vec3::DOWN = vec3(0.f, -1.0f, 0.f);
	const vec3 vec3::LEFT = vec3(-1.0f, 0.f, 0.f);
	const vec3 vec3::RIGHT = vec3(1.0f, 0.f, 0.f);
	const vec3 vec3::FORWARD = vec3(0.f, 0.f, 1.0f);
	const vec3 vec3::BACK = vec3(0.f, 0.f, -1.0f);
}
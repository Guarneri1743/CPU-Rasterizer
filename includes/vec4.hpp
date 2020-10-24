#pragma once
#include "common.hpp"
#include <string>
#include <sstream>
#include <iomanip>

namespace guarneri {
	struct vec4 {
	public:
		vec4() { x = 0; y = 0; z = 0; w = 0; }
		vec4(const float& r) { this->x = r; this->y = r; this->z = r; this->w = r; }
		vec4(const vec4& v) { this->x = v.x; this->y = v.y; this->z = v.z; this->w = v.w; }
		vec4(const float& x, const float& y, const float& z, const float& w) {
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

	public:
		float x;
		float y;
		float z;
		float w;

	public:
		static const vec4 ZERO;
		static const vec4 ONE;

	public:
		float& operator[](const unsigned int& i) { return (&x)[i]; }

		const float& operator[](const unsigned int& i) const { return (&x)[i]; }

		bool operator == (const vec4& other) {
			return EQUALS(x, other.x) && EQUALS(y, other.y) && EQUALS(z, other.z) && EQUALS(w, other.w);
		}

		bool operator != (const vec4& other) {
			return !(*this == other);
		}

		vec4& operator +=(const vec4& other) {
			x = x + other.x;
			y = y + other.y;
			z = z + other.z;
			w = w + other.w;
			return *this;
		}

		vec4& operator -=(const vec4& other) {
			x = x - other.x;
			y = y - other.y;
			z = z - other.z;
			w = z - other.w;
			return *this;
		}

		vec4& operator /=(const float& val) {
			x = x / val;
			y = y / val;
			z = z / val;
			w = w / val;
			return *this;
		}

		vec4& operator *=(const float& val) {
			x = x * val;
			y = y * val;
			z = z * val;
			w = w * val;
			return *this;
		}

		vec4 operator +(const vec4& other) const {
			vec4 ret;
			ret.x = this->x + other.x;
			ret.y = this->y + other.y;
			ret.z = this->z + other.z;
			ret.w = this->w + other.w;
			return ret;
		}

		vec4 operator -(const vec4& other) const {
			vec4 ret;
			ret.x = this->x - other.x;
			ret.y = this->y - other.y;
			ret.z = this->z - other.z;
			ret.w = this->w - other.w;
			return ret;
		}

		vec4 operator *(const float& scale) const {
			vec4 ret;
			ret.x = this->x * scale;
			ret.y = this->y * scale;
			ret.z = this->z * scale;
			ret.w = this->w * scale;
			return ret;
		}

		vec4 operator /(const float& val) const {
			vec4 ret;
			ret.x = this->x / val;
			ret.y = this->y / val;
			ret.z = this->z / val;
			ret.w = this->w / val;
			return ret;
		}

		vec4 operator /(const vec4& val) const {
			vec4 ret;
			ret.x = this->x / val.x;
			ret.y = this->y / val.y;
			ret.z = this->z / val.z;
			ret.w = this->w / val.w;
			return ret;
		}

		vec4 operator -() const {
			return vec4(-x, -y, -z, -w);
		}

		vec4 operator *(const vec4& b) const {
			return vec4(x * b.x, y * b.y, z * b.z, w * b.w);
		}

		static float dot(const vec4& lhs, const vec4& rhs) {
			return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * lhs.w;
		}

		static float magnitude(const vec4& vector) {
			return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w);
		}

		static float length(const vec4& a, const vec4& b) {
			return magnitude(a - b);
		}

		static vec4 max(const vec4& a, const vec4& b) {
			return vec4(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w));
		}

		static vec4 min(const vec4& a, const vec4& b) {
			return vec4(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w));
		}

		static vec4 abs(const vec4& v) {
			return vec4(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
		}

		static vec4 normalize(const vec4& value) {
			float num = magnitude(value);
			vec4 retult;
			if (num > 1e-05f) {
				retult = value / num;
			}
			else {
				retult = vec4();
			}
			return retult;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "(" << std::setprecision(6) << this->x << ", " << std::setprecision(6) << this->y << ", " << std::setprecision(6) << this->z << ", " << this->w << ")";
			return ss.str();
		}
	};

	static std::ostream& operator << (std::ostream& stream, const vec4& vec) {
		stream << vec.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const vec4& vec) {
		stream << vec.str();
		return stream;
	}

	const vec4 vec4::ZERO = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	const vec4 vec4::ONE = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
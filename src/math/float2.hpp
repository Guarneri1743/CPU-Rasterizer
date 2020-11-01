#pragma once
#include <guarneri.hpp>

namespace guarneri {
	static float2 operator *(const float& val, const float2& vec);

	struct float2 {
	public:
		static const float2 ZERO;
		static const float2 ONE;

	public:
		float2() { x = 0; y = 0; }
		float2(const float& r) { this->x = r; this->y = r; }
		float2(const float2& v) { this->x = v.x; this->y = v.y; }
		float2(const float& x, const float& y) { this->x = x; this->y = y; }

	public:
		float x;
		float y;

	public:
		float& operator[](const uint32_t i) { return (&x)[i]; }

		const float& operator[](const uint32_t i) const { return (&x)[i]; }

		bool operator == (const float2& other) {
			return EQUALS(x, other.x) && EQUALS(y, other.y);
		}

		bool operator != (const float2& other) {
			return !(*this == other);
		}

		void operator =(const float2& other) {
			this->x = other.x; this->y = other.y;
		}

		float2& operator +=(const float2& other) {
			x = x + other.x;
			y = y + other.y;
			return *this;
		}

		float2& operator -=(const float2& other) {
			x = x - other.x;
			y = y - other.y;
			return *this;
		}

		float2& operator /=(const float& val) {
			x = x / val;
			y = y / val;
			return *this;
		}

		float2& operator *=(const float& val) {
			x = x * val;
			y = y * val;
			return *this;
		}

		float2 operator +(const float2& other) const {
			float2 ret;
			ret.x = this->x + other.x;
			ret.y = this->y + other.y;
			return ret;
		}

		float2 operator -(const float2& other) const {
			float2 ret;
			ret.x = this->x - other.x;
			ret.y = this->y - other.y;
			return ret;
		}

		float2 operator *(const float& scale) const {
			float2 ret;
			ret.x = this->x * scale;
			ret.y = this->y * scale;
			return ret;
		}

		float2 operator /(const float& val) const {
			float2 ret;
			ret.x = this->x / val;
			ret.y = this->y / val;
			return ret;
		}

		float2 operator /(const float2& val) const {
			float2 ret;
			ret.x = this->x / val.x;
			ret.y = this->y / val.y;
			return ret;
		}

		float2 operator -() const {
			return float2(-x, -y);
		}

		float2 operator *(const float2& b) const {
			return float2(x * b.x, y * b.y);
		}

		static float dot(const float2& lhs, const float2& rhs) {
			return lhs.x * rhs.x + lhs.y * rhs.y;
		}

		static float magnitude(const float2& vector) {
			return sqrt(vector.x * vector.x + vector.y * vector.y);
		}

		static float length(const float2& a, const float2& b) {
			return magnitude(a - b);
		}

		static float2 max(const float2& a, const float2& b) {
			return float2(std::max(a.x, b.x), std::max(a.y, b.y));
		}

		static float2 min(const float2& a, const float2& b) {
			return float2(std::min(a.x, b.x), std::min(a.y, b.y));
		}

		static float2 abs(const float2& v) {
			return float2(std::abs(v.x), std::abs(v.y));
		}

		static float2 normalize(const float2& value) {
			float num = magnitude(value);
			float2 retult;
			if (num > EPSILON) {
				retult = value / num;
			}
			else {
				retult = float2();
			}
			return retult;
		}

		static float2 lerp(const float2& lhs, const float2& rhs, float t) {
			float x = lhs.x + (rhs.x - lhs.x) * t;
			float y = lhs.y + (rhs.y - lhs.y) * t;
			return float2(x, y);
		}

		std::string str() const {
			std::stringstream ss;
			ss << "(" << std::setprecision(FLOAT_LOG_PRECISION) << this->x << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->y << ")";
			return ss.str();
		}

		public:
			float2 xx() const { return float2(x, x); }
			float2 yy() const { return float2(y, y); }
			float2 yx() const { return float2(y, x); }
			float2 xy() const { return float2(x, y); }
	};

	static float2 operator *(const float& val, const float2& vec) {
		return vec * val;
	}

	static std::ostream& operator << (std::ostream& stream, const float2& vec) {
		stream << vec.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const float2& vec) {
		stream << vec.str();
		return stream;
	}

	const float2 float2::ZERO = float2(0.0f, 0.0f);
	const float2 float2::ONE = float2(1.0f, 1.0f);
}
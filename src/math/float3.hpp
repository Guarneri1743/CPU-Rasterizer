#pragma once
#include <guarneri.hpp>

namespace guarneri {
	static float3 operator +(const float& other, const float3& vec);
	static float3 operator -(const float& other, const float3& vec);
	static float3 operator *(const float& other, const float3& vec);

	struct float3 {
	public:
		static const float3 ZERO;
		static const float3 ONE;
		static const float3 UP;
		static const float3 DOWN;
		static const float3 LEFT;
		static const float3 RIGHT;
		static const float3 FORWARD;
		static const float3 BACK;

	public:
		float3() { x = 0; y = 0; z = 0; }
		float3(const float& r) { this->x = r; this->y = r; this->z = r; }
		float3(const float3& v) { this->x = v.x; this->y = v.y; this->z = v.z; }
		float3(const float& x, const float& y, const float& z) { this->x = x; this->y = y; this->z = z; }

	public:
		float x;
		float y;
		float z;
		
	public:
		float3 normalized() {
			return float3::normalize(*this);
		}

		float& operator[](const unsigned int i) { return (&x)[i]; }

		const float& operator[](const unsigned int i) const { return (&x)[i]; }

		bool operator == (const float3& other) {
			return EQUALS(x, other.x) && EQUALS(y, other.y) && EQUALS(z, other.z);
		}

		bool operator != (const float3& other) {
			return !(*this == other);
		}

		void operator =(const float3& other) {
			this->x = other.x; this->y = other.y; this->x = other.x;
		}

		float3& operator +=(const float3& other) {
			x = x + other.x;
			y = y + other.y;
			z = z + other.z;
			return *this;
		}

		float3& operator -=(const float3& other) {
			x = x - other.x;
			y = y - other.y;
			z = z - other.z;
			return *this;
		}

		float3& operator /=(const float& val) {
			x = x / val;
			y = y / val;
			z = z / val;
			return *this;
		}

		float3& operator *=(const float& val) {
			x = x * val;
			y = y * val;
			z = z * val;
			return *this;
		}

		float3 operator +(const float3& other) const {
			float3 ret;
			ret.x = this->x + other.x;
			ret.y = this->y + other.y;
			ret.z = this->z + other.z;
			return ret;
		}

		float3 operator -(const float3& other) const {
			float3 ret;
			ret.x = this->x - other.x;
			ret.y = this->y - other.y;
			ret.z = this->z - other.z;
			return ret;
		}

		float3 operator +(const float& other) const {
			float3 ret;
			ret.x = this->x + other;
			ret.y = this->y + other;
			ret.z = this->z + other;
			return ret;
		}

		float3 operator -(const float& other) const {
			float3 ret;
			ret.x = this->x - other;
			ret.y = this->y - other;
			ret.z = this->z - other;
			return ret;
		}

		float3 operator -() const {
			return float3(-x, -y, -z);
		}

		float3 operator *(const float& scale) const {
			float3 ret;
			ret.x = this->x * scale;
			ret.y = this->y * scale;
			ret.z = this->z * scale;
			return ret;
		}

		float3 operator *(const float3& b) const {
			return float3(x * b.x, y * b.y, z * b.z);
		}

		float3 operator /(const float& val) const {
			float3 ret;
			ret.x = this->x / val;
			ret.y = this->y / val;
			ret.z = this->z / val;
			return ret;
		}

		float3 operator /(const float3& val) const {
			float3 ret;
			ret.x = this->x / val.x;
			ret.y = this->y / val.y;
			ret.z = this->z / val.z;
			return ret;
		}

		bool operator <=(const float3& other) const {
			return float3::magnitude(*this) <= float3::magnitude(other);
		}

		bool operator >=(const float3& other) const {
			return float3::magnitude(*this) >= float3::magnitude(other);
		}

		bool operator >(const float3& other) const {
			return float3::magnitude(*this) > float3::magnitude(other);
		}

		bool operator <(const float3& other) const {
			return float3::magnitude(*this) < float3::magnitude(other);
		}

		static float dot(const float3& lhs, const float3& rhs) {
			return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
		}

		static float3 cross(const float3& lhs, const float3& rhs)
		{
			return float3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
		}

		static float magnitude(const float3& vector) {
			return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
		}

		static float length(const float3& a, const float3& b) {
			return magnitude(a - b);
		}

		static float3 max(const float3& a, const float3& b) {
			return float3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
		}

		static float3 min(const float3& a, const float3& b) {
			return float3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
		}

		static float3 abs(const float3& v) {
			return float3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
		}

		static float3 normalize(const float3& value) {
			float num = magnitude(value);
			float3 retult;
			if (num > EPSILON) {
				retult = value / num;
			}
			else {
				retult = float3();
			}
			return retult;
		}

		static float3 floor(const float3& val) {
			return float3(std::floor(val.x), std::floor(val.y), std::floor(val.z));
		}

		static float3 lerp(const float3& lhs, const float3& rhs, float t) {
			float x = lhs.x + (rhs.x - lhs.x) * t;
			float y = lhs.y + (rhs.y - lhs.y) * t;
			float z = lhs.z + (rhs.z - lhs.z) * t;
			return float3(x, y, z);
		}

		static float3 maxf(const float3& lhs, const float3& rhs) {
			return float3(std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y), std::max(lhs.z, rhs.z));
		}

		std::string str() const {
			std::stringstream ss;
			ss << "(" << std::setprecision(FLOAT_LOG_PRECISION) << this->x << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->y << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->z << ")";
			return ss.str();
		}

		public:
			float3 xxx() const { return float3(x, x, x); }
			float3 xxy() const { return float3(x, x, y); }
			float3 xxz() const { return float3(x, x, z); }
			float3 xyx() const { return float3(x, y, x); }
			float3 xyy() const { return float3(x, y, y); }
			float3 xyz() const { return float3(x, y, z); }
			float3 xzx() const { return float3(x, z, x); }
			float3 xzy() const { return float3(x, z, y); }
			float3 xzz() const { return float3(x, z, z); }

			float3 yxx() const { return float3(y, x, x); }
			float3 yxy() const { return float3(y, x, y); }
			float3 yxz() const { return float3(y, x, z); }
			float3 yyx() const { return float3(y, y, x); }
			float3 yyy() const { return float3(y, y, y); }
			float3 yyz() const { return float3(y, y, z); }
			float3 yzx() const { return float3(y, z, x); }
			float3 yzy() const { return float3(y, z, y); }
			float3 yzz() const { return float3(y, z, z); }

			float3 zxx() const { return float3(z, x, x); }
			float3 zxy() const { return float3(z, x, y); }
			float3 zxz() const { return float3(z, x, z); }
			float3 zyx() const { return float3(z, y, x); }
			float3 zyy() const { return float3(z, y, y); }
			float3 zyz() const { return float3(z, y, z); }
			float3 zzx() const { return float3(z, z, x); }
			float3 zzy() const { return float3(z, z, y); }
			float3 zzz() const { return float3(z, z, z); }

			float2 xx() const { return float2(x, x); }
			float2 xy() const { return float2(x, y); }
			float2 xz() const { return float2(x, z); }

			float2 yx() const { return float2(y, x); }
			float2 yy() const { return float2(y, y); }
			float2 yz() const { return float2(y, z); }

			float2 zz() const { return float2(z, z); }
			float2 zx() const { return float2(z, x); }
			float2 zy() const { return float2(z, y); }
	};

	static float3 operator +(const float& other, const float3& vec) {
		return vec + other;
	}

	static float3 operator -(const float& other, const float3& vec) {
		return -vec + other;
	}

	static float3 operator *(const float& val, const float3& vec) {
		return vec * val;
	}

	static std::ostream& operator << (std::ostream& stream, const float3& vec) {
		stream << vec.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const float3& vec) {
		stream << vec.str();
		return stream;
	}

	const float3 float3::ZERO = float3(0.0f, 0.0f, 0.0f);
	const float3 float3::ONE = float3(1.0f, 1.0f, 1.0f);
	const float3 float3::UP = float3(0.f, 1.0f, 0.f);
	const float3 float3::DOWN = float3(0.f, -1.0f, 0.f);
	const float3 float3::LEFT = float3(-1.0f, 0.f, 0.f);
	const float3 float3::RIGHT = float3(1.0f, 0.f, 0.f);
	const float3 float3::FORWARD = float3(0.f, 0.f, 1.0f);
	const float3 float3::BACK = float3(0.f, 0.f, -1.0f);
}
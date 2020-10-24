#pragma once
#include <common.hpp>
#include <float3.hpp>
#include <float2.hpp>

namespace guarneri {
	struct float4 {
	public:
		static const float4 ZERO;
		static const float4 ONE;

	public:
		float4() { x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f; }
		float4(const float& r) { this->x = r; this->y = r; this->z = r; this->w = r; }
		float4(const float4& v) { this->x = v.x; this->y = v.y; this->z = v.z; this->w = v.w; }
		float4(const float3& v) { this->x = v.x; this->y = v.y; this->z = v.z; this->w = 1.0f; }
		float4(const float& x, const float3& yzw) { this->x = x; this->y = yzw.x; this->z = yzw.y; this->w = yzw.z; }
		float4(const float& x, const float& y, const float2& zw) { this->x = x; this->y = y; this->z = zw.x; this->w = zw.y; }
		float4(const float2& v, const float& z, const float& w) { this->x = v.x; this->y = v.y; this->z = z; this->w = w; }
		float4(const float2& v, const float2& zw) { this->x = v.x; this->y = v.y; this->z = zw.x; this->w = zw.y; }
		float4(const float3& v, const float& w) { this->x = v.x; this->y = v.y; this->z = v.z; this->w = w; }
		float4(const float& x, const float& y, const float& z) { this->x = x; this->y = y; this->z = z; this->w = 1.0f; }
		float4(const float& x, const float& y, const float& z, const float& w) { this->x = x; this->y = y; this->z = z; this->w = w; }

	public:
		float x;
		float y;
		float z;
		float w;

	public:
		float4 normalized() {
			return float4::normalize(*this);
		}

		float& operator[](const unsigned int& i) { return (&x)[i]; }

		const float& operator[](const unsigned int& i) const { return (&x)[i]; }

		bool operator == (const float4& other) {
			return EQUALS(x, other.x) && EQUALS(y, other.y) && EQUALS(z, other.z) && EQUALS(w, other.w);
		}

		bool operator != (const float4& other) {
			return !(*this == other);
		}

		float4& operator +=(const float4& other) {
			x = x + other.x;
			y = y + other.y;
			z = z + other.z;
			w = w + other.w;
			return *this;
		}

		float4& operator -=(const float4& other) {
			x = x - other.x;
			y = y - other.y;
			z = z - other.z;
			w = z - other.w;
			return *this;
		}

		float4& operator /=(const float& val) {
			x = x / val;
			y = y / val;
			z = z / val;
			w = w / val;
			return *this;
		}

		float4& operator *=(const float& val) {
			x = x * val;
			y = y * val;
			z = z * val;
			w = w * val;
			return *this;
		}

		float4 operator +(const float4& other) const {
			float4 ret;
			ret.x = this->x + other.x;
			ret.y = this->y + other.y;
			ret.z = this->z + other.z;
			ret.w = this->w + other.w;
			return ret;
		}

		float4 operator -(const float4& other) const {
			float4 ret;
			ret.x = this->x - other.x;
			ret.y = this->y - other.y;
			ret.z = this->z - other.z;
			ret.w = this->w - other.w;
			return ret;
		}

		float4 operator *(const float& scale) const {
			float4 ret;
			ret.x = this->x * scale;
			ret.y = this->y * scale;
			ret.z = this->z * scale;
			ret.w = this->w * scale;
			return ret;
		}

		float4 operator /(const float& val) const {
			float4 ret;
			ret.x = this->x / val;
			ret.y = this->y / val;
			ret.z = this->z / val;
			ret.w = this->w / val;
			return ret;
		}

		float4 operator /(const float4& val) const {
			float4 ret;
			ret.x = this->x / val.x;
			ret.y = this->y / val.y;
			ret.z = this->z / val.z;
			ret.w = this->w / val.w;
			return ret;
		}

		float4 operator -() const {
			return float4(-x, -y, -z, -w);
		}

		float4 operator *(const float4& b) const {
			return float4(x * b.x, y * b.y, z * b.z, w * b.w);
		}

		static float dot(const float4& lhs, const float4& rhs) {
			return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * lhs.w;
		}

		static float magnitude(const float4& vector) {
			return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w);
		}

		static float length(const float4& a, const float4& b) {
			return magnitude(a - b);
		}

		static float4 max(const float4& a, const float4& b) {
			return float4(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w));
		}

		static float4 min(const float4& a, const float4& b) {
			return float4(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w));
		}

		static float4 abs(const float4& v) {
			return float4(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
		}

		static float4 normalize(const float4& value) {
			float num = magnitude(value);
			float4 retult;
			if (num > EPSILON) {
				retult = value / num;
			}
			else {
				retult = float4();
			}
			return retult;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "(" << std::setprecision(FLOAT_LOG_PRECISION) << this->x << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->y << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->z << ", " << this->w << ")";
			return ss.str();
		}

		public:
			float4 xxxx() { return float4(x, x, x, x); }
			float4 xxxy() { return float4(x, x, x, y); }
			float4 xxxz() { return float4(x, x, x, z); }
			float4 xxxw() { return float4(x, x, x, w); }
			float4 xxyx() { return float4(x, x, y, x); }
			float4 xxyy() { return float4(x, x, y, y); }
			float4 xxyz() { return float4(x, x, y, z); }
			float4 xxyw() { return float4(x, x, y, w); }
			float4 xxzx() { return float4(x, x, z, x); }
			float4 xxzy() { return float4(x, x, z, y); }
			float4 xxzz() { return float4(x, x, z, z); }
			float4 xxzw() { return float4(x, x, z, w); }

			float4 xyxx() { return float4(x, y, x, x); }
			float4 xyxy() { return float4(x, y, x, y); }
			float4 xyxz() { return float4(x, y, x, z); }
			float4 xyxw() { return float4(x, y, x, w); }
			float4 xyyx() { return float4(x, y, y, x); }
			float4 xyyy() { return float4(x, y, y, y); }
			float4 xyyz() { return float4(x, y, y, z); }
			float4 xyyw() { return float4(x, y, y, w); }
			float4 xyzx() { return float4(x, y, z, x); }
			float4 xyzy() { return float4(x, y, z, y); }
			float4 xyzz() { return float4(x, y, z, z); }
			float4 xyzw() { return float4(x, y, z, w); }

			float4 xzxx() { return float4(x, z, x, x); }
			float4 xzxy() { return float4(x, z, x, y); }
			float4 xzxz() { return float4(x, z, x, z); }
			float4 xzxw() { return float4(x, z, x, w); }
			float4 xzyx() { return float4(x, z, y, x); }
			float4 xzyy() { return float4(x, z, y, y); }
			float4 xzyz() { return float4(x, z, y, z); }
			float4 xzyw() { return float4(x, z, y, w); }
			float4 xzzx() { return float4(x, z, z, x); }
			float4 xzzy() { return float4(x, z, z, y); }
			float4 xzzz() { return float4(x, z, z, z); }
			float4 xzzw() { return float4(x, z, z, w); }

			float4 xwxx() { return float4(x, w, x, x); }
			float4 xwxy() { return float4(x, w, x, y); }
			float4 xwxz() { return float4(x, w, x, z); }
			float4 xwxw() { return float4(x, w, x, w); }
			float4 xwyx() { return float4(x, w, y, x); }
			float4 xwyy() { return float4(x, w, y, y); }
			float4 xwyz() { return float4(x, w, y, z); }
			float4 xwyw() { return float4(x, w, y, w); }
			float4 xwzx() { return float4(x, w, z, x); }
			float4 xwzy() { return float4(x, w, z, y); }
			float4 xwzz() { return float4(x, w, z, z); }
			float4 xwzw() { return float4(x, w, z, w); }

			float4 yxxx() { return float4(y, x, x, x); }
			float4 yxxy() { return float4(y, x, x, y); }
			float4 yxxz() { return float4(y, x, x, z); }
			float4 yxxw() { return float4(y, x, x, w); }
			float4 yxyx() { return float4(y, x, y, x); }
			float4 yxyy() { return float4(y, x, y, y); }
			float4 yxyz() { return float4(y, x, y, z); }
			float4 yxyw() { return float4(y, x, y, w); }
			float4 yxzx() { return float4(y, x, z, x); }
			float4 yxzy() { return float4(y, x, z, y); }
			float4 yxzz() { return float4(y, x, z, z); }
			float4 yxzw() { return float4(y, x, z, w); }

			float4 yyxx() { return float4(y, y, x, x); }
			float4 yyxy() { return float4(y, y, x, y); }
			float4 yyxz() { return float4(y, y, x, z); }
			float4 yyxw() { return float4(y, y, x, w); }
			float4 yyyx() { return float4(y, y, y, x); }
			float4 yyyy() { return float4(y, y, y, y); }
			float4 yyyz() { return float4(y, y, y, z); }
			float4 yyyw() { return float4(y, y, y, w); }
			float4 yyzx() { return float4(y, y, z, x); }
			float4 yyzy() { return float4(y, y, z, y); }
			float4 yyzz() { return float4(y, y, z, z); }
			float4 yyzw() { return float4(y, y, z, w); }

			float4 yzxx() { return float4(y, z, x, x); }
			float4 yzxy() { return float4(y, z, x, y); }
			float4 yzxz() { return float4(y, z, x, z); }
			float4 yzxw() { return float4(y, z, x, w); }
			float4 yzyx() { return float4(y, z, y, x); }
			float4 yzyy() { return float4(y, z, y, y); }
			float4 yzyz() { return float4(y, z, y, z); }
			float4 yzyw() { return float4(y, z, y, w); }
			float4 yzzx() { return float4(y, z, z, x); }
			float4 yzzy() { return float4(y, z, z, y); }
			float4 yzzz() { return float4(y, z, z, z); }
			float4 yzzw() { return float4(y, z, z, w); }

			float4 ywxx() { return float4(y, w, x, x); }
			float4 ywxy() { return float4(y, w, x, y); }
			float4 ywxz() { return float4(y, w, x, z); }
			float4 ywxw() { return float4(y, w, x, w); }
			float4 ywyx() { return float4(y, w, y, x); }
			float4 ywyy() { return float4(y, w, y, y); }
			float4 ywyz() { return float4(y, w, y, z); }
			float4 ywyw() { return float4(y, w, y, w); }
			float4 ywzx() { return float4(y, w, z, x); }
			float4 ywzy() { return float4(y, w, z, y); }
			float4 ywzz() { return float4(y, w, z, z); }
			float4 ywzw() { return float4(y, w, z, w); }

			float4 zxxx() { return float4(z, x, x, x); }
			float4 zxxy() { return float4(z, x, x, y); }
			float4 zxxz() { return float4(z, x, x, z); }
			float4 zxxw() { return float4(z, x, x, w); }
			float4 zxyx() { return float4(z, x, y, x); }
			float4 zxyy() { return float4(z, x, y, y); }
			float4 zxyz() { return float4(z, x, y, z); }
			float4 zxyw() { return float4(z, x, y, w); }
			float4 zxzx() { return float4(z, x, z, x); }
			float4 zxzy() { return float4(z, x, z, y); }
			float4 zxzz() { return float4(z, x, z, z); }
			float4 zxzw() { return float4(z, x, z, w); }

			float4 zyxx() { return float4(z, y, x, x); }
			float4 zyxy() { return float4(z, y, x, y); }
			float4 zyxz() { return float4(z, y, x, z); }
			float4 zyxw() { return float4(z, y, x, w); }
			float4 zyyx() { return float4(z, y, y, x); }
			float4 zyyy() { return float4(z, y, y, y); }
			float4 zyyz() { return float4(z, y, y, z); }
			float4 zyyw() { return float4(z, y, y, w); }
			float4 zyzx() { return float4(z, y, z, x); }
			float4 zyzy() { return float4(z, y, z, y); }
			float4 zyzz() { return float4(z, y, z, z); }
			float4 zyzw() { return float4(z, y, z, w); }

			float4 zzxx() { return float4(z, z, x, x); }
			float4 zzxy() { return float4(z, z, x, y); }
			float4 zzxz() { return float4(z, z, x, z); }
			float4 zzxw() { return float4(z, z, x, w); }
			float4 zzyx() { return float4(z, z, y, x); }
			float4 zzyy() { return float4(z, z, y, y); }
			float4 zzyz() { return float4(z, z, y, z); }
			float4 zzyw() { return float4(z, z, y, w); }
			float4 zzzx() { return float4(z, z, z, x); }
			float4 zzzy() { return float4(z, z, z, y); }
			float4 zzzz() { return float4(z, z, z, z); }
			float4 zzzw() { return float4(z, z, z, w); }

			float4 zwxx() { return float4(z, w, x, x); }
			float4 zwxy() { return float4(z, w, x, y); }
			float4 zwxz() { return float4(z, w, x, z); }
			float4 zwxw() { return float4(z, w, x, w); }
			float4 zwyx() { return float4(z, w, y, x); }
			float4 zwyy() { return float4(z, w, y, y); }
			float4 zwyz() { return float4(z, w, y, z); }
			float4 zwyw() { return float4(z, w, y, w); }
			float4 zwzx() { return float4(z, w, z, x); }
			float4 zwzy() { return float4(z, w, z, y); }
			float4 zwzz() { return float4(z, w, z, z); }
			float4 zwzw() { return float4(z, w, z, w); }

			float4 wxxx() { return float4(w, x, x, x); }
			float4 wxxy() { return float4(w, x, x, y); }
			float4 wxxz() { return float4(w, x, x, z); }
			float4 wxxw() { return float4(w, x, x, w); }
			float4 wxyx() { return float4(w, x, y, x); }
			float4 wxyy() { return float4(w, x, y, y); }
			float4 wxyz() { return float4(w, x, y, z); }
			float4 wxyw() { return float4(w, x, y, w); }
			float4 wxzx() { return float4(w, x, z, x); }
			float4 wxzy() { return float4(w, x, z, y); }
			float4 wxzz() { return float4(w, x, z, z); }
			float4 wxzw() { return float4(w, x, z, w); }

			float4 wyxx() { return float4(w, y, x, x); }
			float4 wyxy() { return float4(w, y, x, y); }
			float4 wyxz() { return float4(w, y, x, z); }
			float4 wyxw() { return float4(w, y, x, w); }
			float4 wyyx() { return float4(w, y, y, x); }
			float4 wyyy() { return float4(w, y, y, y); }
			float4 wyyz() { return float4(w, y, y, z); }
			float4 wyyw() { return float4(w, y, y, w); }
			float4 wyzx() { return float4(w, y, z, x); }
			float4 wyzy() { return float4(w, y, z, y); }
			float4 wyzz() { return float4(w, y, z, z); }
			float4 wyzw() { return float4(w, y, z, w); }

			float4 wzxx() { return float4(w, z, x, x); }
			float4 wzxy() { return float4(w, z, x, y); }
			float4 wzxz() { return float4(w, z, x, z); }
			float4 wzxw() { return float4(w, z, x, w); }
			float4 wzyx() { return float4(w, z, y, x); }
			float4 wzyy() { return float4(w, z, y, y); }
			float4 wzyz() { return float4(w, z, y, z); }
			float4 wzyw() { return float4(w, z, y, w); }
			float4 wzzx() { return float4(w, z, z, x); }
			float4 wzzy() { return float4(w, z, z, y); }
			float4 wzzz() { return float4(w, z, z, z); }
			float4 wzzw() { return float4(w, z, z, w); }

			float4 wwxx() { return float4(w, w, x, x); }
			float4 wwxy() { return float4(w, w, x, y); }
			float4 wwxz() { return float4(w, w, x, z); }
			float4 wwxw() { return float4(w, w, x, w); }
			float4 wwyx() { return float4(w, w, y, x); }
			float4 wwyy() { return float4(w, w, y, y); }
			float4 wwyz() { return float4(w, w, y, z); }
			float4 wwyw() { return float4(w, w, y, w); }
			float4 wwzx() { return float4(w, w, z, x); }
			float4 wwzy() { return float4(w, w, z, y); }
			float4 wwzz() { return float4(w, w, z, z); }
			float4 wwzw() { return float4(w, w, z, w); }

			float3 xxx() { return float3(x, x, x); }
			float3 xxy() { return float3(x, x, y); }
			float3 xxz() { return float3(x, x, z); }
			float3 xxw() { return float3(x, x, w); }
			float3 xyx() { return float3(x, y, x); }
			float3 xyy() { return float3(x, y, y); }
			float3 xyz() { return float3(x, y, z); }
			float3 xyw() { return float3(x, y, w); }
			float3 xzx() { return float3(x, z, x); }
			float3 xzy() { return float3(x, z, y); }
			float3 xzz() { return float3(x, z, z); }
			float3 xzw() { return float3(x, z, w); }

			float3 yxx() { return float3(y, x, x); }
			float3 yxy() { return float3(y, x, y); }
			float3 yxz() { return float3(y, x, z); }
			float3 yxw() { return float3(y, x, w); }
			float3 yyx() { return float3(y, y, x); }
			float3 yyy() { return float3(y, y, y); }
			float3 yyz() { return float3(y, y, z); }
			float3 yyw() { return float3(y, y, w); }
			float3 yzx() { return float3(y, z, x); }
			float3 yzy() { return float3(y, z, y); }
			float3 yzz() { return float3(y, z, z); }
			float3 yzw() { return float3(y, z, w); }

			float3 zxx() { return float3(z, x, x); }
			float3 zxy() { return float3(z, x, y); }
			float3 zxz() { return float3(z, x, z); }
			float3 zxw() { return float3(z, x, w); }
			float3 zyx() { return float3(z, y, x); }
			float3 zyy() { return float3(z, y, y); }
			float3 zyz() { return float3(z, y, z); }
			float3 zyw() { return float3(z, y, w); }
			float3 zzx() { return float3(z, z, x); }
			float3 zzy() { return float3(z, z, y); }
			float3 zzz() { return float3(z, z, z); }
			float3 zzw() { return float3(z, z, w); }

			float3 wxx() { return float3(w, x, x); }
			float3 wxy() { return float3(w, x, y); }
			float3 wxz() { return float3(w, x, z); }
			float3 wxw() { return float3(w, x, w); }
			float3 wyx() { return float3(w, y, x); }
			float3 wyy() { return float3(w, y, y); }
			float3 wyz() { return float3(w, y, z); }
			float3 wyw() { return float3(w, y, w); }
			float3 wzx() { return float3(w, z, x); }
			float3 wzy() { return float3(w, z, y); }
			float3 wzz() { return float3(w, z, z); }
			float3 wzw() { return float3(w, z, w); }

			float2 xx() { return float2(x, x); }
			float2 xy() { return float2(x, y); }
			float2 xz() { return float2(x, z); }
			float2 xw() { return float2(x, w); }

			float2 yx() { return float2(y, x); }
			float2 yy() { return float2(y, y); }
			float2 yz() { return float2(y, z); }
			float2 yw() { return float2(y, w); }

			float2 zx() { return float2(z, z); }
			float2 zy() { return float2(z, y); }
			float2 zz() { return float2(z, z); }
			float2 zw() { return float2(z, w); }

			float2 wx() { return float2(w, w); }
			float2 wy() { return float2(w, y); }
			float2 wz() { return float2(w, z); }
			float2 ww() { return float2(w, w); }
	};

	static float4 operator *(const float& val, const float4& vec) {
		return vec * val;
	}

	static std::ostream& operator << (std::ostream& stream, const float4& vec) {
		stream << vec.str();
		return stream;
 	}

	static std::stringstream& operator << (std::stringstream& stream, const float4& vec) {
		stream << vec.str();
		return stream;
	}

	static float4 lerp(const float4& lhs, const float4& rhs, float t) {
		float x = lhs.x + (rhs.x - lhs.x) * t;
		float y = lhs.y + (rhs.y - lhs.y) * t;
		float z = lhs.z + (rhs.z - lhs.z) * t;
		float w = lhs.w + (rhs.w - lhs.w) * t;
		return float4(x, y, z, w);
	}

	const float4 float4::ZERO = float4(0.0f, 0.0f, 0.0f, 0.0f);
	const float4 float4::ONE = float4(1.0f, 1.0f, 1.0f, 1.0f);
}
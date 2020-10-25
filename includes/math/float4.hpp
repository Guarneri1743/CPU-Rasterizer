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
			float4 xxxx() const { return float4(x, x, x, x); }
			float4 xxxy() const { return float4(x, x, x, y); }
			float4 xxxz() const { return float4(x, x, x, z); }
			float4 xxxw() const { return float4(x, x, x, w); }
			float4 xxyx() const { return float4(x, x, y, x); }
			float4 xxyy() const { return float4(x, x, y, y); }
			float4 xxyz() const { return float4(x, x, y, z); }
			float4 xxyw() const { return float4(x, x, y, w); }
			float4 xxzx() const { return float4(x, x, z, x); }
			float4 xxzy() const { return float4(x, x, z, y); }
			float4 xxzz() const { return float4(x, x, z, z); }
			float4 xxzw() const { return float4(x, x, z, w); }

			float4 xyxx() const { return float4(x, y, x, x); }
			float4 xyxy() const { return float4(x, y, x, y); }
			float4 xyxz() const { return float4(x, y, x, z); }
			float4 xyxw() const { return float4(x, y, x, w); }
			float4 xyyx() const { return float4(x, y, y, x); }
			float4 xyyy() const { return float4(x, y, y, y); }
			float4 xyyz() const { return float4(x, y, y, z); }
			float4 xyyw() const { return float4(x, y, y, w); }
			float4 xyzx() const { return float4(x, y, z, x); }
			float4 xyzy() const { return float4(x, y, z, y); }
			float4 xyzz() const { return float4(x, y, z, z); }
			float4 xyzw() const { return float4(x, y, z, w); }

			float4 xzxx() const { return float4(x, z, x, x); }
			float4 xzxy() const { return float4(x, z, x, y); }
			float4 xzxz() const { return float4(x, z, x, z); }
			float4 xzxw() const { return float4(x, z, x, w); }
			float4 xzyx() const { return float4(x, z, y, x); }
			float4 xzyy() const { return float4(x, z, y, y); }
			float4 xzyz() const { return float4(x, z, y, z); }
			float4 xzyw() const { return float4(x, z, y, w); }
			float4 xzzx() const { return float4(x, z, z, x); }
			float4 xzzy() const { return float4(x, z, z, y); }
			float4 xzzz() const { return float4(x, z, z, z); }
			float4 xzzw() const { return float4(x, z, z, w); }

			float4 xwxx() const { return float4(x, w, x, x); }
			float4 xwxy() const { return float4(x, w, x, y); }
			float4 xwxz() const { return float4(x, w, x, z); }
			float4 xwxw() const { return float4(x, w, x, w); }
			float4 xwyx() const { return float4(x, w, y, x); }
			float4 xwyy() const { return float4(x, w, y, y); }
			float4 xwyz() const { return float4(x, w, y, z); }
			float4 xwyw() const { return float4(x, w, y, w); }
			float4 xwzx() const { return float4(x, w, z, x); }
			float4 xwzy() const { return float4(x, w, z, y); }
			float4 xwzz() const { return float4(x, w, z, z); }
			float4 xwzw() const { return float4(x, w, z, w); }

			float4 yxxx() const { return float4(y, x, x, x); }
			float4 yxxy() const { return float4(y, x, x, y); }
			float4 yxxz() const { return float4(y, x, x, z); }
			float4 yxxw() const { return float4(y, x, x, w); }
			float4 yxyx() const { return float4(y, x, y, x); }
			float4 yxyy() const { return float4(y, x, y, y); }
			float4 yxyz() const { return float4(y, x, y, z); }
			float4 yxyw() const { return float4(y, x, y, w); }
			float4 yxzx() const { return float4(y, x, z, x); }
			float4 yxzy() const { return float4(y, x, z, y); }
			float4 yxzz() const { return float4(y, x, z, z); }
			float4 yxzw() const { return float4(y, x, z, w); }

			float4 yyxx() const { return float4(y, y, x, x); }
			float4 yyxy() const { return float4(y, y, x, y); }
			float4 yyxz() const { return float4(y, y, x, z); }
			float4 yyxw() const { return float4(y, y, x, w); }
			float4 yyyx() const { return float4(y, y, y, x); }
			float4 yyyy() const { return float4(y, y, y, y); }
			float4 yyyz() const { return float4(y, y, y, z); }
			float4 yyyw() const { return float4(y, y, y, w); }
			float4 yyzx() const { return float4(y, y, z, x); }
			float4 yyzy() const { return float4(y, y, z, y); }
			float4 yyzz() const { return float4(y, y, z, z); }
			float4 yyzw() const { return float4(y, y, z, w); }

			float4 yzxx() const { return float4(y, z, x, x); }
			float4 yzxy() const { return float4(y, z, x, y); }
			float4 yzxz() const { return float4(y, z, x, z); }
			float4 yzxw() const { return float4(y, z, x, w); }
			float4 yzyx() const { return float4(y, z, y, x); }
			float4 yzyy() const { return float4(y, z, y, y); }
			float4 yzyz() const { return float4(y, z, y, z); }
			float4 yzyw() const { return float4(y, z, y, w); }
			float4 yzzx() const { return float4(y, z, z, x); }
			float4 yzzy() const { return float4(y, z, z, y); }
			float4 yzzz() const { return float4(y, z, z, z); }
			float4 yzzw() const { return float4(y, z, z, w); }

			float4 ywxx() const { return float4(y, w, x, x); }
			float4 ywxy() const { return float4(y, w, x, y); }
			float4 ywxz() const { return float4(y, w, x, z); }
			float4 ywxw() const { return float4(y, w, x, w); }
			float4 ywyx() const { return float4(y, w, y, x); }
			float4 ywyy() const { return float4(y, w, y, y); }
			float4 ywyz() const { return float4(y, w, y, z); }
			float4 ywyw() const { return float4(y, w, y, w); }
			float4 ywzx() const { return float4(y, w, z, x); }
			float4 ywzy() const { return float4(y, w, z, y); }
			float4 ywzz() const { return float4(y, w, z, z); }
			float4 ywzw() const { return float4(y, w, z, w); }

			float4 zxxx() const { return float4(z, x, x, x); }
			float4 zxxy() const { return float4(z, x, x, y); }
			float4 zxxz() const { return float4(z, x, x, z); }
			float4 zxxw() const { return float4(z, x, x, w); }
			float4 zxyx() const { return float4(z, x, y, x); }
			float4 zxyy() const { return float4(z, x, y, y); }
			float4 zxyz() const { return float4(z, x, y, z); }
			float4 zxyw() const { return float4(z, x, y, w); }
			float4 zxzx() const { return float4(z, x, z, x); }
			float4 zxzy() const { return float4(z, x, z, y); }
			float4 zxzz() const { return float4(z, x, z, z); }
			float4 zxzw() const { return float4(z, x, z, w); }

			float4 zyxx() const { return float4(z, y, x, x); }
			float4 zyxy() const { return float4(z, y, x, y); }
			float4 zyxz() const { return float4(z, y, x, z); }
			float4 zyxw() const { return float4(z, y, x, w); }
			float4 zyyx() const { return float4(z, y, y, x); }
			float4 zyyy() const { return float4(z, y, y, y); }
			float4 zyyz() const { return float4(z, y, y, z); }
			float4 zyyw() const { return float4(z, y, y, w); }
			float4 zyzx() const { return float4(z, y, z, x); }
			float4 zyzy() const { return float4(z, y, z, y); }
			float4 zyzz() const { return float4(z, y, z, z); }
			float4 zyzw() const { return float4(z, y, z, w); }

			float4 zzxx() const { return float4(z, z, x, x); }
			float4 zzxy() const { return float4(z, z, x, y); }
			float4 zzxz() const { return float4(z, z, x, z); }
			float4 zzxw() const { return float4(z, z, x, w); }
			float4 zzyx() const { return float4(z, z, y, x); }
			float4 zzyy() const { return float4(z, z, y, y); }
			float4 zzyz() const { return float4(z, z, y, z); }
			float4 zzyw() const { return float4(z, z, y, w); }
			float4 zzzx() const { return float4(z, z, z, x); }
			float4 zzzy() const { return float4(z, z, z, y); }
			float4 zzzz() const { return float4(z, z, z, z); }
			float4 zzzw() const { return float4(z, z, z, w); }

			float4 zwxx() const { return float4(z, w, x, x); }
			float4 zwxy() const { return float4(z, w, x, y); }
			float4 zwxz() const { return float4(z, w, x, z); }
			float4 zwxw() const { return float4(z, w, x, w); }
			float4 zwyx() const { return float4(z, w, y, x); }
			float4 zwyy() const { return float4(z, w, y, y); }
			float4 zwyz() const { return float4(z, w, y, z); }
			float4 zwyw() const { return float4(z, w, y, w); }
			float4 zwzx() const { return float4(z, w, z, x); }
			float4 zwzy() const { return float4(z, w, z, y); }
			float4 zwzz() const { return float4(z, w, z, z); }
			float4 zwzw() const { return float4(z, w, z, w); }

			float4 wxxx() const { return float4(w, x, x, x); }
			float4 wxxy() const { return float4(w, x, x, y); }
			float4 wxxz() const { return float4(w, x, x, z); }
			float4 wxxw() const { return float4(w, x, x, w); }
			float4 wxyx() const { return float4(w, x, y, x); }
			float4 wxyy() const { return float4(w, x, y, y); }
			float4 wxyz() const { return float4(w, x, y, z); }
			float4 wxyw() const { return float4(w, x, y, w); }
			float4 wxzx() const { return float4(w, x, z, x); }
			float4 wxzy() const { return float4(w, x, z, y); }
			float4 wxzz() const { return float4(w, x, z, z); }
			float4 wxzw() const { return float4(w, x, z, w); }

			float4 wyxx() const { return float4(w, y, x, x); }
			float4 wyxy() const { return float4(w, y, x, y); }
			float4 wyxz() const { return float4(w, y, x, z); }
			float4 wyxw() const { return float4(w, y, x, w); }
			float4 wyyx() const { return float4(w, y, y, x); }
			float4 wyyy() const { return float4(w, y, y, y); }
			float4 wyyz() const { return float4(w, y, y, z); }
			float4 wyyw() const { return float4(w, y, y, w); }
			float4 wyzx() const { return float4(w, y, z, x); }
			float4 wyzy() const { return float4(w, y, z, y); }
			float4 wyzz() const { return float4(w, y, z, z); }
			float4 wyzw() const { return float4(w, y, z, w); }

			float4 wzxx() const { return float4(w, z, x, x); }
			float4 wzxy() const { return float4(w, z, x, y); }
			float4 wzxz() const { return float4(w, z, x, z); }
			float4 wzxw() const { return float4(w, z, x, w); }
			float4 wzyx() const { return float4(w, z, y, x); }
			float4 wzyy() const { return float4(w, z, y, y); }
			float4 wzyz() const { return float4(w, z, y, z); }
			float4 wzyw() const { return float4(w, z, y, w); }
			float4 wzzx() const { return float4(w, z, z, x); }
			float4 wzzy() const { return float4(w, z, z, y); }
			float4 wzzz() const { return float4(w, z, z, z); }
			float4 wzzw() const { return float4(w, z, z, w); }

			float4 wwxx() const { return float4(w, w, x, x); }
			float4 wwxy() const { return float4(w, w, x, y); }
			float4 wwxz() const { return float4(w, w, x, z); }
			float4 wwxw() const { return float4(w, w, x, w); }
			float4 wwyx() const { return float4(w, w, y, x); }
			float4 wwyy() const { return float4(w, w, y, y); }
			float4 wwyz() const { return float4(w, w, y, z); }
			float4 wwyw() const { return float4(w, w, y, w); }
			float4 wwzx() const { return float4(w, w, z, x); }
			float4 wwzy() const { return float4(w, w, z, y); }
			float4 wwzz() const { return float4(w, w, z, z); }
			float4 wwzw() const { return float4(w, w, z, w); }

			float3 xxx() const { return float3(x, x, x); }
			float3 xxy() const { return float3(x, x, y); }
			float3 xxz() const { return float3(x, x, z); }
			float3 xxw() const { return float3(x, x, w); }
			float3 xyx() const { return float3(x, y, x); }
			float3 xyy() const { return float3(x, y, y); }
			float3 xyz() const { return float3(x, y, z); }
			float3 xyw() const { return float3(x, y, w); }
			float3 xzx() const { return float3(x, z, x); }
			float3 xzy() const { return float3(x, z, y); }
			float3 xzz() const { return float3(x, z, z); }
			float3 xzw() const { return float3(x, z, w); }

			float3 yxx() const { return float3(y, x, x); }
			float3 yxy() const { return float3(y, x, y); }
			float3 yxz() const { return float3(y, x, z); }
			float3 yxw() const { return float3(y, x, w); }
			float3 yyx() const { return float3(y, y, x); }
			float3 yyy() const { return float3(y, y, y); }
			float3 yyz() const { return float3(y, y, z); }
			float3 yyw() const { return float3(y, y, w); }
			float3 yzx() const { return float3(y, z, x); }
			float3 yzy() const { return float3(y, z, y); }
			float3 yzz() const { return float3(y, z, z); }
			float3 yzw() const { return float3(y, z, w); }

			float3 zxx() const { return float3(z, x, x); }
			float3 zxy() const { return float3(z, x, y); }
			float3 zxz() const { return float3(z, x, z); }
			float3 zxw() const { return float3(z, x, w); }
			float3 zyx() const { return float3(z, y, x); }
			float3 zyy() const { return float3(z, y, y); }
			float3 zyz() const { return float3(z, y, z); }
			float3 zyw() const { return float3(z, y, w); }
			float3 zzx() const { return float3(z, z, x); }
			float3 zzy() const { return float3(z, z, y); }
			float3 zzz() const { return float3(z, z, z); }
			float3 zzw() const { return float3(z, z, w); }

			float3 wxx() const { return float3(w, x, x); }
			float3 wxy() const { return float3(w, x, y); }
			float3 wxz() const { return float3(w, x, z); }
			float3 wxw() const { return float3(w, x, w); }
			float3 wyx() const { return float3(w, y, x); }
			float3 wyy() const { return float3(w, y, y); }
			float3 wyz() const { return float3(w, y, z); }
			float3 wyw() const { return float3(w, y, w); }
			float3 wzx() const { return float3(w, z, x); }
			float3 wzy() const { return float3(w, z, y); }
			float3 wzz() const { return float3(w, z, z); }
			float3 wzw() const { return float3(w, z, w); }

			float2 xx() const { return float2(x, x); }
			float2 xy() const { return float2(x, y); }
			float2 xz() const { return float2(x, z); }
			float2 xw() const { return float2(x, w); }

			float2 yx() const { return float2(y, x); }
			float2 yy() const { return float2(y, y); }
			float2 yz() const { return float2(y, z); }
			float2 yw() const { return float2(y, w); }

			float2 zx() const { return float2(z, z); }
			float2 zy() const { return float2(z, y); }
			float2 zz() const { return float2(z, z); }
			float2 zw() const { return float2(z, w); }

			float2 wx() const { return float2(w, w); }
			float2 wy() const { return float2(w, y); }
			float2 wz() const { return float2(w, z); }
			float2 ww() const { return float2(w, w); }
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
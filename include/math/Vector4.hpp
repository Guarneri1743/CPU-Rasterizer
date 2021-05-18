#ifndef _VEC4_
#define _VEC4_
#include <cmath>
#include <string>
#include "Marcos.h"
#include "Vector2.hpp"
#include "Vector3.hpp"

namespace Guarneri
{
	struct Vector4
	{
		static const Vector4 ZERO;
		static const Vector4 ONE;

		float x;
		float y;
		float z;
		float w;

		Vector4();
		Vector4(const float& r);
		Vector4(const Vector3& v);
		Vector4(const float& x, const Vector3& yzw);
		Vector4(const float& x, const float& y, const Vector2& zw);
		Vector4(const Vector2& v, const float& z, const float& w);
		Vector4(const Vector2& v, const Vector2& zw);
		Vector4(const Vector3& v, const float& w);
		Vector4(const float& x, const float& y, const float& z);
		Vector4(const float& x, const float& y, const float& z, const float& w);

		float& operator[](const unsigned int& i);
		const float& operator[](const unsigned int& i) const;
		bool operator == (const Vector4& other);
		bool operator != (const Vector4& other);
		Vector4& operator +=(const Vector4& other);
		Vector4& operator -=(const Vector4& other);
		Vector4& operator /=(const float& val);
		Vector4& operator *=(const float& val);
		Vector4 operator +(const Vector4& other) const;
		Vector4 operator -(const Vector4& other) const;
		Vector4 operator *(const float& scale) const;
		Vector4 operator +(const float& other) const;
		Vector4 operator -(const float& other) const;
		Vector4 operator -() const;
		Vector4 operator *(const Vector4& b) const;
		Vector4& operator *=(const Vector4& b);
		Vector4 operator /(const float& val) const;
		Vector4 operator /(const Vector4& val) const;

		Vector4 normalized();
		static float dot(const Vector4& lhs, const Vector4& rhs);
		static float magnitude(const Vector4& vector);
		static float length(const Vector4& a, const Vector4& b);
		static Vector4 maximum(const Vector4& a, const Vector4& b);
		static Vector4 minimum(const Vector4& a, const Vector4& b);
		static Vector4 abs(const Vector4& v);
		static Vector4 normalize(const Vector4& value);

		static rapidjson::Value serialize(rapidjson::Document& doc, const Vector4& vec);
		static Vector4 deserialize(const rapidjson::Value& v);

		std::string str() const;

		Vector4 xxxx() const;
		Vector4 xxxy() const;
		Vector4 xxxz() const;
		Vector4 xxxw() const;
		Vector4 xxyx() const;
		Vector4 xxyy() const;
		Vector4 xxyz() const;
		Vector4 xxyw() const;
		Vector4 xxzx() const;
		Vector4 xxzy() const;
		Vector4 xxzz() const;
		Vector4 xxzw() const;

		Vector4 xyxx() const;
		Vector4 xyxy() const;
		Vector4 xyxz() const;
		Vector4 xyxw() const;
		Vector4 xyyx() const;
		Vector4 xyyy() const;
		Vector4 xyyz() const;
		Vector4 xyyw() const;
		Vector4 xyzx() const;
		Vector4 xyzy() const;
		Vector4 xyzz() const;
		Vector4 xyzw() const;

		Vector4 xzxx() const;
		Vector4 xzxy() const;
		Vector4 xzxz() const;
		Vector4 xzxw() const;
		Vector4 xzyx() const;
		Vector4 xzyy() const;
		Vector4 xzyz() const;
		Vector4 xzyw() const;
		Vector4 xzzx() const;
		Vector4 xzzy() const;
		Vector4 xzzz() const;
		Vector4 xzzw() const;

		Vector4 xwxx() const;
		Vector4 xwxy() const;
		Vector4 xwxz() const;
		Vector4 xwxw() const;
		Vector4 xwyx() const;
		Vector4 xwyy() const;
		Vector4 xwyz() const;
		Vector4 xwyw() const;
		Vector4 xwzx() const;
		Vector4 xwzy() const;
		Vector4 xwzz() const;
		Vector4 xwzw() const;

		Vector4 yxxx() const;
		Vector4 yxxy() const;
		Vector4 yxxz() const;
		Vector4 yxxw() const;
		Vector4 yxyx() const;
		Vector4 yxyy() const;
		Vector4 yxyz() const;
		Vector4 yxyw() const;
		Vector4 yxzx() const;
		Vector4 yxzy() const;
		Vector4 yxzz() const;
		Vector4 yxzw() const;

		Vector4 yyxx() const;
		Vector4 yyxy() const;
		Vector4 yyxz() const;
		Vector4 yyxw() const;
		Vector4 yyyx() const;
		Vector4 yyyy() const;
		Vector4 yyyz() const;
		Vector4 yyyw() const;
		Vector4 yyzx() const;
		Vector4 yyzy() const;
		Vector4 yyzz() const;
		Vector4 yyzw() const;

		Vector4 yzxx() const;
		Vector4 yzxy() const;
		Vector4 yzxz() const;
		Vector4 yzxw() const;
		Vector4 yzyx() const;
		Vector4 yzyy() const;
		Vector4 yzyz() const;
		Vector4 yzyw() const;
		Vector4 yzzx() const;
		Vector4 yzzy() const;
		Vector4 yzzz() const;
		Vector4 yzzw() const;

		Vector4 ywxx() const;
		Vector4 ywxy() const;
		Vector4 ywxz() const;
		Vector4 ywxw() const;
		Vector4 ywyx() const;
		Vector4 ywyy() const;
		Vector4 ywyz() const;
		Vector4 ywyw() const;
		Vector4 ywzx() const;
		Vector4 ywzy() const;
		Vector4 ywzz() const;
		Vector4 ywzw() const;

		Vector4 zxxx() const;
		Vector4 zxxy() const;
		Vector4 zxxz() const;
		Vector4 zxxw() const;
		Vector4 zxyx() const;
		Vector4 zxyy() const;
		Vector4 zxyz() const;
		Vector4 zxyw() const;
		Vector4 zxzx() const;
		Vector4 zxzy() const;
		Vector4 zxzz() const;
		Vector4 zxzw() const;

		Vector4 zyxx() const;
		Vector4 zyxy() const;
		Vector4 zyxz() const;
		Vector4 zyxw() const;
		Vector4 zyyx() const;
		Vector4 zyyy() const;
		Vector4 zyyz() const;
		Vector4 zyyw() const;
		Vector4 zyzx() const;
		Vector4 zyzy() const;
		Vector4 zyzz() const;
		Vector4 zyzw() const;

		Vector4 zzxx() const;
		Vector4 zzxy() const;
		Vector4 zzxz() const;
		Vector4 zzxw() const;
		Vector4 zzyx() const;
		Vector4 zzyy() const;
		Vector4 zzyz() const;
		Vector4 zzyw() const;
		Vector4 zzzx() const;
		Vector4 zzzy() const;
		Vector4 zzzz() const;
		Vector4 zzzw() const;

		Vector4 zwxx() const;
		Vector4 zwxy() const;
		Vector4 zwxz() const;
		Vector4 zwxw() const;
		Vector4 zwyx() const;
		Vector4 zwyy() const;
		Vector4 zwyz() const;
		Vector4 zwyw() const;
		Vector4 zwzx() const;
		Vector4 zwzy() const;
		Vector4 zwzz() const;
		Vector4 zwzw() const;

		Vector4 wxxx() const;
		Vector4 wxxy() const;
		Vector4 wxxz() const;
		Vector4 wxxw() const;
		Vector4 wxyx() const;
		Vector4 wxyy() const;
		Vector4 wxyz() const;
		Vector4 wxyw() const;
		Vector4 wxzx() const;
		Vector4 wxzy() const;
		Vector4 wxzz() const;
		Vector4 wxzw() const;

		Vector4 wyxx() const;
		Vector4 wyxy() const;
		Vector4 wyxz() const;
		Vector4 wyxw() const;
		Vector4 wyyx() const;
		Vector4 wyyy() const;
		Vector4 wyyz() const;
		Vector4 wyyw() const;
		Vector4 wyzx() const;
		Vector4 wyzy() const;
		Vector4 wyzz() const;
		Vector4 wyzw() const;

		Vector4 wzxx() const;
		Vector4 wzxy() const;
		Vector4 wzxz() const;
		Vector4 wzxw() const;
		Vector4 wzyx() const;
		Vector4 wzyy() const;
		Vector4 wzyz() const;
		Vector4 wzyw() const;
		Vector4 wzzx() const;
		Vector4 wzzy() const;
		Vector4 wzzz() const;
		Vector4 wzzw() const;

		Vector4 wwxx() const;
		Vector4 wwxy() const;
		Vector4 wwxz() const;
		Vector4 wwxw() const;
		Vector4 wwyx() const;
		Vector4 wwyy() const;
		Vector4 wwyz() const;
		Vector4 wwyw() const;
		Vector4 wwzx() const;
		Vector4 wwzy() const;
		Vector4 wwzz() const;
		Vector4 wwzw() const;

		Vector3 xxx() const;
		Vector3 xxy() const;
		Vector3 xxz() const;
		Vector3 xxw() const;
		Vector3 xyx() const;
		Vector3 xyy() const;
		Vector3 xyz() const;
		Vector3 xyw() const;
		Vector3 xzx() const;
		Vector3 xzy() const;
		Vector3 xzz() const;
		Vector3 xzw() const;

		Vector3 yxx() const;
		Vector3 yxy() const;
		Vector3 yxz() const;
		Vector3 yxw() const;
		Vector3 yyx() const;
		Vector3 yyy() const;
		Vector3 yyz() const;
		Vector3 yyw() const;
		Vector3 yzx() const;
		Vector3 yzy() const;
		Vector3 yzz() const;
		Vector3 yzw() const;

		Vector3 zxx() const;
		Vector3 zxy() const;
		Vector3 zxz() const;
		Vector3 zxw() const;
		Vector3 zyx() const;
		Vector3 zyy() const;
		Vector3 zyz() const;
		Vector3 zyw() const;
		Vector3 zzx() const;
		Vector3 zzy() const;
		Vector3 zzz() const;
		Vector3 zzw() const;

		Vector3 wxx() const;
		Vector3 wxy() const;
		Vector3 wxz() const;
		Vector3 wxw() const;
		Vector3 wyx() const;
		Vector3 wyy() const;
		Vector3 wyz() const;
		Vector3 wyw() const;
		Vector3 wzx() const;
		Vector3 wzy() const;
		Vector3 wzz() const;
		Vector3 wzw() const;

		Vector2 Vector4::xx() const;
		Vector2 Vector4::xy() const;
		Vector2 Vector4::xz() const;
		Vector2 Vector4::xw() const;

		Vector2 Vector4::yx() const;
		Vector2 Vector4::yy() const;
		Vector2 Vector4::yz() const;
		Vector2 Vector4::yw() const;

		Vector2 Vector4::zx() const;
		Vector2 Vector4::zy() const;
		Vector2 Vector4::zz() const;
		Vector2 Vector4::zw() const;

		Vector2 Vector4::wx() const;
		Vector2 Vector4::wy() const;
		Vector2 Vector4::wz() const;
		Vector2 Vector4::ww() const;

		friend static Vector4 operator +(const float& other, const Vector4& vec)
		{
			return vec + other;
		}

		friend static Vector4 operator -(const float& other, const Vector4& vec)
		{
			return -vec + other;
		}

		friend static Vector4 operator *(const float& val, const Vector4& vec)
		{
			return vec * val;
		}

		friend static Vector4 operator /(const float& val, const Vector4& vec)
		{
			return Vector4(val) / vec;
		}

		friend static Vector4 lerp(const Vector4& lhs, const Vector4& rhs, float t)
		{
			float x = lhs.x + (rhs.x - lhs.x) * t;
			float y = lhs.y + (rhs.y - lhs.y) * t;
			float z = lhs.z + (rhs.z - lhs.z) * t;
			float w = lhs.w + (rhs.w - lhs.w) * t;
			return Vector4(x, y, z, w);
		}
	};
}
#endif
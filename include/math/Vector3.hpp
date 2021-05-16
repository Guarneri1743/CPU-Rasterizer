#ifndef _VEC3_
#define _VEC3_
#include <ostream>
#include <sstream>
#include <iostream>
#include <string>
#include <cmath>
#include "Marcos.h"
#include "Vector2.hpp"

namespace Guarneri
{
	struct Vector3
	{
	public:
		float x;
		float y;
		float z;

	public:
		Vector3();
		Vector3(const Vector2& other);
		Vector3(const float& r);
		Vector3(const float& x, const float& y, const float& z);

		float& operator[](const unsigned int i);
		const float& operator[](const unsigned int i) const;
		bool operator == (const Vector3& other) const;
		bool operator != (const Vector3& other) const;
		Vector3& operator +=(const Vector3& other);
		Vector3& operator -=(const Vector3& other);
		Vector3& operator /=(const float& val);
		Vector3& operator *=(const float& val);
		Vector3 operator +(const Vector3& other) const;
		Vector3 operator -(const Vector3& other) const;
		Vector3 operator +(const float& other) const;
		Vector3 operator -(const float& other) const;
		Vector3 operator -() const;
		Vector3 operator *(const float& scale) const;
		Vector3 operator *(const Vector3& b) const;
		Vector3 operator /(const float& val) const;
		Vector3 operator /(const Vector3& val) const;
		bool operator <=(const Vector3& other) const;
		bool operator >=(const Vector3& other) const;
		bool operator >(const Vector3& other) const;
		bool operator <(const Vector3& other) const;

		Vector3 normalized() const;
		float magnitude() const;

		static float dot(const Vector3& lhs, const Vector3& rhs);
		static Vector3 cross(const Vector3& lhs, const Vector3& rhs);
		static float magnitude(const Vector3& vector);
		static float length(const Vector3& a, const Vector3& b);
		static Vector3 maximum(const Vector3& a, const Vector3& b);
		static Vector3 minimum(const Vector3& a, const Vector3& b);
		static Vector3 saturate(const Vector3& vec);
		static Vector3 abs(const Vector3& v);
		static Vector3 normalize(const Vector3& value);
		static Vector3 floor(const Vector3& val);
		static Vector3 lerp(const Vector3& lhs, const Vector3& rhs, float t);
		static Vector3 pow(const Vector3& v, float power);
		static void calculate_right_up(const Vector3& forward, Vector3& right, Vector3& up);

		std::string str() const;

		Vector3 xxx() const;
		Vector3 xxy() const;
		Vector3 xxz() const;
		Vector3 xyx() const;
		Vector3 xyy() const;
		Vector3 xyz() const;
		Vector3 xzx() const;
		Vector3 xzy() const;
		Vector3 xzz() const;
		Vector3 yxx() const;
		Vector3 yxy() const;
		Vector3 yxz() const;
		Vector3 yyx() const;
		Vector3 yyy() const;
		Vector3 yyz() const;
		Vector3 yzx() const;
		Vector3 yzy() const;
		Vector3 yzz() const;
		Vector3 zxx() const;
		Vector3 zxy() const;
		Vector3 zxz() const;
		Vector3 zyx() const;
		Vector3 zyy() const;
		Vector3 zyz() const;
		Vector3 zzx() const;
		Vector3 zzy() const;
		Vector3 zzz() const;
		Vector2 xx() const;
		Vector2 xy() const;
		Vector2 xz() const;
		Vector2 yx() const;
		Vector2 yy() const;
		Vector2 yz() const;
		Vector2 zz() const;
		Vector2 zx() const;
		Vector2 zy() const;

	public:
		static const Vector3 ZERO;
		static const Vector3 ONE;
		static const Vector3 UP;
		static const Vector3 DOWN;
		static const Vector3 LEFT;
		static const Vector3 RIGHT;
		static const Vector3 FORWARD;
		static const Vector3 BACK;
	};

	static Vector3 operator +(const float& other, const Vector3& vec)
	{
		return vec + other;
	}

	static Vector3 operator -(const float& other, const Vector3& vec)
	{
		return -vec + other;
	}

	static Vector3 operator *(const float& val, const Vector3& vec)
	{
		return vec * val;
	}

	static Vector3 operator /(const float& val, const Vector3& vec)
	{
		return Vector3(val) / vec;
	}

	static std::ostream& operator << (std::ostream& stream, const Vector3& vec)
	{
		stream << vec.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Vector3& vec)
	{
		stream << vec.str();
		return stream;
	}
}
#endif
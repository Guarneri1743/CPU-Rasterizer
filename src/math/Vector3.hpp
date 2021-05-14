#ifndef _VEC3_
#define _VEC3_
#include <ostream>
#include <sstream>
#include <iostream>
#include <math.h>
#include "Vector2.hpp"

#undef min
#undef max

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
		static Vector3 max(const Vector3& a, const Vector3& b);
		static Vector3 min(const Vector3& a, const Vector3& b);
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


	Vector3::Vector3()
	{
		x = 0; y = 0; z = 0;
	}

	Vector3::Vector3(const Vector2& other)
	{
		this->x = other.x; this->y = other.y; this->z = 0.0f;
	}

	Vector3::Vector3(const float& r)
	{
		this->x = r; this->y = r; this->z = r;
	}

	Vector3::Vector3(const float& x, const float& y, const float& z)
	{
		this->x = x; this->y = y; this->z = z;
	}

	Vector3 Vector3::normalized() const
	{
		return Vector3::normalize(*this);
	}

	float& Vector3::operator[](const unsigned int i)
	{
		return (&x)[i];
	}

	const float& Vector3::operator[](const unsigned int i) const
	{
		return (&x)[i];
	}

	bool Vector3::operator == (const Vector3& other) const
	{
		return EQUALS(x, other.x) && EQUALS(y, other.y) && EQUALS(z, other.z);
	}

	bool Vector3::operator != (const Vector3& other) const
	{
		return !(*this == other);
	}

	Vector3& Vector3::operator +=(const Vector3& other)
	{
		x = x + other.x;
		y = y + other.y;
		z = z + other.z;
		return *this;
	}

	Vector3& Vector3::operator -=(const Vector3& other)
	{
		x = x - other.x;
		y = y - other.y;
		z = z - other.z;
		return *this;
	}

	Vector3& Vector3::operator /=(const float& val)
	{
		x = x / val;
		y = y / val;
		z = z / val;
		return *this;
	}

	Vector3& Vector3::operator *=(const float& val)
	{
		x = x * val;
		y = y * val;
		z = z * val;
		return *this;
	}

	Vector3 Vector3::operator +(const Vector3& other) const
	{
		Vector3 ret;
		ret.x = this->x + other.x;
		ret.y = this->y + other.y;
		ret.z = this->z + other.z;
		return ret;
	}

	Vector3 Vector3::operator -(const Vector3& other) const
	{
		Vector3 ret;
		ret.x = this->x - other.x;
		ret.y = this->y - other.y;
		ret.z = this->z - other.z;
		return ret;
	}

	Vector3 Vector3::operator +(const float& other) const
	{
		Vector3 ret;
		ret.x = this->x + other;
		ret.y = this->y + other;
		ret.z = this->z + other;
		return ret;
	}

	Vector3 Vector3::operator -(const float& other) const
	{
		Vector3 ret;
		ret.x = this->x - other;
		ret.y = this->y - other;
		ret.z = this->z - other;
		return ret;
	}

	Vector3 Vector3::operator -() const
	{
		return Vector3(-x, -y, -z);
	}

	Vector3 Vector3::operator *(const float& scale) const
	{
		Vector3 ret;
		ret.x = this->x * scale;
		ret.y = this->y * scale;
		ret.z = this->z * scale;
		return ret;
	}

	Vector3 Vector3::operator *(const Vector3& b) const
	{
		return Vector3(x * b.x, y * b.y, z * b.z);
	}

	Vector3 Vector3::operator /(const float& val) const
	{
		Vector3 ret;
		ret.x = this->x / val;
		ret.y = this->y / val;
		ret.z = this->z / val;
		return ret;
	}

	Vector3 Vector3::operator /(const Vector3& val) const
	{
		Vector3 ret;
		ret.x = this->x / val.x;
		ret.y = this->y / val.y;
		ret.z = this->z / val.z;
		return ret;
	}

	float Vector3::magnitude() const
	{
		return magnitude(*this);
	}

	bool Vector3::operator <=(const Vector3& other) const
	{
		return Vector3::magnitude(*this) <= Vector3::magnitude(other);
	}

	bool Vector3::operator >=(const Vector3& other) const
	{
		return Vector3::magnitude(*this) >= Vector3::magnitude(other);
	}

	bool Vector3::operator >(const Vector3& other) const
	{
		return Vector3::magnitude(*this) > Vector3::magnitude(other);
	}

	bool Vector3::operator <(const Vector3& other) const
	{
		return Vector3::magnitude(*this) < Vector3::magnitude(other);
	}

	float Vector3::dot(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	Vector3 Vector3::cross(const Vector3& lhs, const Vector3& rhs)
	{
		return Vector3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
	}

	float Vector3::magnitude(const Vector3& vector)
	{
		return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	}

	float Vector3::length(const Vector3& a, const Vector3& b)
	{
		return magnitude(a - b);
	}

	Vector3 Vector3::saturate(const Vector3& vec)
	{
		return Vector3(Vector3::max(Vector3::min(Vector3::ONE, vec), Vector3::ZERO));
	}

	Vector3 Vector3::max(const Vector3& a, const Vector3& b)
	{
		return Vector3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
	}

	Vector3 Vector3::min(const Vector3& a, const Vector3& b)
	{
		return Vector3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
	}

	Vector3 Vector3::abs(const Vector3& v)
	{
		return Vector3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
	}

	Vector3 Vector3::pow(const Vector3& v, float power)
	{
		return Vector3(std::pow(v.x, power), std::pow(v.y, power), std::pow(v.z, power));
	}

	Vector3 Vector3::normalize(const Vector3& value)
	{
		float num = magnitude(value);
		Vector3 retult;
		if (num > EPSILON)
		{
			retult = value / num;
		}
		else
		{
			retult = Vector3();
		}
		return retult;
	}

	Vector3 Vector3::floor(const Vector3& val)
	{
		return Vector3(std::floor(val.x), std::floor(val.y), std::floor(val.z));
	}

	Vector3 Vector3::lerp(const Vector3& lhs, const Vector3& rhs, float t)
	{
		float x = lhs.x + (rhs.x - lhs.x) * t;
		float y = lhs.y + (rhs.y - lhs.y) * t;
		float z = lhs.z + (rhs.z - lhs.z) * t;
		return Vector3(x, y, z);
	}

	void Vector3::calculate_right_up(const Vector3& forward, Vector3& right, Vector3& up)
	{
	#ifdef LEFT_HANDED
		right = Vector3::normalize(Vector3::cross(Vector3::UP, forward));
		up = Vector3::cross(forward, right);
	#else 
		right = Vector3::normalize(Vector3::cross(forward, Vector3::UP));
		up = Vector3::cross(right, forward);
	#endif
	}

	std::string Vector3::str() const
	{
		std::stringstream ss;
		ss << "(" << std::setprecision(FLOAT_LOG_PRECISION) << this->x << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->y << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->z << ")";
		return ss.str();
	}

	Vector3 Vector3::xxx() const
	{
		return Vector3(x, x, x);
	}

	Vector3 Vector3::xxy() const
	{
		return Vector3(x, x, y);
	}

	Vector3 Vector3::xxz() const
	{
		return Vector3(x, x, z);
	}

	Vector3 Vector3::xyx() const
	{
		return Vector3(x, y, x);
	}

	Vector3 Vector3::xyy() const
	{
		return Vector3(x, y, y);
	}

	Vector3 Vector3::xyz() const
	{
		return Vector3(x, y, z);
	}

	Vector3 Vector3::xzx() const
	{
		return Vector3(x, z, x);
	}

	Vector3 Vector3::xzy() const
	{
		return Vector3(x, z, y);
	}

	Vector3 Vector3::xzz() const
	{
		return Vector3(x, z, z);
	}

	Vector3 Vector3::yxx() const
	{
		return Vector3(y, x, x);
	}

	Vector3 Vector3::yxy() const
	{
		return Vector3(y, x, y);
	}

	Vector3 Vector3::yxz() const
	{
		return Vector3(y, x, z);
	}

	Vector3 Vector3::yyx() const
	{
		return Vector3(y, y, x);
	}

	Vector3 Vector3::yyy() const
	{
		return Vector3(y, y, y);
	}

	Vector3 Vector3::yyz() const
	{
		return Vector3(y, y, z);
	}

	Vector3 Vector3::yzx() const
	{
		return Vector3(y, z, x);
	}

	Vector3 Vector3::yzy() const
	{
		return Vector3(y, z, y);
	}

	Vector3 Vector3::yzz() const
	{
		return Vector3(y, z, z);
	}

	Vector3 Vector3::zxx() const
	{
		return Vector3(z, x, x);
	}

	Vector3 Vector3::zxy() const
	{
		return Vector3(z, x, y);
	}

	Vector3 Vector3::zxz() const
	{
		return Vector3(z, x, z);
	}

	Vector3 Vector3::zyx() const
	{
		return Vector3(z, y, x);
	}

	Vector3 Vector3::zyy() const
	{
		return Vector3(z, y, y);
	}

	Vector3 Vector3::zyz() const
	{
		return Vector3(z, y, z);
	}

	Vector3 Vector3::zzx() const
	{
		return Vector3(z, z, x);
	}

	Vector3 Vector3::zzy() const
	{
		return Vector3(z, z, y);
	}

	Vector3 Vector3::zzz() const
	{
		return Vector3(z, z, z);
	}

	Vector2 Vector3::xx() const
	{
		return Vector2(x, x);
	}

	Vector2 Vector3::xy() const
	{
		return Vector2(x, y);
	}

	Vector2 Vector3::xz() const
	{
		return Vector2(x, z);
	}

	Vector2 Vector3::yx() const
	{
		return Vector2(y, x);
	}

	Vector2 Vector3::yy() const
	{
		return Vector2(y, y);
	}

	Vector2 Vector3::yz() const
	{
		return Vector2(y, z);
	}

	Vector2 Vector3::zz() const
	{
		return Vector2(z, z);
	}

	Vector2 Vector3::zx() const
	{
		return Vector2(z, x);
	}

	Vector2 Vector3::zy() const
	{
		return Vector2(z, y);
	}

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

	const Vector3 Vector3::ZERO = Vector3(0.0f, 0.0f, 0.0f);
	const Vector3 Vector3::ONE = Vector3(1.0f, 1.0f, 1.0f);
	const Vector3 Vector3::UP = Vector3(0.f, 1.0f, 0.0f);
	const Vector3 Vector3::DOWN = Vector3(0.0f, -1.0f, 0.0f);
	const Vector3 Vector3::LEFT = Vector3(-1.0f, 0.0f, 0.0f);
	const Vector3 Vector3::RIGHT = Vector3(1.0f, 0.0f, 0.0f);
	const Vector3 Vector3::FORWARD = Vector3(0.0f, 0.0f, 1.0f);
	const Vector3 Vector3::BACK = Vector3(0.0f, 0.0f, -1.0f);
}
#endif
#ifndef _VEC2_
#define _VEC2_
#include <cmath>
#include <algorithm>
#include <string>

namespace Guarneri
{
	struct Vector2
	{
		static const Vector2 ZERO;
		static const Vector2 ONE;
		float x;
		float y;

		Vector2();
		Vector2(const float& r);
		Vector2(const Vector2& v);
		Vector2(const float& x, const float& y);
		Vector2(const int& x, const int& y);
		Vector2(const uint32_t& x, const uint32_t& y);

		float& operator[](const uint32_t i);
		const float& operator[](const uint32_t i) const;
		bool operator == (const Vector2& other);
		bool operator != (const Vector2& other);
		Vector2& operator +=(const Vector2& other);
		Vector2& operator -=(const Vector2& other);
		Vector2& operator /=(const float& val);
		Vector2& operator *=(const float& val);
		Vector2 operator +(const Vector2& other) const;
		Vector2 operator -(const Vector2& other) const;
		Vector2 operator *(const float& scale) const;
		Vector2 operator /(const float& val) const;
		Vector2 operator /(const Vector2& val) const;
		Vector2 operator -() const;
		Vector2 operator *(const Vector2& b) const;

		static float dot(const Vector2& lhs, const Vector2& rhs);
		static float magnitude(const Vector2& vector);
		static float length(const Vector2& a, const Vector2& b);
		static Vector2 maximum(const Vector2& a, const Vector2& b);
		static Vector2 minimum(const Vector2& a, const Vector2& b);
		static Vector2 abs(const Vector2& v);
		static Vector2 normalize(const Vector2& value);
		static Vector2 lerp(const Vector2& lhs, const Vector2& rhs, float t);

		std::string str() const;

		Vector2 xx() const;
		Vector2 yy() const;
		Vector2 yx() const;
		Vector2 xy() const;

		friend static Vector2 operator /(const float& val, const Vector2& vec)
		{
			return Vector2(val) / vec;
		}

		friend static Vector2 operator *(const float& val, const Vector2& vec)
		{
			return vec * val;
		}
	};
}
#endif
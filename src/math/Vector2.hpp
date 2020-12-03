#ifndef _VEC2_
#define _VEC2_
#include <Guarneri.hpp>

namespace Guarneri
{
	static Vector2 operator *(const float& val, const Vector2& vec);

	struct Vector2
	{
	public:
		static const Vector2 ZERO;
		static const Vector2 ONE;

		float x;
		float y;

	public:
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
		static Vector2 max(const Vector2& a, const Vector2& b);
		static Vector2 min(const Vector2& a, const Vector2& b);
		static Vector2 abs(const Vector2& v);
		static Vector2 normalize(const Vector2& value);
		static Vector2 lerp(const Vector2& lhs, const Vector2& rhs, float t);
		std::string str() const;
		Vector2 xx() const;
		Vector2 yy() const;
		Vector2 yx() const;
		Vector2 xy() const;
	};


	Vector2::Vector2()
	{
		x = 0; y = 0;
	}

	Vector2::Vector2(const float& r)
	{
		this->x = r; this->y = r;
	}

	Vector2::Vector2(const Vector2& v)
	{
		this->x = v.x; this->y = v.y;
	}

	Vector2::Vector2(const float& x, const float& y)
	{
		this->x = x; this->y = y;
	}

	Vector2::Vector2(const int& x, const int& y)
	{
		this->x = (float)x; this->y = (float)y;
	}

	Vector2::Vector2(const uint32_t& x, const uint32_t& y)
	{
		this->x = (float)x; this->y = (float)y;
	}

	float& Vector2::operator[](const uint32_t i)
	{
		return (&x)[i];
	}

	const float& Vector2::operator[](const uint32_t i) const
	{
		return (&x)[i];
	}

	bool Vector2::operator == (const Vector2& other)
	{
		return EQUALS(x, other.x) && EQUALS(y, other.y);
	}

	bool Vector2::operator != (const Vector2& other)
	{
		return !(*this == other);
	}

	Vector2& Vector2::operator +=(const Vector2& other)
	{
		x = x + other.x;
		y = y + other.y;
		return *this;
	}

	Vector2& Vector2::operator -=(const Vector2& other)
	{
		x = x - other.x;
		y = y - other.y;
		return *this;
	}

	Vector2& Vector2::operator /=(const float& val)
	{
		x = x / val;
		y = y / val;
		return *this;
	}

	Vector2& Vector2::operator *=(const float& val)
	{
		x = x * val;
		y = y * val;
		return *this;
	}

	Vector2 Vector2::operator +(const Vector2& other) const
	{
		Vector2 ret;
		ret.x = this->x + other.x;
		ret.y = this->y + other.y;
		return ret;
	}

	Vector2 Vector2::operator -(const Vector2& other) const
	{
		Vector2 ret;
		ret.x = this->x - other.x;
		ret.y = this->y - other.y;
		return ret;
	}

	Vector2 Vector2::operator *(const float& scale) const
	{
		Vector2 ret;
		ret.x = this->x * scale;
		ret.y = this->y * scale;
		return ret;
	}

	Vector2 Vector2::operator /(const float& val) const
	{
		Vector2 ret;
		ret.x = this->x / val;
		ret.y = this->y / val;
		return ret;
	}

	Vector2 Vector2::operator /(const Vector2& val) const
	{
		Vector2 ret;
		ret.x = this->x / val.x;
		ret.y = this->y / val.y;
		return ret;
	}

	Vector2 Vector2::operator -() const
	{
		return Vector2(-x, -y);
	}

	Vector2 Vector2::operator *(const Vector2& b) const
	{
		return Vector2(x * b.x, y * b.y);
	}

	float Vector2::dot(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	float Vector2::magnitude(const Vector2& vector)
	{
		return sqrt(vector.x * vector.x + vector.y * vector.y);
	}

	float Vector2::length(const Vector2& a, const Vector2& b)
	{
		return magnitude(a - b);
	}

	Vector2 Vector2::max(const Vector2& a, const Vector2& b)
	{
		return Vector2(std::max(a.x, b.x), std::max(a.y, b.y));
	}

	Vector2 Vector2::min(const Vector2& a, const Vector2& b)
	{
		return Vector2(std::min(a.x, b.x), std::min(a.y, b.y));
	}

	Vector2 Vector2::abs(const Vector2& v)
	{
		return Vector2(std::abs(v.x), std::abs(v.y));
	}

	Vector2 Vector2::normalize(const Vector2& value)
	{
		float num = magnitude(value);
		Vector2 retult;
		if (num > EPSILON)
		{
			retult = value / num;
		}
		else
		{
			retult = Vector2();
		}
		return retult;
	}

	Vector2 Vector2::lerp(const Vector2& lhs, const Vector2& rhs, float t)
	{
		float x = lhs.x + (rhs.x - lhs.x) * t;
		float y = lhs.y + (rhs.y - lhs.y) * t;
		return Vector2(x, y);
	}

	std::string Vector2::str() const
	{
		std::stringstream ss;
		ss << "(" << std::setprecision(FLOAT_LOG_PRECISION) << this->x << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->y << ")";
		return ss.str();
	}

	Vector2 Vector2::xx() const
	{
		return Vector2(x, x);
	}
	Vector2 Vector2::yy() const
	{
		return Vector2(y, y);
	}
	Vector2 Vector2::yx() const
	{
		return Vector2(y, x);
	}
	Vector2 Vector2::xy() const
	{
		return Vector2(x, y);
	}

	static Vector2 operator *(const float& val, const Vector2& vec)
	{
		return vec * val;
	}

	static std::ostream& operator << (std::ostream& stream, const Vector2& vec)
	{
		stream << vec.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Vector2& vec)
	{
		stream << vec.str();
		return stream;
	}

	const Vector2 Vector2::ZERO = Vector2(0.0f, 0.0f);
	const Vector2 Vector2::ONE = Vector2(1.0f, 1.0f);
}
#endif
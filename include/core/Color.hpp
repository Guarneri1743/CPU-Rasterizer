#ifndef _COLOR_
#define _COLOR_
#include <string>
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Define.hpp"
#include "rapidjson/document.h"

namespace Guarneri
{
	struct Color
	{
		static const Color WHITE;
		static const Color BLACK;
		static const Color RED;
		static const Color GREEN;
		static const Color BLUE;

		float r;
		float g;
		float b;
		float a;

		constexpr Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
		constexpr Color(const float& v) : r(v), g(v), b(v), a(v) {}
		constexpr Color(const float& x, const float& y, const float& z) : r(x), g(y), b(z), a(1.0f) {}
		constexpr Color(const float& x, const float& y, const float& z, const float& w) : r(x), g(y), b(z), a(w) {}
		constexpr Color(const Vector4& v) : r(v.x), g(v.y), b(v.z), a(v.w) {}
		constexpr Color(const Vector3& v) : r(v.x), g(v.y), b(v.z), a(1.0f) {}
		constexpr Color(const float& x, const Vector3& yzw) : r(x), g(yzw.x), b(yzw.y), a(yzw.z) {}
		Color(const float& r, const float& g, const Vector2& zw);
		Color(const Vector2& v, const float& b, const float& w);
		Color(const Vector2& v, const Vector2& zw);
		Color(const Vector3& v, const float& w);
		Color normalized();
		float& operator[](const uint32_t& i);
		const float& operator[](const uint32_t& i) const;
		bool operator == (const Color& other);
		bool operator != (const Color& other);
		Color& operator +=(const Color& other);
		Color& operator -=(const Color& other);
		Color& operator /=(const float& val);
		Color& operator *=(const float& val);
		Color operator +(const Color& other) const;
		Color operator -(const Color& other) const;
		Color operator *(const float& scale) const;
		Color operator /(const float& val) const;
		Color operator /(const Color& val) const;
		Color operator +(const float& other) const;
		Color operator -(const float& other) const;
		Color operator -() const;
		Color operator *(const Color& other) const;
		Color& operator *=(const Color& other);
		static Color lerp(const Color& lhs, const Color& rhs, float t);
		static Color saturate(const Color& c);
		static color_gray encode_gray(const float& c);
		static color_gray encode_gray(const Color& c);
		static color_rg encode_rg(const float& r);
		static color_rg encode_rg(const float& r, const float& g);
		static color_rgb encode_rgb(const Color& c);
		static color_rgb encode_rgb(const Vector4& c);
		static color_rgb encode_rgb(const Vector3& c);
		static color_rgb encode_rgb(const Vector2& c);
		static color_bgra encode_bgra(const Color& c);
		static color_bgra encode_bgra(const Vector4& c);
		static color_bgra encode_bgra(const Vector3& c);
		static color_bgra encode_bgra(const Vector2& c);
		static color_rgba encode_rgba(const Color& c);
		static color_rgba encode_rgba(const Vector4& c);
		static color_rgba encode_rgba(const Vector3& c);
		static color_rgba encode_rgba(const Vector2& c);
		static color_rgb16f encode_rgb16f(const Color& c);
		static color_rgb16f encode_rgb16f(const Vector4& c);
		static color_rgb16f encode_rgb16f(const Vector3& c);
		static color_rgb16f encode_rgb16f(const Vector2& c);
		static color_rgba16f encode_rgba16f(const Color& c);
		static color_rgba16f encode_rgba16f(const Vector4& c);
		static color_rgba16f encode_rgba16f(const Vector3& c);
		static color_rgba16f encode_rgba16f(const Vector2& c);
		static int encode(const float& r, const float& g, const float& b, const float& alpha);
		static color_rgb encode_rgb(const float& r, const float& g, const float& b);
		static color_rgba encode_rgba(const float& r, const float& g, const float& b, const float& alpha);
		static color_rgb16f encode_rgb16f(const float& r, const float& g, const float& b);
		static color_rgba16f encode_rgba16f(const float& r, const float& g, const float& b, const float& alpha);
		static color_bgra encode_bgra(const float& r, const float& g, const float& b, const float& alpha);
		static color_bgra encode_bgra(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a);
		static Color decode(const int& c);
		static Color decode(const color_gray& c);
		static Color decode(const color_rgb& c);
		static Color decode(const color_rg& c);
		static Color decode(const color_rgba& c);
		static Color decode(const color_rgb16f& c);
		static Color decode(const color_rgba16f& c);
		static Color decode(const color_bgra& c);
		static float dot(const Color& lhs, const Color& rhs);
		static float magnitude(const Color& vector);
		static float length(const Color& lhs, const Color& rhs);
		static Color maximum(const Color& lhs, const Color& rhs);
		static Color minimum(const Color& lhs, const Color& rhs);
		static Color abs(const Color& v);
		static Color pow(const Color& c, float power);
		static Color normalize(const Color& value);
		static rapidjson::Value serialize(rapidjson::Document& doc, const Color& color);
		static Color deserialize(const rapidjson::Value& v);
		std::string str() const;

		friend static Color operator +(const float& other, const Color& c)
		{
			return c + other;
		}

		friend static Color operator -(const float& other, const Color& c)
		{
			return -c + other;
		}

		friend static Color operator *(const float& other, const Color& c)
		{
			return c * other;
		}

		friend static Color operator *(const Vector4& other, const Color& c)
		{
			return Color(other.x * c.r, other.y * c.g, other.z * c.b, other.w * c.a);
		}

		friend static Color operator *(const Color& c, const Vector4& other)
		{
			return Color(other.x * c.r, other.y * c.g, other.z * c.b, other.w * c.a);
		}

		friend static Color operator *(const Vector3& other, const Color& c)
		{
			return Color(other.x * c.r, other.y * c.g, other.z * c.b, c.a);
		}

		friend static Color operator *(const Color& c, const Vector3& other)
		{
			return Color(other.x * c.r, other.y * c.g, other.z * c.b, c.a);
		}
	};
}
#endif
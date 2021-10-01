#pragma once
#include <string>
#include "tinymath.h"
#include "Define.hpp"

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
		constexpr Color(const tinymath::vec4f& v) : r(v.x), g(v.y), b(v.z), a(v.w) {}
		constexpr Color(const tinymath::vec3f& v) : r(v.x), g(v.y), b(v.z), a(1.0f) {}
		constexpr Color(const float& x, const tinymath::vec3f& yzw) : r(x), g(yzw.x), b(yzw.y), a(yzw.z) {}
		Color(const float& r, const float& g, const tinymath::vec2f& zw);
		Color(const tinymath::vec2f& v, const float& b, const float& w);
		Color(const tinymath::vec2f& v, const tinymath::vec2f& zw);
		Color(const tinymath::vec3f& v, const float& w);
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
		static color_rgb encode_rgb(const tinymath::vec4f& c);
		static color_rgb encode_rgb(const tinymath::vec3f& c);
		static color_rgb encode_rgb(const tinymath::vec2f& c);
		static color_bgra encode_bgra(const Color& c);
		static color_bgra encode_bgra(const tinymath::vec4f& c);
		static color_bgra encode_bgra(const tinymath::vec3f& c);
		static color_bgra encode_bgra(const tinymath::vec2f& c);
		static color_rgba encode_rgba(const Color& c);
		static color_rgba encode_rgba(const tinymath::vec4f& c);
		static color_rgba encode_rgba(const tinymath::vec3f& c);
		static color_rgba encode_rgba(const tinymath::vec2f& c);
		static color_rgb16f encode_rgb16f(const Color& c);
		static color_rgb16f encode_rgb16f(const tinymath::vec4f& c);
		static color_rgb16f encode_rgb16f(const tinymath::vec3f& c);
		static color_rgb16f encode_rgb16f(const tinymath::vec2f& c);
		static color_rgba16f encode_rgba16f(const Color& c);
		static color_rgba16f encode_rgba16f(const tinymath::vec4f& c);
		static color_rgba16f encode_rgba16f(const tinymath::vec3f& c);
		static color_rgba16f encode_rgba16f(const tinymath::vec2f& c);
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

		friend static Color operator *(const tinymath::vec4f& other, const Color& c)
		{
			return Color(other.x * c.r, other.y * c.g, other.z * c.b, other.w * c.a);
		}

		friend static Color operator *(const Color& c, const tinymath::vec4f& other)
		{
			return Color(other.x * c.r, other.y * c.g, other.z * c.b, other.w * c.a);
		}

		friend static Color operator *(const tinymath::vec3f& other, const Color& c)
		{
			return Color(other.x * c.r, other.y * c.g, other.z * c.b, c.a);
		}

		friend static Color operator *(const Color& c, const tinymath::vec3f& other)
		{
			return Color(other.x * c.r, other.y * c.g, other.z * c.b, c.a);
		}
	};

#define DEFAULT_COLOR Color::encode_rgba(Color::BLACK)
#define DEFAULT_DEPTH_COLOR Color::encode_rgba(Color::WHITE)
}
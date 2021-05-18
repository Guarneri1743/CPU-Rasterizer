#include "Color.hpp"
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include "Marcos.h"

namespace Guarneri
{	
	Color::Color()
	{
		r = 0.0f; g = 0.0f; b = 0.0f; a = 1.0f;
	}

	Color::Color(const float& r)
	{
		this->r = r; this->g = r; this->b = r; this->a = r;
	}

	Color::Color(const Vector4& v)
	{
		this->r = v.x; this->g = v.y; this->b = v.z; this->a = v.w;
	}

	Color::Color(const Vector3& v)
	{
		this->r = v.x; this->g = v.y; this->b = v.z; this->a = 1.0f;
	}

	Color::Color(const float& r, const Vector3& yzw)
	{
		this->r = r; this->g = yzw.x; this->b = yzw.y; this->a = yzw.z;
	}

	Color::Color(const float& r, const float& g, const Vector2& zw)
	{
		this->r = r; this->g = g; this->b = zw.x; this->a = zw.y;
	}

	Color::Color(const Vector2& v, const float& b, const float& w)
	{
		this->r = v.x; this->g = v.y; this->b = b; this->a = w;
	}

	Color::Color(const Vector2& v, const Vector2& zw)
	{
		this->r = v.x; this->g = v.y; this->b = zw.x; this->a = zw.y;
	}

	Color::Color(const Vector3& v, const float& w)
	{
		this->r = v.x; this->g = v.y; this->b = v.z; this->a = w;
	}

	Color::Color(const float& r, const float& g, const float& b)
	{
		this->r = r; this->g = g; this->b = b; this->a = 1.0f;
	}

	Color::Color(const float& r, const float& g, const float& b, const float& a)
	{
		this->r = r; this->g = g; this->b = b; this->a = a;
	}

	Color Color::normalized()
	{
		return Color::normalize(*this);
	}

	float& Color::operator[](const uint32_t& i)
	{
		return (&r)[i];
	}

	const float& Color::operator[](const uint32_t& i) const
	{
		return (&r)[i];
	}

	bool Color::operator == (const Color& other)
	{
		return EQUALS(r, other.r) && EQUALS(g, other.g) && EQUALS(b, other.b) && EQUALS(a, other.a);
	}

	bool Color::operator != (const Color& other)
	{
		return !(*this == other);
	}

	Color& Color::operator +=(const Color& other)
	{
		r = r + other.r;
		g = g + other.g;
		b = b + other.b;
		a = a + other.a;
		return *this;
	}

	Color& Color::operator -=(const Color& other)
	{
		r = r - other.r;
		g = g - other.g;
		b = b - other.b;
		a = b - other.a;
		return *this;
	}

	Color& Color::operator /=(const float& val)
	{
		r = r / val;
		g = g / val;
		b = b / val;
		a = a / val;
		return *this;
	}

	Color& Color::operator *=(const float& val)
	{
		r = r * val;
		g = g * val;
		b = b * val;
		a = a * val;
		return *this;
	}

	Color Color::operator +(const Color& other) const
	{
		Color ret;
		ret.r = this->r + other.r;
		ret.g = this->g + other.g;
		ret.b = this->b + other.b;
		ret.a = this->a + other.a;
		return ret;
	}

	Color Color::operator -(const Color& other) const
	{
		Color ret;
		ret.r = this->r - other.r;
		ret.g = this->g - other.g;
		ret.b = this->b - other.b;
		ret.a = this->a - other.a;
		return ret;
	}

	Color Color::operator *(const float& scale) const
	{
		Color ret;
		ret.r = this->r * scale;
		ret.g = this->g * scale;
		ret.b = this->b * scale;
		ret.a = this->a * scale;
		return ret;
	}

	Color Color::operator /(const float& val) const
	{
		Color ret;
		ret.r = this->r / val;
		ret.g = this->g / val;
		ret.b = this->b / val;
		ret.a = this->a / val;
		return ret;
	}

	Color Color::operator /(const Color& val) const
	{
		Color ret;
		ret.r = this->r / val.r;
		ret.g = this->g / val.g;
		ret.b = this->b / val.b;
		ret.a = this->a / val.a;
		return ret;
	}

	Color Color::operator +(const float& other) const
	{
		Color ret;
		ret.r = this->r + other;
		ret.g = this->g + other;
		ret.b = this->b + other;
		ret.a = this->a + other;
		return ret;
	}

	Color Color::operator -(const float& other) const
	{
		Color ret;
		ret.r = this->r - other;
		ret.g = this->g - other;
		ret.b = this->b - other;
		ret.a = this->a - other;
		return ret;
	}

	Color Color::operator -() const
	{
		return Color(-r, -g, -b, -a);
	}

	Color Color::operator *(const Color& other) const
	{
		return Color(r * other.r, g * other.g, b * other.b, a * other.a);
	}

	Color& Color::operator *=(const Color& other)
	{
		r *= other.r;
		g *= other.g;
		b *= other.b;
		a *= other.a;
		return *this;
	}

	Color Color::lerp(const Color& lhs, const Color& rhs, float t)
	{
		float r = lhs.r + (rhs.r - lhs.r) * t;
		float g = lhs.g + (rhs.g - lhs.g) * t;
		float b = lhs.b + (rhs.b - lhs.b) * t;
		float w = lhs.a + (rhs.a - lhs.a) * t;
		return Color(r, g, b, w);
	}

	Color Color::saturate(const Color& c)
	{
		Color ret = c;
		ret.r = std::clamp(c.r, 0.0f, 1.0f);
		ret.g = std::clamp(c.g, 0.0f, 1.0f);
		ret.b = std::clamp(c.b, 0.0f, 1.0f);
		ret.a = std::clamp(c.a, 0.0f, 1.0f);
		return ret;
	}

	color_gray Color::encode_gray(const float& gray)
	{
		color_gray ret;
		ret.gray = std::clamp((uint8_t)(gray * 255.0f), (uint8_t)0, (uint8_t)255);
		return ret;
	}

	color_gray Color::encode_gray(const Color& c)
	{
		color_gray ret;
		ret.gray = std::clamp((uint8_t)(c.r * 255.0f), (uint8_t)0, (uint8_t)255);
		return ret;
	}

	color_rg Color::encode_rg(const float& r)
	{
		color_rg ret;
		ret.r = std::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255);
		ret.g = std::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255);
		return ret;
	}

	color_rg Color::encode_rg(const float& r, const float& g)
	{
		color_rg ret;
		ret.r = std::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255);
		ret.g = std::clamp((uint8_t)(g * 255.0f), (uint8_t)0, (uint8_t)255);
		return ret;
	}

	color_rgb Color::encode_rgb(const Color& c)
	{
		return encode_rgb(c.r, c.g, c.b);
	}

	color_rgb Color::encode_rgb(const Vector4& c)
	{
		return encode_rgb(c.x, c.y, c.z);
	}

	color_rgb Color::encode_rgb(const Vector3& c)
	{
		return encode_rgb(c.x, c.y, c.z);
	}

	color_rgb Color::encode_rgb(const Vector2& c)
	{
		return encode_rgb(c.x, c.y, 0.0f);
	}

	color_bgra Color::encode_bgra(const Color& c)
	{
		return encode_bgra(c.r, c.g, c.b, c.a);
	}

	color_bgra Color::encode_bgra(const Vector4& c)
	{
		return encode_bgra(c.x, c.y, c.z, c.w);
	}

	color_bgra Color::encode_bgra(const Vector3& c)
	{
		return encode_bgra(c.x, c.y, c.z, 1.0f);
	}

	color_bgra Color::encode_bgra(const Vector2& c)
	{
		return encode_bgra(c.x, c.y, 0.0f, 1.0f);
	}

	color_rgba Color::encode_rgba(const Color& c)
	{
		return encode_rgba(c.r, c.g, c.b, c.a);
	}

	color_rgba Color::encode_rgba(const Vector4& c)
	{
		return encode_rgba(c.x, c.y, c.z, c.w);
	}

	color_rgba Color::encode_rgba(const Vector3& c)
	{
		return encode_rgba(c.x, c.y, c.z, 1.0f);
	}

	color_rgba Color::encode_rgba(const Vector2& c)
	{
		return encode_rgba(c.x, c.y, 0.0f, 1.0f);
	}

	int Color::encode(const float& r, const float& g, const float& b, const float& alpha)
	{
		int c = 0;
		c |= (std::clamp((uint8_t)(alpha * 255.0f), (uint8_t)0, (uint8_t)255) << 24) & 0xff000000;
		c |= (std::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255) << 16) & 0xff0000;
		c |= (std::clamp((uint8_t)(g * 255.0f), (uint8_t)0, (uint8_t)255) << 8) & 0xff00;
		c |= (std::clamp((uint8_t)(b * 255.0f), (uint8_t)0, (uint8_t)255)) & 0xff;
		return c;
	}

	color_rgb Color::encode_rgb(const float& r, const float& g, const float& b)
	{
		color_rgb c;
		c.r = std::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255);
		c.g = std::clamp((uint8_t)(g * 255.0f), (uint8_t)0, (uint8_t)255);
		c.b = std::clamp((uint8_t)(b * 255.0f), (uint8_t)0, (uint8_t)255);
		return c;
	}

	color_rgba Color::encode_rgba(const float& r, const float& g, const float& b, const float& alpha)
	{
		color_rgba c;
		c.a = std::clamp((uint8_t)(alpha * 255.0f), (uint8_t)0, (uint8_t)255);
		c.r = std::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255);
		c.g = std::clamp((uint8_t)(g * 255.0f), (uint8_t)0, (uint8_t)255);
		c.b = std::clamp((uint8_t)(b * 255.0f), (uint8_t)0, (uint8_t)255);
		return c;
	}

	color_bgra Color::encode_bgra(const float& r, const float& g, const float& b, const float& alpha)
	{
		color_bgra c;
		c.a = std::clamp((uint8_t)(alpha * 255.0f), (uint8_t)0, (uint8_t)255);
		c.r = std::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255);
		c.g = std::clamp((uint8_t)(g * 255.0f), (uint8_t)0, (uint8_t)255);
		c.b = std::clamp((uint8_t)(b * 255.0f), (uint8_t)0, (uint8_t)255);
		return c;
	}

	color_bgra Color::encode_bgra(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a)
	{
		color_bgra c;
		c.a = a;
		c.r = r;
		c.g = g;
		c.b = b;
		return c;
	}

	Color Color::decode(const int& c)
	{
		int mask = 0xff;
		int a = (c >> 24) & mask;
		int r = (c >> 16) & mask;
		int g = (c >> 8) & mask;
		int b = c & mask;
		return Color((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
	}

	Color Color::decode(const color_gray& c)
	{
		return Color((float)c.gray / 255.0f, (float)c.gray / 255.0f, (float)c.gray / 255.0f, 1.0f);
	}

	Color Color::decode(const color_rg& c)
	{
		return Color((float)c.r / 255.0f, (float)c.g / 255.0f, 0.0f, 1.0f);
	}

	Color Color::decode(const color_rgb& c)
	{
		return Color((float)c.r / 255.0f, (float)c.g / 255.0f, (float)c.b / 255.0f, 1.0f);
	}

	Color Color::decode(const color_rgba& c)
	{
		return Color((float)c.r / 255.0f, (float)c.g / 255.0f, (float)c.b / 255.0f, (float)c.a / 255.0f);
	}

	Color Color::decode(const color_bgra& c)
	{
		return Color((float)c.r / 255.0f, (float)c.g / 255.0f, (float)c.b / 255.0f, (float)c.a / 255.0f);
	}

	float Color::dot(const Color& lhs, const Color& rhs)
	{
		return lhs.r * rhs.r + lhs.g * rhs.g + lhs.b * rhs.b + lhs.a * lhs.a;
	}

	float Color::magnitude(const Color& vector)
	{
		return sqrt(vector.r * vector.r + vector.g * vector.g + vector.b * vector.b + vector.a * vector.a);
	}

	float Color::length(const Color& lhs, const Color& rhs)
	{
		return magnitude(lhs - rhs);
	}

	Color Color::abs(const Color& v)
	{
		return Color(std::abs(v.r), std::abs(v.g), std::abs(v.b), std::abs(v.a));
	}

	Color Color::pow(const Color& c, float power)
	{
		return Color(std::pow(c.r, power), std::pow(c.g, power), std::pow(c.b, power), c.a);
	}

	Color Color::normalize(const Color& value)
	{
		float num = magnitude(value);
		Color retult;
		if (num > EPSILON)
		{
			retult = value / num;
		}
		else
		{
			retult = Color();
		}
		return retult;
	}

	rapidjson::Value Color::serialize(rapidjson::Document& doc, const Color& color)
	{
		rapidjson::Value v;
		v.SetObject();
		v.AddMember("r", color.r, doc.GetAllocator());
		v.AddMember("g", color.g, doc.GetAllocator());
		v.AddMember("b", color.b, doc.GetAllocator());
		v.AddMember("a", color.a, doc.GetAllocator());
		return v;
	}

	Color Color::deserialize(const rapidjson::Value& v)
	{
		Color color;
		color.r = v["r"].GetFloat();
		color.g = v["g"].GetFloat();
		color.b = v["b"].GetFloat();
		color.a = v["a"].GetFloat();
		return color;
	}

	Color Color::maximum(const Color& lhs, const Color& rhs)
	{
		return Color(std::max(lhs.r, rhs.r), std::max(lhs.g, rhs.g), std::max(lhs.b, rhs.b), std::max(lhs.a, rhs.a));
	}

	Color Color::minimum(const Color& lhs, const Color& rhs)
	{
		return Color(std::min(lhs.r, rhs.r), std::min(lhs.g, rhs.g), std::min(lhs.b, rhs.b), std::min(lhs.a, rhs.a));
	}

	std::string Color::str() const
	{
		std::stringstream ss;
		ss << "(" << std::setprecision(FLOAT_LOG_PRECISION) << this->r << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->g << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->b << ", " << this->a << ")";
		return ss.str();
	}

	const Color Color::BLACK = Color(0.0f, 0.0f, 0.0f, 1.0f);
	const Color Color::WHITE = Color(1.0f, 1.0f, 1.0f, 1.0f);
	const Color Color::RED = Color(1.0f, 0.0f, 0.0f, 1.0f);
	const Color Color::GREEN = Color(0.0f, 1.0f, 0.0f, 1.0f);
	const Color Color::BLUE = Color(0.0f, 0.0f, 1.0f, 1.0f);
}
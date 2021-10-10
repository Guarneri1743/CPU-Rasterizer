#pragma once
#include "tinymath.h"

namespace ColorEncoding
{
	TMATH_INLINE int encode(float r, float g, float b, float alpha)
	{
		int c = 0;
		c |= (tinymath::clamp((uint8_t)(alpha * 255.0f), (uint8_t)0, (uint8_t)255) << 24) & 0xff000000;
		c |= (tinymath::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255) << 16) & 0xff0000;
		c |= (tinymath::clamp((uint8_t)(g * 255.0f), (uint8_t)0, (uint8_t)255) << 8) & 0xff00;
		c |= (tinymath::clamp((uint8_t)(b * 255.0f), (uint8_t)0, (uint8_t)255)) & 0xff;
		return c;
	}

	TMATH_INLINE tinymath::color_rgb encode_rgb(float r, float g, float b)
	{
		tinymath::color_rgb c;
		c.r = tinymath::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255);
		c.g = tinymath::clamp((uint8_t)(g * 255.0f), (uint8_t)0, (uint8_t)255);
		c.b = tinymath::clamp((uint8_t)(b * 255.0f), (uint8_t)0, (uint8_t)255);
		return c;
	}

	TMATH_INLINE tinymath::color_rgba encode_rgba(float r, float g, float b, float alpha)
	{
		tinymath::color_rgba c;
		c.a = tinymath::clamp((uint8_t)(alpha * 255.0f), (uint8_t)0, (uint8_t)255);
		c.r = tinymath::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255);
		c.g = tinymath::clamp((uint8_t)(g * 255.0f), (uint8_t)0, (uint8_t)255);
		c.b = tinymath::clamp((uint8_t)(b * 255.0f), (uint8_t)0, (uint8_t)255);
		return c;
	}

	TMATH_INLINE tinymath::color_bgra encode_bgra(float r, float g, float b, float alpha)
	{
		tinymath::color_bgra c;
		c.a = tinymath::clamp((uint8_t)(alpha * 255.0f), (uint8_t)0, (uint8_t)255);
		c.r = tinymath::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255);
		c.g = tinymath::clamp((uint8_t)(g * 255.0f), (uint8_t)0, (uint8_t)255);
		c.b = tinymath::clamp((uint8_t)(b * 255.0f), (uint8_t)0, (uint8_t)255);
		return c;
	}

	TMATH_INLINE tinymath::color_bgra encode_bgra(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		tinymath::color_bgra c;
		c.a = a;
		c.r = r;
		c.g = g;
		c.b = b;
		return c;
	}

	TMATH_INLINE tinymath::color_rgb16f encode_rgb16f(float r, float g, float b)
	{
		return { r, g, b };
	}

	TMATH_INLINE tinymath::color_rgba16f encode_rgba16f(float r, float g, float b, float alpha)
	{
		return { r, g, b, alpha };
	}

	TMATH_INLINE tinymath::color_gray encode_gray(float gray)
	{
		tinymath::color_gray ret;
		ret.gray = tinymath::clamp((uint8_t)(gray * 255.0f), (uint8_t)0, (uint8_t)255);
		return ret;
	}

	TMATH_INLINE tinymath::color_gray encode_gray(const tinymath::Color& c)
	{
		tinymath::color_gray ret;
		ret.gray = tinymath::clamp((uint8_t)(c.r * 255.0f), (uint8_t)0, (uint8_t)255);
		return ret;
	}

	TMATH_INLINE tinymath::color_rg encode_rg(float r)
	{
		tinymath::color_rg ret;
		ret.r = tinymath::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255);
		ret.g = tinymath::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255);
		return ret;
	}

	TMATH_INLINE tinymath::color_rg encode_rg(float r, float g)
	{
		tinymath::color_rg ret;
		ret.r = tinymath::clamp((uint8_t)(r * 255.0f), (uint8_t)0, (uint8_t)255);
		ret.g = tinymath::clamp((uint8_t)(g * 255.0f), (uint8_t)0, (uint8_t)255);
		return ret;
	}

	TMATH_INLINE tinymath::color_rgb encode_rgb(const tinymath::Color& c)
	{
		return encode_rgb(c.r, c.g, c.b);
	}

	TMATH_INLINE tinymath::color_rgb encode_rgb(const tinymath::vec3f& c)
	{
		return encode_rgb(c.x, c.y, c.z);
	}

	TMATH_INLINE tinymath::color_rgb encode_rgb(const tinymath::vec2f& c)
	{
		return encode_rgb(c.x, c.y, 0.0f);
	}

	TMATH_INLINE tinymath::color_bgra encode_bgra(const tinymath::Color& c)
	{
		return encode_bgra(c.r, c.g, c.b, c.a);
	}

	TMATH_INLINE tinymath::color_bgra encode_bgra(const tinymath::vec3f& c)
	{
		return encode_bgra(c.x, c.y, c.z, 1.0f);
	}

	TMATH_INLINE tinymath::color_bgra encode_bgra(const tinymath::vec2f& c)
	{
		return encode_bgra(c.x, c.y, 0.0f, 1.0f);
	}

	TMATH_INLINE tinymath::color_rgba encode_rgba(const tinymath::Color& c)
	{
		return encode_rgba(c.r, c.g, c.b, c.a);
	}

	TMATH_INLINE tinymath::color_rgba encode_rgba(const tinymath::vec3f& c)
	{
		return encode_rgba(c.x, c.y, c.z, 1.0f);
	}

	TMATH_INLINE tinymath::color_rgba encode_rgba(const tinymath::vec2f& c)
	{
		return encode_rgba(c.x, c.y, 0.0f, 1.0f);
	}

	TMATH_INLINE tinymath::color_rgb16f encode_rgb16f(const tinymath::Color& c)
	{
		return encode_rgb16f(c.r, c.g, c.b);
	}

	TMATH_INLINE tinymath::color_rgb16f encode_rgb16f(const tinymath::vec3f& c)
	{
		return encode_rgb16f(c.x, c.y, c.z);
	}

	TMATH_INLINE tinymath::color_rgb16f encode_rgb16f(const tinymath::vec2f& c)
	{
		return encode_rgb16f(c.x, c.y, 0.0f);
	}

	TMATH_INLINE tinymath::color_rgba16f encode_rgba16f(const tinymath::Color& c)
	{
		return encode_rgba16f(c.r, c.g, c.b, c.a);
	}

	TMATH_INLINE tinymath::color_rgba16f encode_rgba16f(const tinymath::vec3f& c)
	{
		return encode_rgba16f(c.x, c.y, c.z, 1.0f);
	}

	TMATH_INLINE tinymath::color_rgba16f encode_rgba16f(const tinymath::vec2f& c)
	{
		return encode_rgba16f(c.x, c.y, 0.0f, 1.0f);
	}

	TMATH_INLINE tinymath::Color decode(int c)
	{
		int mask = 0xff;
		int a = (c >> 24) & mask;
		int r = (c >> 16) & mask;
		int g = (c >> 8) & mask;
		int b = c & mask;
		return tinymath::Color((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
	}

	TMATH_INLINE tinymath::Color decode(const tinymath::color_gray& c)
	{
		return tinymath::Color((float)c.gray / 255.0f, (float)c.gray / 255.0f, (float)c.gray / 255.0f, 1.0f);
	}

	TMATH_INLINE tinymath::Color decode(const tinymath::color_rg& c)
	{
		return tinymath::Color((float)c.r / 255.0f, (float)c.g / 255.0f, 0.0f, 1.0f);
	}

	TMATH_INLINE tinymath::Color decode(const tinymath::color_rgb& c)
	{
		return tinymath::Color((float)c.r / 255.0f, (float)c.g / 255.0f, (float)c.b / 255.0f, 1.0f);
	}

	TMATH_INLINE tinymath::Color decode(const tinymath::color_rgba& c)
	{
		return tinymath::Color((float)c.r / 255.0f, (float)c.g / 255.0f, (float)c.b / 255.0f, (float)c.a / 255.0f);
	}

	TMATH_INLINE tinymath::Color decode(const tinymath::color_rgb16f& c)
	{
		return tinymath::Color(c.r, c.g, c.b, 1.0f);
	}

	TMATH_INLINE tinymath::Color decode(const tinymath::color_rgba16f& c)
	{
		return tinymath::Color(c.r, c.g, c.b, c.a);
	}

	TMATH_INLINE tinymath::Color decode(const tinymath::color_bgra& c)
	{
		return tinymath::Color((float)c.r / 255.0f, (float)c.g / 255.0f, (float)c.b / 255.0f, (float)c.a / 255.0f);
	}
}
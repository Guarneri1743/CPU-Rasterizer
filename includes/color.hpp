#pragma once
#include <common.hpp>
#include <float3.hpp>
#include <float4.hpp>

namespace guarneri {

	enum class blend_factor {
		one,
		color,
		alpha,
		one_minus_color,
		one_minus_alpha
	};

	enum class blend_operator {
		add,
		sub
	};

	color_t CLAMP(color_t x, color_t min, color_t max) { return (x < min) ? min : ((x > max) ? max : x); }

	struct color {
	public:
		static const color WHITE;
		static const color BLACK;
		static const color RED;
		static const color GREEN;
		static const color BLUE;

	public:
		color() { r = 0.0f; g = 0.0f; b = 0.0f; a = 1.0f; }
		color(const float& r) { this->r = r; this->g = r; this->b = r; this->a = r; }
		color(const color& v) { this->r = v.r; this->g = v.g; this->b = v.b; this->a = v.a; }
		color(const float4& v) { this->r = v.x; this->g = v.y; this->b = v.z; this->a = v.w; }
		color(const float3& v) { this->r = v.x; this->g = v.y; this->b = v.z; this->a = 1.0f; }
		color(const float& r, const float3& yzw) { this->r = r; this->g = yzw.x; this->b = yzw.y; this->a = yzw.z; }
		color(const float& r, const float& g, const float2& zw) { this->r = r; this->g = g; this->b = zw.x; this->a = zw.y; }
		color(const float2& v, const float& b, const float& w) { this->r = v.x; this->g = v.y; this->b = b; this->a = w; }
		color(const float2& v, const float2& zw) { this->r = v.x; this->g = v.y; this->b = zw.x; this->a = zw.y; }
		color(const float3& v, const float& w) { this->r = v.x; this->g = v.y; this->b = v.z; this->a = w; }
		color(const float& r, const float& g, const float& b) { this->r = r; this->g = g; this->b = b; this->a = 1.0f; }
		color(const float& r, const float& g, const float& b, const float& w) { this->r = r; this->g = g; this->b = b; this->a = w; }

	public:
		float r;
		float g;
		float b;
		float a;

	public:
		color normalized() {
			return color::normalize(*this);
		}

		float& operator[](const unsigned int& i) { return (&r)[i]; }

		const float& operator[](const unsigned int& i) const { return (&r)[i]; }

		bool operator == (const color& other) {
			return EQUALS(r, other.r) && EQUALS(g, other.g) && EQUALS(b, other.b) && EQUALS(a, other.a);
		}

		bool operator != (const color& other) {
			return !(*this == other);
		}

		color& operator +=(const color& other) {
			r = r + other.r;
			g = g + other.g;
			b = b + other.b;
			a = a + other.a;
			return *this;
		}

		color& operator -=(const color& other) {
			r = r - other.r;
			g = g - other.g;
			b = b - other.b;
			a = b - other.a;
			return *this;
		}

		color& operator /=(const float& val) {
			r = r / val;
			g = g / val;
			b = b / val;
			a = a / val;
			return *this;
		}

		color& operator *=(const float& val) {
			r = r * val;
			g = g * val;
			b = b * val;
			a = a * val;
			return *this;
		}

		color operator +(const color& other) const {
			color ret;
			ret.r = this->r + other.r;
			ret.g = this->g + other.g;
			ret.b = this->b + other.b;
			ret.a = this->a + other.a;
			return ret;
		}

		color operator -(const color& other) const {
			color ret;
			ret.r = this->r - other.r;
			ret.g = this->g - other.g;
			ret.b = this->b - other.b;
			ret.a = this->a - other.a;
			return ret;
		}

		color operator *(const float& scale) const {
			color ret;
			ret.r = this->r * scale;
			ret.g = this->g * scale;
			ret.b = this->b * scale;
			ret.a = this->a * scale;
			return ret;
		}

		color operator /(const float& val) const {
			color ret;
			ret.r = this->r / val;
			ret.g = this->g / val;
			ret.b = this->b / val;
			ret.a = this->a / val;
			return ret;
		}

		color operator /(const color& val) const {
			color ret;
			ret.r = this->r / val.r;
			ret.g = this->g / val.g;
			ret.b = this->b / val.b;
			ret.a = this->a / val.a;
			return ret;
		}


		color operator +(const color& other) {
			color ret;
			ret.r = this->r + other.r;
			ret.g = this->g + other.g;
			ret.b = this->b + other.b;
			ret.a = this->a + other.a;
			return ret;
		}

		color operator -(const color& other) {
			color ret;
			ret.r = this->r - other.r;
			ret.g = this->g - other.g;
			ret.b = this->b - other.b;
			ret.a = this->a - other.a;
			return ret;
		}

		color operator +(const float& other) {
			color ret;
			ret.r = this->r + other;
			ret.g = this->g + other;
			ret.b = this->b + other;
			ret.a = this->a + other;
			return ret;
		}

		color operator -(const float& other) {
			color ret;
			ret.r = this->r - other;
			ret.g = this->g - other;
			ret.b = this->b - other;
			ret.a = this->a - other;
			return ret;
		}

		color operator -() const {
			return color(-r, -g, -b, -a);
		}

		color operator *(const color& other) const {
			return color(r * other.r, g * other.g, b * other.b, a * other.a);
		}

		color& operator *=(const color& other) {
			r *= other.r;
			g *= other.g;
			b *= other.b;
			a *= other.a;
			return *this;
		}

		color apply_blend_factor(blend_factor factor) {
			switch (factor) {
			case blend_factor::one:
				return *this;
			case blend_factor::alpha:
				return color(r * a, g * a, b * a);
			case blend_factor::color:
				return color(r * r, g * g, b * b);
			case blend_factor::one_minus_color:
				return color(1.0f - r, 1.0f - g, 1.0f - b);
			case blend_factor::one_minus_alpha:
				return color(1.0f - a, 1.0f - a, 1.0f - a);
			}
			return *this;
		}

		static color blend(color src_color, color dst_color, blend_factor src_factor, blend_factor dst_factor, blend_operator op) {
			color lhs = src_color.apply_blend_factor(src_factor);
			color rhs = dst_color.apply_blend_factor(dst_factor);
			switch (op) {
			case blend_operator::add:
				return lhs + rhs;
			case blend_operator::sub:
				return lhs - rhs;
			}
			return lhs + rhs;
		}

		static color_t encode(const color& c) {
			return encode(c.r, c.g, c.b, c.a);
		}

		static color_t encode(const float4& c) {
			return encode(c.x, c.y, c.z, c.w);
		}

		static color_t encode(const float3& c) {
			return encode(c.x, c.y, c.z, 1.0f);
		}

		static color_t encode(const float2& c) {
			return encode(c.x, c.y, 0.0f, 1.0f);
		}

		static color_t encode(const float& r, const float& g, const float& b, const float& alpha) {
			color_t c = 0;
			// bitmap A-R-G-B
			c |= (CLAMP((int)(alpha * 255.0f), 0, 255) << 24) & 0xff000000;
			c |= (CLAMP((int)(r * 255.0f), 0, 255) << 16) & 0xff0000;
			c |= (CLAMP((int)(g * 255.0f), 0, 255) << 8) & 0xff00;
			c |= (CLAMP((int)(b * 255.0f), 0, 255)) & 0xff;
			return c;
		}

		static color decode(const color_t& c) {
			int mask = 0xff;
			int a = (c >> 24) & mask;
			int r = (c >> 16) & mask;
			int g = (c >> 8) & mask;
			int b = c & mask;
			return color((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
		}

		static float dot(const color& lhs, const color& rhs) {
			return lhs.r * rhs.r + lhs.g * rhs.g + lhs.b * rhs.b + lhs.a * lhs.a;
		}

		static float magnitude(const color& vector) {
			return sqrt(vector.r * vector.r + vector.g * vector.g + vector.b * vector.b + vector.a * vector.a);
		}

		static float length(const color& a, const color& b) {
			return magnitude(a - b);
		}

		static color max(const color& a, const color& b) {
			return color(std::max(a.r, b.r), std::max(a.g, b.g), std::max(a.b, b.b), std::max(a.a, b.a));
		}

		static color min(const color& a, const color& b) {
			return color(std::min(a.r, b.r), std::min(a.g, b.g), std::min(a.b, b.b), std::min(a.a, b.a));
		}

		static color abs(const color& v) {
			return color(std::abs(v.r), std::abs(v.g), std::abs(v.b), std::abs(v.a));
		}

		static color normalize(const color& value) {
			float num = magnitude(value);
			color retult;
			if (num > EPSILON) {
				retult = value / num;
			}
			else {
				retult = color();
			}
			return retult;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "(" << std::setprecision(FLOAT_LOG_PRECISION) << this->r << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->g << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->b << ", " << this->a << ")";
			return ss.str();
		}
	};

	static color maxf(const color& lhs, const color& rhs) {
		return color(std::max(lhs.r, rhs.r), std::max(lhs.g, rhs.g), std::max(lhs.b, rhs.b), std::max(lhs.a, rhs.a));
	}

	static color operator +(const float& other, const color& c) {
		return c + other;
	}

	static color operator -(const float& other, const color& c) {
		return -c + other;
	}

	static color operator *(const float& val, const color& c) {
		return c * val;
	}

	static std::ostream& operator << (std::ostream& stream, const color& c) {
		stream << c.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const color& c) {
		stream << c.str();
		return stream;
	}

	static color lerp(const color& lhs, const color& rhs, float t) {
		float r = lhs.r + (rhs.r - lhs.r) * t;
		float g = lhs.g + (rhs.g - lhs.g) * t;
		float b = lhs.b + (rhs.b - lhs.b) * t;
		float w = lhs.a + (rhs.a - lhs.a) * t;
		return color(r, g, b, w);
	}

	const color color::BLACK = color(0.0f, 0.0f, 0.0f, 1.0f);
	const color color::WHITE = color(1.0f, 1.0f, 1.0f, 1.0f);
	const color color::RED = color(1.0f, 0.0f, 0.0f, 1.0f);
	const color color::GREEN = color(0.0f, 1.0f, 0.0f, 1.0f);
	const color color::BLUE = color(0.0f, 0.0f, 1.0f, 1.0f);
}
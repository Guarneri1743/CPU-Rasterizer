#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	enum class blend_factor {
		one,
		src_color,
		src_alpha,
		one_minus_src_alpha,
		one_minus_src_color,
		dst_color,
		dst_alpha,
		one_minus_dst_alpha,
		one_minus_dst_color
	};

	enum class blend_operator {
		add,
		sub
	};

	unsigned char CLAMP(unsigned char x, unsigned char min, unsigned char max) { return (x < min) ? min : ((x > max) ? max : x); }
	uint32_t CLAMP_UINT(uint32_t x, uint32_t min, uint32_t max) { return (x < min) ? min : ((x > max) ? max : x); }
	int CLAMP_INT(int x, int min, int max) { return (x < min) ? min : ((x > max) ? max : x); }
	float CLAMP_FLT(float x, float min, float max) { return (x < min) ? min : ((x > max) ? max : x); }

	static Color operator +(const float& other, const Color& c);
	static Color operator -(const float& other, const Color& c);
	static Color operator *(const float& other, const Color& c);

	struct Color {
	public:
		static const Color WHITE;
		static const Color BLACK;
		static const Color RED;
		static const Color GREEN;
		static const Color BLUE;

	public:
		Color() { r = 0.0f; g = 0.0f; b = 0.0f; a = 1.0f; }
		Color(const float& r) { this->r = r; this->g = r; this->b = r; this->a = r; }
		Color(const Vector4& v) { this->r = v.x; this->g = v.y; this->b = v.z; this->a = v.w; }
		Color(const Vector3& v) { this->r = v.x; this->g = v.y; this->b = v.z; this->a = 1.0f; }
		Color(const float& r, const Vector3& yzw) { this->r = r; this->g = yzw.x; this->b = yzw.y; this->a = yzw.z; }
		Color(const float& r, const float& g, const Vector2& zw) { this->r = r; this->g = g; this->b = zw.x; this->a = zw.y; }
		Color(const Vector2& v, const float& b, const float& w) { this->r = v.x; this->g = v.y; this->b = b; this->a = w; }
		Color(const Vector2& v, const Vector2& zw) { this->r = v.x; this->g = v.y; this->b = zw.x; this->a = zw.y; }
		Color(const Vector3& v, const float& w) { this->r = v.x; this->g = v.y; this->b = v.z; this->a = w; }
		Color(const float& r, const float& g, const float& b) { this->r = r; this->g = g; this->b = b; this->a = 1.0f; }
		Color(const float& r, const float& g, const float& b, const float& a) { this->r = r; this->g = g; this->b = b; this->a = a; }

	public:
		float r;
		float g;
		float b;
		float a;

	public:
		Color normalized() {
			return Color::normalize(*this);
		}

		float& operator[](const uint32_t& i) { return (&r)[i]; }

		const float& operator[](const uint32_t& i) const { return (&r)[i]; }

		bool operator == (const Color& other) {
			return EQUALS(r, other.r) && EQUALS(g, other.g) && EQUALS(b, other.b) && EQUALS(a, other.a);
		}

		bool operator != (const Color& other) {
			return !(*this == other);
		}

		Color& operator +=(const Color& other) {
			r = r + other.r;
			g = g + other.g;
			b = b + other.b;
			a = a + other.a;
			return *this;
		}

		Color& operator -=(const Color& other) {
			r = r - other.r;
			g = g - other.g;
			b = b - other.b;
			a = b - other.a;
			return *this;
		}

		Color& operator /=(const float& val) {
			r = r / val;
			g = g / val;
			b = b / val;
			a = a / val;
			return *this;
		}

		Color& operator *=(const float& val) {
			r = r * val;
			g = g * val;
			b = b * val;
			a = a * val;
			return *this;
		}

		Color operator +(const Color& other) const {
			Color ret;
			ret.r = this->r + other.r;
			ret.g = this->g + other.g;
			ret.b = this->b + other.b;
			ret.a = this->a + other.a;
			return ret;
		}

		Color operator -(const Color& other) const {
			Color ret;
			ret.r = this->r - other.r;
			ret.g = this->g - other.g;
			ret.b = this->b - other.b;
			ret.a = this->a - other.a;
			return ret;
		}

		Color operator *(const float& scale) const {
			Color ret;
			ret.r = this->r * scale;
			ret.g = this->g * scale;
			ret.b = this->b * scale;
			ret.a = this->a * scale;
			return ret;
		}

		Color operator /(const float& val) const {
			Color ret;
			ret.r = this->r / val;
			ret.g = this->g / val;
			ret.b = this->b / val;
			ret.a = this->a / val;
			return ret;
		}

		Color operator /(const Color& val) const {
			Color ret;
			ret.r = this->r / val.r;
			ret.g = this->g / val.g;
			ret.b = this->b / val.b;
			ret.a = this->a / val.a;
			return ret;
		}

		Color operator +(const float& other) const {
			Color ret;
			ret.r = this->r + other;
			ret.g = this->g + other;
			ret.b = this->b + other;
			ret.a = this->a + other;
			return ret;
		}

		Color operator -(const float& other) const {
			Color ret;
			ret.r = this->r - other;
			ret.g = this->g - other;
			ret.b = this->b - other;
			ret.a = this->a - other;
			return ret;
		}

		Color operator -() const {
			return Color(-r, -g, -b, -a);
		}

		Color operator *(const Color& other) const {
			return Color(r * other.r, g * other.g, b * other.b, a * other.a);
		}

		Color& operator *=(const Color& other) {
			r *= other.r;
			g *= other.g;
			b *= other.b;
			a *= other.a;
			return *this;
		}

		// todo: alpha factor
		static Color blend(Color src_color, Color dst_color, blend_factor src_factor, blend_factor dst_factor, blend_operator op) {
			Color lhs, rhs;
			switch (src_factor) {
			case blend_factor::one:
				lhs = src_color;
				break;
			case blend_factor::src_alpha:
				lhs = src_color * src_color.a;
				break;
			case blend_factor::src_color:
				lhs = src_color * src_color;
				break;
			case blend_factor::one_minus_src_alpha:
				lhs = src_color * (1.0f - src_color);
				break;
			case blend_factor::one_minus_src_color:
				lhs = src_color * (1.0f - dst_color);
				break;
			case blend_factor::dst_alpha:
				lhs = src_color * dst_color.a;
				break;
			case blend_factor::dst_color:
				lhs = src_color * dst_color;
				break;
			case blend_factor::one_minus_dst_alpha:
				lhs = src_color * (1.0f - dst_color.a);
				break;
			case blend_factor::one_minus_dst_color:
				lhs = src_color * (1.0f - dst_color);
				break;
			}

			switch (dst_factor) {
			case blend_factor::one:
				rhs = src_color;
				break;
			case blend_factor::src_alpha:
				rhs = dst_color * src_color.a;
				break;
			case blend_factor::src_color:
				rhs = dst_color * src_color;
				break;
			case blend_factor::one_minus_src_alpha:
				rhs = dst_color * (1.0f - src_color);
				break;
			case blend_factor::one_minus_src_color:
				rhs = dst_color * (1.0f - dst_color);
				break;
			case blend_factor::dst_alpha:
				rhs = dst_color * dst_color.a;
				break;
			case blend_factor::dst_color:
				rhs = dst_color * dst_color;
				break;
			case blend_factor::one_minus_dst_alpha:
				rhs = dst_color * (1.0f - dst_color.a);
				break;
			case blend_factor::one_minus_dst_color:
				rhs = dst_color * (1.0f - dst_color);
				break;
			}

			switch (op) {
			case blend_operator::add:
				return lhs + rhs;
			case blend_operator::sub:
				return lhs - rhs;
			}
			return lhs + rhs;
		}

		static Color lerp(const Color& lhs, const Color& rhs, float t) {
			float r = lhs.r + (rhs.r - lhs.r) * t;
			float g = lhs.g + (rhs.g - lhs.g) * t;
			float b = lhs.b + (rhs.b - lhs.b) * t;
			float w = lhs.a + (rhs.a - lhs.a) * t;
			return Color(r, g, b, w);
		}

		static color_rgb encode_rgb(const Color& c) {
			return encode_rgb(c.r, c.g, c.b);
		}

		static color_rgb encode_rgb(const Vector4& c) {
			return encode_rgb(c.x, c.y, c.z);
		}

		static color_rgb encode_rgb(const Vector3& c) {
			return encode_rgb(c.x, c.y, c.z);
		}

		static color_rgb encode_rgb(const Vector2& c) {
			return encode_rgb(c.x, c.y, 0.0f);
		}

		static color_bgra encode_bgra(const Color& c) {
			return encode_bgra(c.r, c.g, c.b, c.a);
		}

		static color_bgra encode_bgra(const Vector4& c) {
			return encode_bgra(c.x, c.y, c.z, c.w);
		}

		static color_bgra encode_bgra(const Vector3& c) {
			return encode_bgra(c.x, c.y, c.z, 1.0f);
		}

		static color_bgra encode_bgra(const Vector2& c) {
			return encode_bgra(c.x, c.y, 0.0f, 1.0f);
		}

		static color_rgba encode_rgba(const Color& c) {
			return encode_rgba(c.r, c.g, c.b, c.a);
		}

		static color_rgba encode_rgba(const Vector4& c) {
			return encode_rgba(c.x, c.y, c.z, c.w);
		}

		static color_rgba encode_rgba(const Vector3& c) {
			return encode_rgba(c.x, c.y, c.z, 1.0f);
		}

		static color_rgba encode_rgba(const Vector2& c) {
			return encode_rgba(c.x, c.y, 0.0f, 1.0f);
		}

		static int encode(const float& r, const float& g, const float& b, const float& alpha) {
			int c = 0;
			c |= (CLAMP((unsigned char)(alpha * 255.0f), 0, 255) << 24) & 0xff000000;
			c |= (CLAMP((unsigned char)(r * 255.0f), 0, 255) << 16) & 0xff0000;
			c |= (CLAMP((unsigned char)(g * 255.0f), 0, 255) << 8) & 0xff00;
			c |= (CLAMP((unsigned char)(b * 255.0f), 0, 255)) & 0xff;
			return c;
		}

		static color_rgb encode_rgb(const float& r, const float& g, const float& b) {
			color_rgb c;
			c.r = CLAMP((unsigned char)(r * 255.0f), 0, 255);
			c.g = CLAMP((unsigned char)(g * 255.0f), 0, 255);
			c.b = CLAMP((unsigned char)(b * 255.0f), 0, 255);
			return c;
		}

		static color_rgba encode_rgba(const float& r, const float& g, const float& b, const float& alpha) {
			color_rgba c;
			c.a = CLAMP((unsigned char)(alpha * 255.0f), 0, 255);
			c.r = CLAMP((unsigned char)(r * 255.0f), 0, 255);
			c.g = CLAMP((unsigned char)(g * 255.0f), 0, 255);
			c.b = CLAMP((unsigned char)(b * 255.0f), 0, 255);
			return c;
		}

		static color_bgra encode_bgra(const float& r, const float& g, const float& b, const float& alpha) {
			color_bgra c;
			c.a = CLAMP((unsigned char)(alpha * 255.0f), 0, 255);
			c.r = CLAMP((unsigned char)(r * 255.0f), 0, 255);
			c.g = CLAMP((unsigned char)(g * 255.0f), 0, 255);
			c.b = CLAMP((unsigned char)(b * 255.0f), 0, 255);
			return c;
		}

		static Color decode(const int& c) {
			int mask = 0xff;
			int a = (c >> 24) & mask;
			int r = (c >> 16) & mask;
			int g = (c >> 8) & mask;
			int b = c & mask;
			return Color((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
		}

		static Color decode(const color_rgb& c) {
			return Color((float)c.r / 255.0f, (float)c.g / 255.0f, (float)c.b / 255.0f, 255.0f);
		}

		static Color decode(const color_rgba& c) {
			return Color((float)c.r / 255.0f, (float)c.g / 255.0f, (float)c.b / 255.0f, (float)c.a / 255.0f);
		}

		static Color decode(const color_bgra& c) {
			return Color((float)c.r / 255.0f, (float)c.g / 255.0f, (float)c.b / 255.0f, (float)c.a / 255.0f);
		}

		static float dot(const Color& lhs, const Color& rhs) {
			return lhs.r * rhs.r + lhs.g * rhs.g + lhs.b * rhs.b + lhs.a * lhs.a;
		}

		static float magnitude(const Color& vector) {
			return sqrt(vector.r * vector.r + vector.g * vector.g + vector.b * vector.b + vector.a * vector.a);
		}

		static float length(const Color& a, const Color& b) {
			return magnitude(a - b);
		}

		static Color max(const Color& a, const Color& b) {
			return Color(std::max(a.r, b.r), std::max(a.g, b.g), std::max(a.b, b.b), std::max(a.a, b.a));
		}

		static Color min(const Color& a, const Color& b) {
			return Color(std::min(a.r, b.r), std::min(a.g, b.g), std::min(a.b, b.b), std::min(a.a, b.a));
		}

		static Color abs(const Color& v) {
			return Color(std::abs(v.r), std::abs(v.g), std::abs(v.b), std::abs(v.a));
		}

		static Color normalize(const Color& value) {
			float num = magnitude(value);
			Color retult;
			if (num > EPSILON) {
				retult = value / num;
			}
			else {
				retult = Color();
			}
			return retult;
		}

		std::string str() const {
			std::stringstream ss;
			ss << "(" << std::setprecision(FLOAT_LOG_PRECISION) << this->r << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->g << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->b << ", " << this->a << ")";
			return ss.str();
		}
	};

	static Color maxf(const Color& lhs, const Color& rhs) {
		return Color(std::max(lhs.r, rhs.r), std::max(lhs.g, rhs.g), std::max(lhs.b, rhs.b), std::max(lhs.a, rhs.a));
	}

	static Color operator +(const float& other, const Color& c) {
		return c + other;
	}

	static Color operator -(const float& other, const Color& c) {
		return -c + other;
	}

	static Color operator *(const float& other, const Color& c) {
		return c * other;
	}

	static Color operator *(const Vector4& other, const Color& c) {
		return Color(other.x * c.r, other.y * c.g, other.z * c.b, other.w * c.a);
	}

	static Color operator *(const Color& c, const Vector4& other) {
		return Color(other.x * c.r, other.y * c.g, other.z * c.b, other.w * c.a);
	}

	static Color operator *(const Vector3& other, const Color& c) {
		return Color(other.x * c.r, other.y * c.g, other.z * c.b, c.a);
	}

	static Color operator *(const Color& c, const Vector3& other) {
		return Color(other.x * c.r, other.y * c.g, other.z * c.b, c.a);
	}

	static std::ostream& operator << (std::ostream& stream, const Color& c) {
		stream << c.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Color& c) {
		stream << c.str();
		return stream;
	}

	const Color Color::BLACK = Color(0.0f, 0.0f, 0.0f, 1.0f);
	const Color Color::WHITE = Color(1.0f, 1.0f, 1.0f, 1.0f);
	const Color Color::RED = Color(1.0f, 0.0f, 0.0f, 1.0f);
	const Color Color::GREEN = Color(0.0f, 1.0f, 0.0f, 1.0f);
	const Color Color::BLUE = Color(0.0f, 0.0f, 1.0f, 1.0f);
}
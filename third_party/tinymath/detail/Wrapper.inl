#include <algorithm>
#include <cmath>

TINYMATH_NAMESPACE

template<typename T>
T max(const T& lhs, const T& rhs)
{
	return lhs < rhs ? rhs : lhs;
}

template<typename T>
T min(const T& lhs, const T& rhs)
{
	return lhs > rhs ? rhs : lhs;
}

template<typename T>
T abs(const T& val)
{
	return(val < (T)0 ? -val : val);
}

float sqrt(const float& x)
{
	return std::sqrt(x);
}

template<typename T>
T clamp(const T& val, const T& min_val, const T& max_val)
{
	return val < min_val ? min_val : (val > max_val ? max_val : val);
}

template<typename T>
int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

template<typename T>
void swap(T& a, T& b)
{
	std::swap(a, b);
}

float sin(const float& rad)
{
	return ::std::sin(rad);
}

float asin(const float& val)
{
	return ::std::asin(val);
}

float cos(const float& rad)
{
	return ::std::cos(rad);
}

float acos(const float& val)
{
	return ::std::acos(val);
}

float tan(const float& rad)
{
	return ::std::tan(rad);
}

float atan(const float& rad)
{
	return ::std::atan(rad);
}

float atan2(const float& y, const float& x)
{
	return ::std::atan2(y, x);
}

float cot(const float& rad)
{
	return 1.0f / tan(rad);
}

float floor(const float& val)
{
	return ::std::floorf(val);
}

float ceil(const float& val)
{
	return ::std::ceilf(val);
}

float exp(const float& val)
{
	return ::std::expf(val);
}

float exp2(const float& val)
{
	return ::std::exp2f(val);
}

float log(const float& val)
{
	return ::std::logf(val);
}

float log2(const float& val)
{
	return ::std::log2f(val);
}

float log10(const float& val)
{
	return ::std::log10f(val);
}

float pow(const float& val, const float& power)
{
	return ::std::powf(val, power);
}

float modf(const float& val, float* integer)
{
	return ::std::modff(val, integer);
}

float trunc(const float& val)
{
	return ::std::truncf(val);
}

float lerp(const float& lhs, const float& rhs, const float& t)
{
	return lhs + (rhs - lhs) * t;
}

bool approx(const float& lhs, const float& rhs)
{
	return abs(lhs - rhs) <= EPSILON;
}

END_NAMESPACE
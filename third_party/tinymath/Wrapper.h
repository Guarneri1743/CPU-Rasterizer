#pragma once
#include "MathDefine.h"

TINYMATH_NAMESPACE

TMATH_INLINE float sin(const float& rad);
TMATH_INLINE float asin(const float& val);
TMATH_INLINE float cos(const float& rad);
TMATH_INLINE float acos(const float& val);
TMATH_INLINE float tan(const float& rad);
TMATH_INLINE float atan(const float& rad);
TMATH_INLINE float atan2(const float& y, const float& x);
TMATH_INLINE float cot(const float& rad);
TMATH_INLINE float floor(const float& val);
TMATH_INLINE float ceil(const float& val);
TMATH_INLINE float exp(const float& val);
TMATH_INLINE float exp2(const float& val);
TMATH_INLINE float log(const float& val);
TMATH_INLINE float log2(const float& val);
TMATH_INLINE float log10(const float& val);
TMATH_INLINE float pow(const float& val, const float& power);
TMATH_INLINE float modf(const float& val, float* integer);
TMATH_INLINE float trunc(const float& val);
TMATH_INLINE float lerp(const float& lhs, const float& rhs, const float& t);
TMATH_INLINE bool approx(const float& lhs, const float& rhs); 
TMATH_INLINE bool is_sqrt_integer(const int& num);

template<typename T>
TMATH_INLINE T round_up(const T& num, const T& alignment);
template<typename T>
TMATH_INLINE T round_up_to_power_of_two(const T& num);
template<typename T>
TMATH_INLINE T max(const T& lhs, const T& rhs);
template<typename T>
TMATH_INLINE T min(const T& lhs, const T& rhs);
template<typename T>
TMATH_INLINE T abs(const T& val); 
template<typename T>
TMATH_INLINE T sqrt(const T& val);
template<typename T>
TMATH_INLINE T clamp(const T& val, const T& min_val, const T& max_val);
template <typename T>
TMATH_INLINE int sgn(T val);
template<typename T>
TMATH_INLINE void swap(T& a, T& b);

END_NAMESPACE

#include "detail/Wrapper.inl"
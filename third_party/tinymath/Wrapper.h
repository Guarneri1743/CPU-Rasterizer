#pragma once
#include "MathDefine.h"

TINYMATH_NAMESPACE

TMATH_INLINE float sin(float rad);
TMATH_INLINE float asin(float val);
TMATH_INLINE float cos(float rad);
TMATH_INLINE float acos(float val);
TMATH_INLINE float tan(float rad);
TMATH_INLINE float atan(float rad);
TMATH_INLINE float atan2(float y, float x);
TMATH_INLINE float cot(float rad);
TMATH_INLINE float floor(float val);
TMATH_INLINE float ceil(float val);
TMATH_INLINE float exp(float val);
TMATH_INLINE float exp2(float val);
TMATH_INLINE float log(float val);
TMATH_INLINE float log2(float val);
TMATH_INLINE float log10(float val);
TMATH_INLINE float pow(float val, float power);
TMATH_INLINE float modf(float val, float* integer);
TMATH_INLINE float trunc(float val);
TMATH_INLINE float lerp(float lhs, float rhs, float t);
TMATH_INLINE bool approx(float lhs, float rhs); 
TMATH_INLINE bool is_sqrt_integer(int num);

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
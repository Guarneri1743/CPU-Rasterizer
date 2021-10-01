#pragma once
#include "MathDefine.h"
#include "Wrapper.h"

TINYMATH_NAMESPACE

template <typename Component, size_t N> struct Vector;

// component wwise functions
#define COMPONENT_WWISE_FUNC_DECLARE(func)\
template<typename Component, size_t N>\
Vector<Component, N> func(const Vector<Component, N>& v);

#define COMPONENT_WWISE_FUNC_2_DECLARE(func)\
template<typename Component, size_t N>\
Vector<Component, N> func(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

#define COMPONENT_WWISE_FUNC_COMP_DECLARE(func)\
template<typename Component, size_t N>\
Vector<Component, N> func(const Vector<Component, N>& lhs, const Component& rhs);

COMPONENT_WWISE_FUNC_DECLARE(sgn);
COMPONENT_WWISE_FUNC_DECLARE(sin);
COMPONENT_WWISE_FUNC_DECLARE(asin);
COMPONENT_WWISE_FUNC_DECLARE(cos);
COMPONENT_WWISE_FUNC_DECLARE(acos);
COMPONENT_WWISE_FUNC_DECLARE(tan);
COMPONENT_WWISE_FUNC_DECLARE(floor);
COMPONENT_WWISE_FUNC_DECLARE(ceil);
COMPONENT_WWISE_FUNC_DECLARE(abs);
COMPONENT_WWISE_FUNC_DECLARE(exp);
COMPONENT_WWISE_FUNC_DECLARE(exp2);
COMPONENT_WWISE_FUNC_DECLARE(trunc);
COMPONENT_WWISE_FUNC_DECLARE(sqrt);
COMPONENT_WWISE_FUNC_DECLARE(log);
COMPONENT_WWISE_FUNC_DECLARE(log10);
COMPONENT_WWISE_FUNC_DECLARE(log2);
COMPONENT_WWISE_FUNC_2_DECLARE(pow); 
COMPONENT_WWISE_FUNC_COMP_DECLARE(pow);

template<typename Component, size_t N>
TMATH_INLINE bool operator!=(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE bool operator==(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator-(const Vector<Component, N>& v);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator+(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator-(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator*(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator/(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator+(const Vector<Component, N>& lhs, const Component& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator-(const Vector<Component, N>& lhs, const Component& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator*(const Vector<Component, N>& lhs, const Component& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator/(const Vector<Component, N>& lhs, const Component& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator+(const Component& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator-(const Component& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator*(const Component& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator/(const Component& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N>& operator+=(Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N>& operator-=(Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N>& operator*=(Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N>& operator/=(Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator+=(Vector<Component, N>& lhs, const Component& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator-=(Vector<Component, N>& lhs, const Component& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator*=(Vector<Component, N>& lhs, const Component& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> operator/=(Vector<Component, N>& lhs, const Component& rhs);

template <typename Component> struct Vector<Component, 2>;
template <typename Component> struct Vector<Component, 3>;
template <typename Component> struct Vector<Component, 4>;

template<typename Component, size_t N>
TMATH_INLINE Component dot(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component>
TMATH_INLINE Vector<Component, 3> cross(const Vector<Component, 3>& lhs, const Vector<Component, 3>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Component magnitude(const Vector<Component, N>& vector);

template<typename Component, size_t N>
TMATH_INLINE Component length(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> max(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> min(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> clamp(const Vector<Component, N>& val, const Vector<Component, N>& min, const Vector<Component, N>& max);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> clamp(const Vector<Component, N>& val, const Component& min, const Component& max);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> normalize(const Vector<Component, N>& value);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> lerp(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs, float t);

template<typename Component, size_t N>
TMATH_INLINE Vector<Component, N> saturate(const Vector<Component, N>& v);

template<typename Component, size_t N>
TMATH_INLINE bool approx(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs);

template<typename Component>
TMATH_INLINE void calculate_right_up(const Vector<Component, 3>& forward, Vector<Component, 3>& right, Vector<Component, 3>& up);

END_NAMESPACE

#include "detail/Vector.inl"
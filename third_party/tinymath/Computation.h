#pragma once
#include "MathDefine.h"
#include "Matrix.h"
#include "Vector.h"
#include "color/Color.h"
#include "color/ColorEncoding.h"

TINYMATH_NAMESPACE

// Matrix computation
template<typename Component, size_t ROW, size_t N>
TMATH_INLINE Vector<Component, N> operator*(const Matrix<Component, ROW, N>& m, const Vector<Component, N>& v);

template<typename Component>
TMATH_INLINE Vector<Component, 3> transform_point(const Matrix<Component, 4, 4>& m, const Vector<Component, 3>& point);

template<typename Component>
TMATH_INLINE Vector<Component, 3> transform_vector(const Matrix<Component, 4, 4>& m, const Vector<Component, 3>& vec);

// Color computation
#define COLOR_COMPUTATION_FUNC(channels, op)\
inline static color_##channels operator##op(const color_##channels& a, const color_##channels& b)\
{\
	return ColorEncoding::encode_##channels(ColorEncoding::decode(a) op ColorEncoding::decode(b));\
}\
\
inline static color_##channels operator##op(const color_##channels& a, const float& b)\
{\
	return ColorEncoding::encode_##channels(ColorEncoding::decode(a) op b);\
}

COLOR_COMPUTATION_FUNC(gray, +)
COLOR_COMPUTATION_FUNC(gray, -)
COLOR_COMPUTATION_FUNC(gray, *)
COLOR_COMPUTATION_FUNC(gray, / )

COLOR_COMPUTATION_FUNC(rg, +)
COLOR_COMPUTATION_FUNC(rg, -)
COLOR_COMPUTATION_FUNC(rg, *)
COLOR_COMPUTATION_FUNC(rg, / )

COLOR_COMPUTATION_FUNC(rgb, +)
COLOR_COMPUTATION_FUNC(rgb, -)
COLOR_COMPUTATION_FUNC(rgb, *)
COLOR_COMPUTATION_FUNC(rgb, /)

COLOR_COMPUTATION_FUNC(rgba, +)
COLOR_COMPUTATION_FUNC(rgba, -)
COLOR_COMPUTATION_FUNC(rgba, *)
COLOR_COMPUTATION_FUNC(rgba, /)

COLOR_COMPUTATION_FUNC(rgb16f, +)
COLOR_COMPUTATION_FUNC(rgb16f, -)
COLOR_COMPUTATION_FUNC(rgb16f, *)
COLOR_COMPUTATION_FUNC(rgb16f, /)

COLOR_COMPUTATION_FUNC(rgba16f, +)
COLOR_COMPUTATION_FUNC(rgba16f, -)
COLOR_COMPUTATION_FUNC(rgba16f, *)
COLOR_COMPUTATION_FUNC(rgba16f, /)

END_NAMESPACE

#include "detail/Computation.inl"
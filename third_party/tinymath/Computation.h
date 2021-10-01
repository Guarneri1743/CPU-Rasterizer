#pragma once
#include "MathDefine.h"
#include "Matrix.h"
#include "Vector.h"

TINYMATH_NAMESPACE

template<typename Component, size_t ROW, size_t N>
TMATH_INLINE Vector<Component, N> operator*(const Matrix<Component, ROW, N>& m, const Vector<Component, N>& v);

template<typename Component>
TMATH_INLINE Vector<Component, 3> transform_point(const Matrix<Component, 4, 4>& m, const Vector<Component, 3>& point);

template<typename Component>
TMATH_INLINE Vector<Component, 3> transform_vector(const Matrix<Component, 4, 4>& m, const Vector<Component, 3>& vec);

END_NAMESPACE

#include "detail/Computation.inl"
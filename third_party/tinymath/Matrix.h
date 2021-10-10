#pragma once
#include "MathDefine.h"
#include "Vector.h"

TINYMATH_NAMESPACE

template <typename Component, size_t ROW, size_t COL> 
struct Matrix
{
	Component data[ROW * COL];
	Vector<Component, 3> forward() const;
	Vector<Component, 3> up() const;
	Vector<Component, 3> right() const;
	Vector<Component, COL> row(int index) const;
	Vector<Component, ROW> column(int index) const;
	Component& at(size_t r, size_t c);
	const Component& at(size_t r, size_t c) const;
};

template<typename Component, size_t ROW, size_t COL>
bool is_rotation_matrix(const Matrix<Component, ROW, COL>& m);

template<typename Component, size_t ROW, size_t COL>
TMATH_INLINE bool operator!=(const Matrix<Component, ROW, COL>& lhs, const Matrix<Component, ROW, COL>& rhs);

template<typename Component, size_t ROW, size_t COL>
TMATH_INLINE bool operator==(const Matrix<Component, ROW, COL>& lhs, const Matrix<Component, ROW, COL>& rhs);

template<typename Component, size_t ROW, size_t COL>
TMATH_INLINE Matrix<Component, ROW, COL> operator-(const Matrix<Component, ROW, COL>& v);

template<typename Component, size_t ROW, size_t COL>
TMATH_INLINE Matrix<Component, ROW, COL> operator+(const Matrix<Component, ROW, COL>& lhs, const Matrix<Component, ROW, COL>& rhs);

template<typename Component, size_t ROW, size_t COL>
TMATH_INLINE Matrix<Component, ROW, COL> operator-(const Matrix<Component, ROW, COL>& lhs, const Matrix<Component, ROW, COL>& rhs);

template<typename Component, size_t ROW, size_t N, size_t COL>
TMATH_INLINE Matrix<Component, ROW, COL> operator*(const Matrix<Component, ROW, N>& lhs, const Matrix<Component, N, COL>& rhs);

template <typename Component> struct Matrix<Component, 3, 3>;
template <typename Component> struct Matrix<Component, 4, 4>;

template<typename Component>
TMATH_INLINE Component determinant(const Matrix<Component, 3, 3>& m);

template<typename Component>
TMATH_INLINE Component determinant(const Matrix<Component, 4, 4>& m);

template<typename Component>
TMATH_INLINE Matrix<Component, 3, 3> inverse(const Matrix<Component, 3, 3>& m);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> inverse(const Matrix<Component, 4, 4>& m);

template<typename Component, size_t ROW, size_t COL>
TMATH_INLINE Matrix<Component, ROW, COL> transpose(const Matrix<Component, ROW, COL>& m);

template<typename Component, size_t ROW, size_t COL>
TMATH_INLINE bool approx(const Matrix<Component, ROW, COL>& lhs, const Matrix<Component, ROW, COL>& rhs);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> translation(const Vector<Component, 3>& translation);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> rotation(const Vector<Component, 3>& axis, const Component& theta);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> scale(const Vector<Component, 3>& scale);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> viewport(int x, int y, int w, int h);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> from_euler(const Vector<Component, 3>& euler);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> from_angle(const Vector<Component, 3>& euler);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> euler_x(const Component& rad);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> euler_y(const Component& rad);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> euler_z(const Component& rad);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> lookat(const Vector<Component, 3>& eye, const Vector<Component, 3>& target, const Vector<Component, 3>& world_up);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> lookat_lh(const Vector<Component, 3>& eye, const Vector<Component, 3>& target, const Vector<Component, 3>& world_up);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> lookat_rh(const Vector<Component, 3>& eye, const Vector<Component, 3>& target, const Vector<Component, 3>& world_up);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> perspective(const Component& fov, const Component& aspect, const Component& near, const Component& far);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> perspective_rh_dx(const Component& fov, const Component& aspect, const Component& near, const Component& far);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> perspective_lh_dx(const Component& fov, const Component& aspect, const Component& near, const Component& far);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> perspective_rh_gl(const Component& fov, const Component& aspect, const Component& near, const Component& far);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> perspective_lh_gl(const Component& fov, const Component& aspect, const Component& near, const Component& far);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> ortho(const Component& left, const Component& right, const Component& bottom, const Component& top, const Component& near, const Component& far);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> ortho_rh_dx(const Component& left, const Component& right, const Component& bottom, const Component& top, const Component& near, const Component& far);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> ortho_lh_dx(const Component& left, const Component& right, const Component& bottom, const Component& top, const Component& near, const Component& far);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> ortho_rh_gl(const Component& left, const Component& right, const Component& bottom, const Component& top, const Component& near, const Component& far);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> ortho_lh_gl(const Component& left, const Component& right, const Component& bottom, const Component& top, const Component& near, const Component& far);

template<typename Component>
TMATH_INLINE Matrix<Component, 3, 3> mat4x4_to_mat3x3(const Matrix<Component, 4, 4>& mat);

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> mat3x3_to_mat4x4(const Matrix<Component, 3, 3>& mat);

END_NAMESPACE

#include "detail/Matrix.inl"
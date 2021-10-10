#include <assert.h>

TINYMATH_NAMESPACE

#define rc2index(row, col, COL) row * COL + col

template<typename Component, size_t ROW, size_t COL>
bool operator!=(const Matrix<Component, ROW, COL>& lhs, const Matrix<Component, ROW, COL>& rhs)
{
	for (size_t idx = 0; idx < ROW * COL; ++idx) { if (lhs[idx] != rhs[idx]) return true; } return false;
}

template<typename Component, size_t ROW, size_t COL>
bool operator==(const Matrix<Component, ROW, COL>& lhs, const Matrix<Component, ROW, COL>& rhs)
{
	for (size_t idx = 0; idx < ROW * COL; ++idx) { if (lhs[idx] != rhs[idx]) return false; } return true;
}

template<typename Component, size_t ROW, size_t COL>
Matrix<Component, ROW, COL> operator-(const Matrix<Component, ROW, COL>& v)
{
	Matrix<Component, ROW, COL> ret = Matrix<Component, ROW, COL>();

	for (size_t r = 0; r < ROW; ++r)
	{
		for (size_t c = 0; c < COL; ++c)
		{
			ret.at(r, c) = -v.at(r, c);
		}
	}

	return ret;
}

template<typename Component, size_t ROW, size_t COL>
Matrix<Component, ROW, COL> operator+(const Matrix<Component, ROW, COL>& lhs, const Matrix<Component, ROW, COL>& rhs)
{
	Matrix<Component, ROW, COL> ret = Matrix<Component, ROW, COL>();

	for (size_t r = 0; r < ROW; ++r)
	{
		for (size_t c = 0; c < COL; ++c)
		{
			ret.at(r, c) = lhs.at(r, c) + rhs.at(r, c);
		}
	}

	return ret;
}

template<typename Component, size_t ROW, size_t COL>
Matrix<Component, ROW, COL> operator-(const Matrix<Component, ROW, COL>& lhs, const Matrix<Component, ROW, COL>& rhs)
{
	Matrix<Component, ROW, COL> ret = Matrix<Component, ROW, COL>();

	for (size_t r = 0; r < ROW; ++r)
	{
		for (size_t c = 0; c < COL; ++c)
		{
			ret.at(r, c) = lhs.at(r, c) - rhs.at(r, c);
		}
	}

	return ret;
}

template<typename Component, size_t ROW, size_t COL>
Matrix<Component, ROW, COL> transpose(const Matrix<Component, ROW, COL>& m)
{
	Matrix<Component, ROW, COL> ret = Matrix<Component, ROW, COL>();

	for (size_t r = 0; r < ROW; ++r)
	{
		for (size_t c = 0; c < COL; ++c)
		{
			ret.at(c, r) = m.at(r, c);
		}
	}

	return ret;
}

template<typename Component, size_t ROW, size_t COL>
TMATH_INLINE bool approx(const Matrix<Component, ROW, COL>& lhs, const Matrix<Component, ROW, COL>& rhs)
{
	for (size_t idx = 0; idx < ROW * COL; ++idx) { if (abs(lhs[idx] - rhs[idx]) > EPSILON) return false; } return true;
}

template<typename Component, size_t N>
Matrix<Component, N, N> create_identity()
{
	Matrix<Component, N, N> ret = Matrix<Component, N, N>((Component)0);

	for (size_t r = 0; r < N; ++r)
	{
		ret.at(r, r) = (Component)1;
	}

	return ret;
}

template<typename Component, size_t ROW, size_t COL>
bool is_rotation_matrix(const Matrix<Component, ROW, COL>& m)
{
	Matrix<Component, ROW, COL> t = transpose(m);
	Matrix<Component, ROW, COL> multiply = m * t;
	return approx(multiply, create_identity<Component, ROW>());
}

// suppress c4201
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4201)
#endif
template <typename Component>
struct Matrix<Component, 3, 3>
{
	union
	{
		Component data[9];
		struct
		{
			Component m00;
			Component m01;
			Component m02;
			Component m10;
			Component m11;
			Component m12;
			Component m20;
			Component m21;
			Component m22;
		};
	};

	Matrix<Component, 3, 3>() {}

	constexpr Matrix<Component, 3, 3>(const Component& v) :
		m00(v), m01(v), m02(v),
		m10(v), m11(v), m12(v),
		m20(v), m21(v), m22(v)
	{}

	constexpr Matrix<Component, 3, 3>(const Vector<Component, 3>& row0, const Vector<Component, 3>& row1, const Vector<Component, 3>& row2) :
		m00(row0.x), m01(row0.y), m02(row0.z),
		m10(row1.x), m11(row1.y), m12(row1.z),
		m20(row2.x), m21(row2.y), m22(row2.z)
	{}

	constexpr Matrix<Component, 3, 3>(float _m00, float _m01, float _m02,
									  float _m10, float _m11, float _m12,
									  float _m20, float _m21, float _m22) :
		m00(_m00), m01(_m01), m02(_m02),
		m10(_m10), m11(_m11), m12(_m12),
		m20(_m20), m21(_m21), m22(_m22)
	{}

	constexpr Matrix<Component, 3, 3>(const Matrix<Component, 3, 3>& m) :
		m00(m.m00), m01(m.m01), m02(m.m02),
		m10(m.m10), m11(m.m11), m12(m.m12),
		m20(m.m20), m21(m.m21), m22(m.m22)
	{}

	Component& operator[](size_t index) { return data[index]; }
	const Component& operator[](size_t index) const { return data[index]; }

	Component& at(size_t r, size_t c) { return data[rc2index(r, c, 3)]; }
	const Component& at(size_t r, size_t c) const { return data[rc2index(r, c, 3)]; }

	Vector<Component, 3> forward() const
	{
		Vector<Component, 3> f;
		f.x = at(2, 0);
		f.y = at(2, 1);
		f.z = at(2, 2);
		return normaliz(f);
	}

	Vector<Component, 3> up() const
	{
		Vector<Component, 3> u;
		u.x = at(1, 0);
		u.y = at(1, 1);
		u.z = at(1, 2);
		return normaliz(u);
	}

	Vector<Component, 3> right() const
	{
		Vector<Component, 3> r;
		r.x = at(0, 0);
		r.y = at(0, 1);
		r.z = at(0, 2);
		return normaliz(u);
	}

	Vector<Component, 3> row(int index) const
	{
		assert(index >= 0 && index <= 2);
		Vector<Component, 3> ret;
		switch (index)
		{
		case 0:
			ret = Vector<Component, 3>(m00, m01, m02);
			break;
		case 1:
			ret = Vector<Component, 3>(m10, m11, m12);
			break;
		case 2:
			ret = Vector<Component, 3>(m20, m21, m22);
			break;
		}
		return ret;
	}

	Vector<Component, 3> column(int index) const
	{
		assert(index >= 0 && index <= 2);
		Vector<Component, 3> ret;
		switch (index)
		{
		case 0:
			ret = Vector<Component, 3>(m00, m10, m20);
			break;
		case 1:
			ret = Vector<Component, 3>(m01, m11, m21);
			break;
		case 2:
			ret = Vector<Component, 3>(m02, m12, m22);
			break;
		}
		return ret;
	}
};

template <typename Component>
struct Matrix<Component, 4, 4>
{
	union
	{
		Component data[16];
		struct
		{
			Component m00;
			Component m01;
			Component m02;
			Component m03;
			Component m10;
			Component m11;
			Component m12;
			Component m13;
			Component m20;
			Component m21;
			Component m22;
			Component m23;
			Component m30;
			Component m31;
			Component m32;
			Component m33;
		};
	};

	Matrix<Component, 4, 4>() {}

	constexpr Matrix<Component, 4, 4>(const Component& v) :
		m00(v), m01(v), m02(v), m03(v),
		m10(v), m11(v), m12(v), m13(v),
		m20(v), m21(v), m22(v), m23(v),
		m30(v), m31(v), m32(v), m33(v)
	{}

	constexpr Matrix<Component, 4, 4>(const Vector<Component, 4>& row0, const Vector<Component, 4>& row1, const Vector<Component, 4>& row2, const Vector<Component, 4>& row3) :
		m00(row0.x), m01(row0.y), m02(row0.z), m03(row0.w),
		m10(row1.x), m11(row1.y), m12(row1.z), m13(row1.w),
		m20(row2.x), m21(row2.y), m22(row2.z), m23(row2.w),
		m30(row3.x), m31(row3.y), m32(row3.z), m33(row3.w)
	{}

	constexpr Matrix<Component, 4, 4>(float _m00, float _m01, float _m02, float _m03,
									  float _m10, float _m11, float _m12, float _m13,
									  float _m20, float _m21, float _m22, float _m23,
									  float _m30, float _m31, float _m32, float _m33) :
		m00(_m00), m01(_m01), m02(_m02), m03(_m03),
		m10(_m10), m11(_m11), m12(_m12), m13(_m13),
		m20(_m20), m21(_m21), m22(_m22), m23(_m23),
		m30(_m30), m31(_m31), m32(_m32), m33(_m33)
	{}

	constexpr Matrix<Component, 4, 4>(const Matrix<Component, 4, 4>& m) :
		m00(m.m00), m01(m.m01), m02(m.m02), m03(m.m03),
		m10(m.m10), m11(m.m11), m12(m.m12), m13(m.m13),
		m20(m.m20), m21(m.m21), m22(m.m22), m23(m.m23),
		m30(m.m30), m31(m.m31), m32(m.m32), m33(m.m33)
	{}

	Component& operator[](size_t index) { return data[index]; }
	const Component& operator[](size_t index) const { return data[index]; }

	Component& at(size_t r, size_t c) { return data[rc2index(r, c, 4)]; }
	const Component& at(size_t r, size_t c) const { return data[rc2index(r, c, 4)]; }

	Vector<Component, 3> forward() const
	{
		Vector<Component, 3> f;
		f.x = at(2, 0);
		f.y = at(2, 1);
		f.z = at(2, 2);
		return normalize(f);
	}

	Vector<Component, 3> up() const
	{
		Vector<Component, 3> u;
		u.x = at(1, 0);
		u.y = at(1, 1);
		u.z = at(1, 2);
		return normalize(u);
	}

	Vector<Component, 3> right() const
	{
		Vector<Component, 3> r;
		r.x = at(0, 0);
		r.y = at(0, 1);
		r.z = at(0, 2);
		return normalize(r);
	}

	Vector<Component, 3> position() const
	{
		Vector<Component, 3> pos;
		pos.x = at(0, 3);
		pos.y = at(1, 3);
		pos.z = at(2, 3);
		return pos;
	}

	Vector<Component, 3> get_scale() const
	{
		Vector<Component, 3> s;
		s.x = magnitude(Vector<Component, 4>(m00, m01, m02, m03));
		s.y = magnitude(Vector<Component, 4>(m10, m11, m12, m13));
		s.z = magnitude(Vector<Component, 4>(m20, m21, m22, m23));
		return s;
	}

	// yaw-pitch-roll
	// https://www.geometrictools.com/Documentation/EulerAngles.pdf
	Vector<Component, 3> to_euler() const
	{
		assert(is_rotation_matrix(*this));

		Vector<Component, 3> euler;

		if (m20 < (Component)1)
		{
			if (m20 > -(Component)1)
			{
				euler.x = tinymath::asin(m20);
				euler.y = tinymath::atan2(-m21, m22);
				euler.z = tinymath::atan2(-m10, m00);
			}
			else
			{
				euler.x = -HALF_PI;
				euler.y = -tinymath::atan2(m01, m11);
				euler.z = (Component)0;
			}
		}
		else
		{
			euler.x = HALF_PI;
			euler.y = tinymath::atan2(m01, m11);
			euler.z = (Component)0;
		}

		return euler;
	}

	Vector<Component, 4> row(int index) const
	{
		assert(index >= 0 && index <= 3);
		Vector<Component, 4> ret;
		switch (index)
		{
		case 0:
			ret = Vector<Component, 4>(m00, m01, m02, m03);
			break;
		case 1:
			ret = Vector<Component, 4>(m10, m11, m12, m13);
			break;
		case 2:
			ret = Vector<Component, 4>(m20, m21, m22, m23);
			break;
		case 3:
			ret = Vector<Component, 4>(m30, m31, m32, m33);
			break;
		}
		return ret;
	}

	Vector<Component, 4> column(int index) const
	{
		assert(index >= 0 && index <= 3);
		Vector<Component, 4> ret;
		switch (index)
		{
		case 0:
			ret = Vector<Component, 4>(m00, m10, m20, m30);
			break;
		case 1:
			ret = Vector<Component, 4>(m01, m11, m21, m31);
			break;
		case 2:
			ret = Vector<Component, 4>(m02, m12, m22, m32);
			break;
		case 3:
			ret = Vector<Component, 4>(m03, m13, m23, m33);
			break;
		}
		return ret;
	}
};

constexpr Matrix<float, 3, 3> kMat3x3Identity = Matrix<float, 3, 3>
(
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f
	);

constexpr Matrix<float, 3, 3> kMat3x3Zero = Matrix<float, 3, 3>(0.f);

constexpr Matrix<float, 4, 4> kMat4x4Identity = Matrix<float, 4, 4>
(
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
	);

constexpr Matrix<float, 4, 4> kMat4x4Zero = Matrix<float, 4, 4>(0.f);

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

template<typename Component, size_t ROW, size_t N, size_t COL>
Matrix<Component, ROW, COL> operator*(const Matrix<Component, ROW, N>& lhs, const Matrix<Component, N, COL>& rhs)
{
	Matrix<Component, ROW, COL> ret = Matrix<Component, ROW, COL>();

	for (size_t r = 0; r < ROW; ++r)
	{
		Component row[N];
		for (size_t c0 = 0; c0 < N; ++c0)
		{
			row[c0] = lhs.at(r, c0);
		}

		for (size_t c = 0; c < COL; ++c)
		{
			Component col[N];
			for (size_t r1 = 0; r1 < N; ++r1)
			{
				col[r1] = rhs.at(r1, c);
			}

			ret.at(r, c) = Component();
			for (size_t idx = 0; idx < N; idx++)
			{
				ret.at(r, c) += row[idx] * col[idx];
			}
		}
	}

	return ret;
}

template<typename Component>
Component determinant(const Matrix<Component, 3, 3>& m)
{
	Component det = (m.at(0, 0) * (m.at(1, 1) * m.at(2, 2) - m.at(1, 2) * m.at(2, 1)) -
					 m.at(0, 1) * (m.at(1, 0) * m.at(2, 2) - m.at(1, 2) * m.at(2, 0)) +
					 m.at(0, 2) * (m.at(1, 0) * m.at(2, 1) - m.at(1, 1) * m.at(2, 0)));
	return det;
}

template<typename Component>
Component determinant(const Matrix<Component, 4, 4>& m)
{
	Component m00 = m.at(0, 0);
	Component m10 = m.at(1, 0);
	Component m20 = m.at(2, 0);
	Component m30 = m.at(3, 0);
	Component m01 = m.at(0, 1);
	Component m11 = m.at(1, 1);
	Component m21 = m.at(2, 1);
	Component m31 = m.at(3, 1);
	Component m02 = m.at(0, 2);
	Component m12 = m.at(1, 2);
	Component m22 = m.at(2, 2);
	Component m32 = m.at(3, 2);
	Component m03 = m.at(0, 3);
	Component m13 = m.at(1, 3);
	Component m23 = m.at(2, 3);
	Component m33 = m.at(3, 3);

	return  m03 * m12 * m21 * m30 - m02 * m13 * m21 * m30 - m03 * m11 * m22 * m30 + m01 * m13 * m22 * m30 +
		m02 * m11 * m23 * m30 - m01 * m12 * m23 * m30 - m03 * m12 * m20 * m31 + m02 * m13 * m20 * m31 +
		m03 * m10 * m22 * m31 - m00 * m13 * m22 * m31 - m02 * m10 * m23 * m31 + m00 * m12 * m23 * m31 +
		m03 * m11 * m20 * m32 - m01 * m13 * m20 * m32 - m03 * m10 * m21 * m32 + m00 * m13 * m21 * m32 +
		m01 * m10 * m23 * m32 - m00 * m11 * m23 * m32 - m02 * m11 * m20 * m33 + m01 * m12 * m20 * m33 +
		m02 * m10 * m21 * m33 - m00 * m12 * m21 * m33 - m01 * m10 * m22 * m33 + m00 * m11 * m22 * m33;
}

template<typename Component>
Matrix<Component, 3, 3> inverse(const Matrix<Component, 3, 3>& m)
{
	Component one_over_det = (Component)1 / determinant(m);
	Matrix<Component, 3, 3> ret = Matrix<float, 3, 3>(1, 0, 0,
													  0, 1, 0,
													  0, 0, 1);

	ret.at(0, 0) = +(m.at(1, 1) * m.at(2, 2) - m.at(1, 2) * m.at(2, 1)) * one_over_det;
	ret.at(0, 1) = -(m.at(0, 1) * m.at(2, 2) - m.at(0, 2) * m.at(2, 1)) * one_over_det;
	ret.at(0, 2) = +(m.at(0, 1) * m.at(1, 2) - m.at(0, 2) * m.at(1, 1)) * one_over_det;
	ret.at(1, 0) = -(m.at(1, 0) * m.at(2, 2) - m.at(1, 2) * m.at(2, 0)) * one_over_det;
	ret.at(1, 1) = +(m.at(0, 0) * m.at(2, 2) - m.at(0, 2) * m.at(2, 0)) * one_over_det;
	ret.at(1, 2) = -(m.at(0, 0) * m.at(1, 2) - m.at(0, 2) * m.at(1, 0)) * one_over_det;
	ret.at(2, 0) = +(m.at(1, 0) * m.at(2, 1) - m.at(1, 1) * m.at(2, 0)) * one_over_det;
	ret.at(2, 1) = -(m.at(0, 0) * m.at(2, 1) - m.at(0, 1) * m.at(2, 0)) * one_over_det;
	ret.at(2, 2) = +(m.at(0, 0) * m.at(1, 1) - m.at(0, 1) * m.at(1, 0)) * one_over_det;
	return ret;
}

// Gaussian Elimination with Partial Pivoting https://stuff.mit.edu/afs/athena/course/10/10.001/Web/Course_Notes/GaussElimPivoting.html
// Cramer's rule is O(N^4) where Gaussian Elimination is O(N^3) https://www.reddit.com/r/math/comments/fkec7/why_would_you_use_gaussian_elimination_instead_of/
template<typename Component>
Matrix<Component, 4, 4> inverse(const Matrix<Component, 4, 4>& m)
{
	Component s0 = ((m[0] * m[5]) - (m[1] * m[4]));
	Component s1 = ((m[0] * m[6]) - (m[2] * m[4]));
	Component s2 = ((m[0] * m[7]) - (m[3] * m[4]));
	Component s3 = ((m[1] * m[6]) - (m[2] * m[5]));
	Component s4 = ((m[1] * m[7]) - (m[3] * m[5]));
	Component s5 = ((m[2] * m[7]) - (m[3] * m[6]));

	Component c5 = ((m[10] * m[15]) - (m[11] * m[14]));
	Component c4 = ((m[9] * m[15]) - (m[11] * m[13]));
	Component c3 = ((m[9] * m[14]) - (m[10] * m[13]));
	Component c2 = ((m[8] * m[15]) - (m[11] * m[12]));
	Component c1 = ((m[8] * m[14]) - (m[10] * m[12]));
	Component c0 = ((m[8] * m[13]) - (m[9] * m[12]));

	Component det = (s0 * c5) - (s1 * c4) + (s2 * c3) + (s3 * c2) - (s4 * c1) + (s5 * c0);

	Component one_over_det = (Component)1 / det;
	Matrix<Component, 4, 4> ret = kMat4x4Identity;

	ret[0] = +(m[5] * c5 - m[6] * c4 + m[7] * c3) * one_over_det;
	ret[1] = -(m[1] * c5 - m[2] * c4 + m[3] * c3) * one_over_det;
	ret[2] = +(m[13] * s5 - m[14] * s4 + m[15] * s3) * one_over_det;
	ret[3] = -(m[9] * s5 - m[10] * s4 + m[11] * s3) * one_over_det;
	ret[4] = -(m[4] * c5 - m[6] * c2 + m[7] * c1) * one_over_det;
	ret[5] = +(m[0] * c5 - m[2] * c2 + m[3] * c1) * one_over_det;
	ret[6] = -(m[12] * s5 - m[14] * s2 + m[15] * s1) * one_over_det;
	ret[7] = +(m[8] * s5 - m[10] * s2 + m[11] * s1) * one_over_det;
	ret[8] = +(m[4] * c4 - m[5] * c2 + m[7] * c0) * one_over_det;
	ret[9] = -(m[0] * c4 - m[1] * c2 + m[3] * c0) * one_over_det;
	ret[10] = +(m[12] * s4 - m[13] * s2 + m[15] * s0) * one_over_det;
	ret[11] = -(m[8] * s4 - m[9] * s2 + m[11] * s0) * one_over_det;
	ret[12] = -(m[4] * c3 - m[5] * c1 + m[6] * c0) * one_over_det;
	ret[13] = +(m[0] * c3 - m[1] * c1 + m[2] * c0) * one_over_det;
	ret[14] = -(m[12] * s3 - m[13] * s1 + m[14] * s0) * one_over_det;
	ret[15] = +(m[8] * s3 - m[9] * s1 + m[10] * s0) * one_over_det;
	return ret;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> translation(const Vector<Component, 3>& translation)
{
	Matrix<Component, 4, 4> m = kMat4x4Identity;
	m.at(0, 3) = translation.x;
	m.at(1, 3) = translation.y;
	m.at(2, 3) = translation.z;
	return m;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> rotation(const Vector<Component, 3>& axis, const Component& theta)
{
	Component rad = DEGREE2RAD(theta);
	Component s = tinymath::sin(rad);
	Component c = tinymath::cos(rad);
	Vector<Component, 3> normalized_axis = normalize(axis);
	Component x = normalized_axis.x;
	Component y = normalized_axis.y;
	Component z = normalized_axis.z;
	Matrix<Component, 4, 4> m = kMat4x4Identity;
	m.at(0, 0) = c + (1 - c) * x * x;
	m.at(0, 1) = (1 - c) * x * y - s * z;
	m.at(0, 2) = (1 - c) * x * z + s * y;
	m.at(1, 0) = (1 - c) * y * x + s * z;
	m.at(1, 1) = c + (1 - c) * y * y;
	m.at(1, 2) = (1 - c) * y * z - s * x;
	m.at(2, 0) = (1 - c) * z * x - s * y;
	m.at(2, 1) = (1 - c) * z * y + s * x;
	m.at(2, 2) = c + (1 - c) * z * z;
	return m;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> scale(const Vector<Component, 3>& scale)
{
	Matrix<Component, 4, 4> m = kMat4x4Identity;
	m.at(0, 0) = scale.x;
	m.at(1, 1) = scale.y;
	m.at(2, 2) = scale.z;
	return m;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> viewport(int x, int y, int w, int h)
{
	Matrix<Component, 4, 4> ret = kMat4x4Identity;
	ret.at(0, 0) = w * 0.5f;
	ret.at(0, 3) = x + (w * 0.5f);
	ret.at(1, 1) = h * 0.5f;
	ret.at(1, 3) = y + (h * 0.5f);
	return ret;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> from_euler(const Vector<Component, 3>& euler)
{
	return euler_x(euler.x) * euler_y(euler.y) * euler_z(euler.z);
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> from_angle(const Vector<Component, 3>& angles)
{
	return euler_x(DEGREE2RAD(angles.x)) * euler_y(DEGREE2RAD(angles.y)) * euler_z(DEGREE2RAD(angles.z));
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> euler_x(const Component& rad)
{
	Component c = tinymath::cos(rad);
	Component s = tinymath::sin(rad);
	return Matrix<Component, 4, 4>(
		1, 0, 0, 0,
		0, c, s, 0,
		0, -s, c, 0,
		0, 0, 0, 1
		);
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> euler_y(const Component& rad)
{
	Component c = tinymath::cos(rad);
	Component s = tinymath::sin(rad);
	return Matrix<Component, 4, 4>(
		c, 0, -s, 0,
		0, 1, 0, 0,
		s, 0, c, 0,
		0, 0, 0, 1
		);
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> euler_z(const Component& rad)
{
	Component c = tinymath::cos(rad);
	Component s = tinymath::sin(rad);
	return Matrix<Component, 4, 4>(
		c, s, 0, 0,
		-s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
		);
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> lookat(const Vector<Component, 3>& eye, const Vector<Component, 3>& target, const Vector<Component, 3>& world_up)
{
#ifdef LEFT_HANDED
	return lookat_lh(eye, target, world_up);
#else
	return lookat_rh(eye, target, world_up);
#endif
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> lookat_lh(const Vector<Component, 3>& eye, const Vector<Component, 3>& target, const Vector<Component, 3>& world_up)
{
	Vector<Component, 3> forward = normalize(target - eye);
	Vector<Component, 3> right = normalize(cross(normalize(world_up), forward));
	Vector<Component, 3> up = cross(forward, right);

	// UVN--right up forward
	Matrix<Component, 4, 4> view = kMat4x4Identity;
	view.at(0, 0) = right.x;
	view.at(0, 1) = right.y;
	view.at(0, 2) = right.z;
	view.at(1, 0) = up.x;
	view.at(1, 1) = up.y;
	view.at(1, 2) = up.z;
	view.at(2, 0) = forward.x;
	view.at(2, 1) = forward.y;
	view.at(2, 2) = forward.z;
	view.at(0, 3) = -dot(right, eye);
	view.at(1, 3) = -dot(up, eye);
	view.at(2, 3) = -dot(forward, eye);

	return view;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> lookat_rh(const Vector<Component, 3>& eye, const Vector<Component, 3>& target, const Vector<Component, 3>& world_up)
{
	Vector<Component, 3> forward = normalize(target - eye);
	Vector<Component, 3> right = normalize(cross(forward, normalize(world_up)));
	Vector<Component, 3> up = cross(right, forward);

	// UVN--right up forward
	Matrix<Component, 4, 4> view = kMat4x4Identity;
	view.at(0, 0) = right.x;
	view.at(0, 1) = right.y;
	view.at(0, 2) = right.z;
	view.at(1, 0) = up.x;
	view.at(1, 1) = up.y;
	view.at(1, 2) = up.z;
	view.at(2, 0) = -forward.x;
	view.at(2, 1) = -forward.y;
	view.at(2, 2) = -forward.z;
	view.at(0, 3) = -dot(right, eye);
	view.at(1, 3) = -dot(up, eye);
	view.at(2, 3) = dot(forward, eye);

	return view;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> perspective(const Component& fov, const Component& aspect, const Component& near, const Component& far)
{
#ifdef LEFT_HANDED 
#ifdef GL_LIKE
	return perspective_lh_gl(fov, aspect, near, far);
#else 
	return perspective_lh_dx(fov, aspect, near, far);
#endif
#else
#ifdef GL_LIKE
	return perspective_rh_gl(fov, aspect, near, far);
#else 
	return perspective_rh_dx(fov, aspect, near, far);
#endif
#endif
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> perspective_rh_dx(const Component& fov, const Component& aspect, const Component& near, const Component& far)
{
	Component rad = DEGREE2RAD(fov * 0.5f);
	Component cot = tinymath::cos(rad) / tinymath::sin(rad);
	Matrix<Component, 4, 4> m = kMat4x4Zero;
	m.at(0, 0) = cot / aspect;
	m.at(1, 1) = cot;
	m.at(2, 2) = -far / (far - near);
	m.at(3, 2) = -(Component)1;
	m.at(2, 3) = -(far * near) / (far - near);
	return m;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> perspective_lh_dx(const Component& fov, const Component& aspect, const Component& near, const Component& far)
{
	Component rad = DEGREE2RAD(fov * 0.5f);
	Component cot = tinymath::cos(rad) / tinymath::sin(rad);
	Matrix<Component, 4, 4> m = kMat4x4Zero;
	m.at(0, 0) = cot / aspect;
	m.at(1, 1) = cot;
	m.at(2, 2) = far / (far - near);
	m.at(3, 2) = (Component)1;
	m.at(2, 3) = -(far * near) / (far - near);
	return m;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> perspective_rh_gl(const Component& fov, const Component& aspect, const Component& near, const Component& far)
{
	Component rad = DEGREE2RAD(fov * 0.5f);
	Component cot = tinymath::cos(rad) / tinymath::sin(rad);
	Matrix<Component, 4, 4> m = kMat4x4Zero;
	m.at(0, 0) = cot / aspect;
	m.at(1, 1) = cot;
	m.at(2, 2) = -(far + near) / (far - near);
	m.at(3, 2) = -(Component)1;
	m.at(2, 3) = -(Component)2 * (far * near) / (far - near);
	return m;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> perspective_lh_gl(const Component& fov, const Component& aspect, const Component& near, const Component& far)
{
	Component rad = DEGREE2RAD(fov * 0.5f);
	Component cot = tinymath::cos(rad) / tinymath::sin(rad);
	Matrix<Component, 4, 4> m = kMat4x4Zero;
	m.at(0, 0) = cot / aspect;
	m.at(1, 1) = cot;
	m.at(2, 2) = (far + near) / (far - near);
	m.at(3, 2) = (Component)1;
	m.at(2, 3) = -(Component)2 * (far * near) / (far - near);
	return m;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> ortho(const Component& left, const Component& right, const Component& bottom, const Component& top, const Component& near, const Component& far)
{
#ifdef LEFT_HANDED 
#ifdef GL_LIKE
	return ortho_lh_gl(left, right, bottom, top, near, far);
#else 
	return ortho_lh_dx(left, right, bottom, top, near, far);
#endif
#else
#ifdef GL_LIKE
	return ortho_rh_gl(left, right, bottom, top, near, far);
#else 
	return ortho_rh_dx(left, right, bottom, top, near, far);
#endif
#endif
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> ortho_rh_dx(const Component& left, const Component& right, const Component& bottom, const Component& top, const Component& near, const Component& far)
{
	Matrix<Component, 4, 4> m = kMat4x4Identity;
	m.at(0, 0) = (Component)2 / (right - left);
	m.at(1, 1) = (Component)2 / (top - bottom);
	m.at(2, 2) = -(Component)1 / (far - near);
	m.at(0, 3) = -(right + left) / (right - left);
	m.at(1, 3) = -(top + bottom) / (top - bottom);
	m.at(2, 3) = -near / (far - near);
	return m;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> ortho_lh_dx(const Component& left, const Component& right, const Component& bottom, const Component& top, const Component& near, const Component& far)
{
	Matrix<Component, 4, 4> m = kMat4x4Identity;
	m.at(0, 0) = (Component)2 / (right - left);
	m.at(1, 1) = (Component)2 / (top - bottom);
	m.at(2, 2) = (Component)1 / (far - near);
	m.at(0, 3) = -(right + left) / (right - left);
	m.at(1, 3) = -(top + bottom) / (top - bottom);
	m.at(2, 3) = -near / (far - near);
	return m;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> ortho_rh_gl(const Component& left, const Component& right, const Component& bottom, const Component& top, const Component& near, const Component& far)
{
	Matrix<Component, 4, 4> m = kMat4x4Identity;
	m.at(0, 0) = (Component)2 / (right - left);
	m.at(1, 1) = (Component)2 / (top - bottom);
	m.at(2, 2) = -(Component)2 / (far - near);
	m.at(0, 3) = -(right + left) / (right - left);
	m.at(1, 3) = -(top + bottom) / (top - bottom);
	m.at(2, 3) = -(far + near) / (far - near);
	return m;
}

template <typename Component>
TMATH_INLINE Matrix<Component, 4, 4> ortho_lh_gl(const Component& left, const Component& right, const Component& bottom, const Component& top, const Component& near, const Component& far)
{
	Matrix<Component, 4, 4> m = kMat4x4Identity;
	m.at(0, 0) = (Component)2 / (right - left);
	m.at(1, 1) = (Component)2 / (top - bottom);
	m.at(2, 2) = (Component)2 / (far - near);
	m.at(0, 3) = -(right + left) / (right - left);
	m.at(1, 3) = -(top + bottom) / (top - bottom);
	m.at(2, 3) = -(far + near) / (far - near);
	return m;
}

template<typename Component>
TMATH_INLINE Matrix<Component, 3, 3> mat4x4_to_mat3x3(const Matrix<Component, 4, 4>& mat)
{
	Matrix<Component, 3, 3> ret = kMat3x3Zero;
	ret.m00 = mat.at(0, 0);
	ret.m01 = mat.at(0, 1);
	ret.m02 = mat.at(0, 2);
	ret.m10 = mat.at(1, 0);
	ret.m11 = mat.at(1, 1);
	ret.m12 = mat.at(1, 2);
	ret.m20 = mat.at(2, 0);
	ret.m21 = mat.at(2, 1);
	ret.m22 = mat.at(2, 2);
	return ret;
}

template<typename Component>
TMATH_INLINE Matrix<Component, 4, 4> mat3x3_to_mat4x4(const Matrix<Component, 3, 3>& mat)
{
	Matrix<Component, 4, 4> ret = kMat4x4Identity;
	ret.m00 = mat.at(0, 0);
	ret.m01 = mat.at(0, 1);
	ret.m02 = mat.at(0, 2);
	ret.m10 = mat.at(1, 0);
	ret.m11 = mat.at(1, 1);
	ret.m12 = mat.at(1, 2);
	ret.m20 = mat.at(2, 0);
	ret.m21 = mat.at(2, 1);
	ret.m22 = mat.at(2, 2);
	return ret;
}

END_NAMESPACE
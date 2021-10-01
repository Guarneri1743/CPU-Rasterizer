TINYMATH_NAMESPACE

template<typename Component, size_t ROW, size_t N>
Vector<Component, N> operator*(const Matrix<Component, ROW, N>& m, const Vector<Component, N>& v)
{
	Vector<Component, N> ret = Vector<Component, N>((Component)0);

	for (int r = 0; r < ROW; ++r)
	{
		Component row[N];
		for (int c0 = 0; c0 < N; ++c0)
		{
			row[c0] = m.at(r, c0);
		}

		for (int idx = 0; idx < N; idx++)
		{
			ret[r] += row[idx] * v[idx];
		}
	}

	return ret;
}

template<typename Component>
Vector<Component, 3> transform_point(const Matrix<Component, 4, 4>& m, const Vector<Component, 3>& point)
{
	Vector<Component, 3> ret;
	ret.x = m.m00 * point.x + m.m01 * point.y + m.m02 * point.z + m.m03;
	ret.y = m.m10 * point.x + m.m11 * point.y + m.m12 * point.z + m.m13;
	ret.z = m.m20 * point.x + m.m21 * point.y + m.m22 * point.z + m.m23;
	float num = m.m30 * point.x + m.m31 * point.y + m.m32 * point.z + m.m33;
	num = 1.0f / num;
	ret.x *= num;
	ret.y *= num;
	ret.z *= num;
	return ret;
}

template<typename Component>
Vector<Component, 3> transform_vector(const Matrix<Component, 4, 4>& m, const Vector<Component, 3>& vec)
{
	Vector<Component, 3> ret;
	ret.x = m.m00 * point.x + m.m01 * point.y + m.m02 * point.z;
	ret.y = m.m10 * point.x + m.m11 * point.y + m.m12 * point.z;
	ret.z = m.m20 * point.x + m.m21 * point.y + m.m22 * point.z;
	return ret;
}

END_NAMESPACE
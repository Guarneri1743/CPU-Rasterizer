#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	struct Matrix3x3 {
	public:
		static const Matrix3x3 IDENTITY;
		static const Matrix3x3 ZERO;

	public:
		Matrix3x3() {
			for (int i = 0; i < 9; i++) {
				this->operator[](i) = 0.0f;
			}
		}

		Matrix3x3(const Vector3& row0, const Vector3& row1, const Vector3& row2) {
			m00 = row0.x;
			m01 = row1.x;
			m02 = row2.x;
			m10 = row0.y;
			m11 = row1.y;
			m12 = row2.y;
			m20 = row0.z;
			m21 = row1.z;
			m22 = row2.z;
		}

		Matrix3x3(const float& _m00, const float& _m01, const float& _m02,
			const float& _m10, const float& _m11, const float& _m12,
			const float& _m20, const float& _m21, const float& _m22) {
			m00 = _m00;
			m01 = _m01;
			m02 = _m02;
			m10 = _m10;
			m11 = _m11;
			m12 = _m12;
			m20 = _m20;
			m21 = _m21;
			m22 = _m22;
		}

		Matrix3x3(const Matrix4x4& mat) {
			m00 = mat.at(0, 0);
			m01 = mat.at(0, 1);
			m02 = mat.at(0, 2);
			m10 = mat.at(1, 0);
			m11 = mat.at(1, 1);
			m12 = mat.at(1, 2);
			m20 = mat.at(2, 0);
			m21 = mat.at(2, 1);
			m22 = mat.at(2, 2);
		}

	private:
		float m00;
		float m01;
		float m02;
		float m10;
		float m11;
		float m12;
		float m20;
		float m21;
		float m22;

	public:
		static Matrix3x3 rotation(const Vector3& axis, const float& theta) {
			float rad = DEGREE2RAD(theta);
			float s = std::sin(rad);
			float c = std::cos(rad);
			Vector3 normalized_axis = Vector3::normalize(axis);
			float x = normalized_axis.x;
			float y = normalized_axis.y;
			float z = normalized_axis.z;
			Matrix3x3 m = IDENTITY;
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

		static Matrix3x3 scale(const Vector3& scale) {
			Matrix3x3 m = IDENTITY;
			m.at(0, 0) = scale.x;
			m.at(1, 1) = scale.y;
			m.at(2, 2) = scale.z;
			return m;
		}

		Vector3 forward() const {
			Vector3 f;
			f.x = at(2, 0);
			f.y = at(2, 1);
			f.z = at(2, 2);
			return f.normalized();
		}

		Vector3 up() const {
			Vector3 u;
			u.x = at(1, 0);
			u.y = at(1, 1);
			u.z = at(1, 2);
			return u.normalized();
		}

		Vector3 right() const {
			Vector3 r;
			r.x = at(0, 0);
			r.y = at(0, 1);
			r.z = at(0, 2);
			return r.normalized();
		}

		Matrix3x3 operator *(const Matrix3x3& rhs) const {
			Matrix3x3 ret;
			ret.m00 = m00 * rhs.m00 + m01 * rhs.m10 + m02 * rhs.m20;
			ret.m01 = m00 * rhs.m01 + m01 * rhs.m11 + m02 * rhs.m21;
			ret.m02 = m00 * rhs.m02 + m01 * rhs.m12 + m02 * rhs.m22;
			ret.m10 = m10 * rhs.m00 + m11 * rhs.m10 + m12 * rhs.m20;
			ret.m11 = m10 * rhs.m01 + m11 * rhs.m11 + m12 * rhs.m21;
			ret.m12 = m10 * rhs.m02 + m11 * rhs.m12 + m12 * rhs.m22;
			ret.m20 = m20 * rhs.m00 + m21 * rhs.m10 + m22 * rhs.m20;
			ret.m21 = m20 * rhs.m01 + m21 * rhs.m11 + m22 * rhs.m21;
			ret.m22 = m20 * rhs.m02 + m21 * rhs.m12 + m22 * rhs.m22;
			return ret;
		}

		Matrix3x3 transpose() const {
			Matrix3x3 ret = Matrix3x3(*this);
			std::swap(ret.at(0, 1), ret.at(1, 0));
			std::swap(ret.at(0, 2), ret.at(2, 0));
			std::swap(ret.at(1, 2), ret.at(2, 1));
			return ret;
		}

		Matrix3x3 inverse() const {
			float det = (at(0, 0) * (at(1, 1) * at(2, 2) - at(1, 2) * at(2, 1)) - 
					     at(0, 1) * (at(1, 0) * at(2, 2) - at(1, 2) * at(2, 0)) +
				         at(0, 2) * (at(1, 0) * at(2, 1) - at(1, 1) * at(2, 0)));
			float one_over_det = 1.0f / det;
			Matrix3x3 ret;
			ret.at(0, 0) = +(at(1, 1) * at(2, 2) - at(1, 2) * at(2, 1)) * one_over_det;
			ret.at(0, 1) = -(at(0, 1) * at(2, 2) - at(0, 2) * at(2, 1)) * one_over_det;
			ret.at(0, 2) = +(at(0, 1) * at(1, 2) - at(0, 2) * at(1, 1)) * one_over_det;
			ret.at(1, 0) = -(at(1, 0) * at(2, 2) - at(1, 2) * at(2, 0)) * one_over_det;
			ret.at(1, 1) = +(at(0, 0) * at(2, 2) - at(0, 2) * at(2, 0)) * one_over_det;
			ret.at(1, 2) = -(at(0, 0) * at(1, 2) - at(0, 2) * at(1, 0)) * one_over_det;
			ret.at(2, 0) = +(at(1, 0) * at(2, 1) - at(1, 1) * at(2, 0)) * one_over_det;
			ret.at(2, 1) = -(at(0, 0) * at(2, 1) - at(0, 1) * at(2, 0)) * one_over_det;
			ret.at(2, 2) = +(at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0)) * one_over_det;
			return ret;
		}

		Vector3 operator *(const Vector3& vector) const {
			Vector3 ret;
			ret.x = m00 * vector.x + m01 * vector.y + m02 * vector.z;
			ret.y = m10 * vector.x + m11 * vector.y + m12 * vector.z;
			ret.z = m20 * vector.x + m21 * vector.y + m22 * vector.z;
			return ret;
		}

		bool operator ==(const Matrix3x3& rhs) const {
			return !(*this != rhs);
		}

		bool operator !=(const Matrix3x3& rhs) const {
			for (int idx = 0; idx < 3; idx++) {
				if (column(idx) != rhs.column(idx)) {
					return true;
				}
			}
			return false;
		}

		Vector3 row(const int& index) const {
			Vector3 ret;
			switch (index)
			{
			case 0:
				ret = Vector3(m00, m01, m02);
				break;
			case 1:
				ret = Vector3(m10, m11, m12);
				break;
			case 2:
				ret = Vector3(m20, m21, m22);
				break;
			default:
				std::cerr << "index out of range: " << index << std::endl;
			}
			return ret;
		}

		Vector3 column(const int& index) const {
			Vector3 ret;
			switch (index)
			{
			case 0:
				ret = Vector3(m00, m10, m20);
				break;
			case 1:
				ret = Vector3(m01, m11, m21);
				break;
			case 2:
				ret = Vector3(m02, m12, m22);
				break;
				break;
			default:
				std::cerr << "index out of range: " << index << std::endl;
			}
			return ret;
		}

		const float& at(const int& row, const int& column) const { return this->operator[](rc2index(row, column)); }

		float& at(const int& row, const int& column) { return this->operator[](rc2index(row, column)); }

		const float& operator[](const int& index) const {
			switch (index)
			{
			case 0:
				return m00;
			case 1:
				return m01;
			case 2:
				return m02;
			case 3:
				return m10;
			case 4:
				return m11;
			case 5:
				return m12;
			case 6:
				return m20;
			case 7:
				return m21;
			case 8:
				return m22;
			default:
				std::cerr << "index out of range: " << index << std::endl;
				return m00;
			}
		}

		float& operator[](const int& index) {
			switch (index)
			{
			case 0:
				return m00;
			case 1:
				return m01;
			case 2:
				return m02;
			case 3:
				return m10;
			case 4:
				return m11;
			case 5:
				return m12;
			case 6:
				return m20;
			case 7:
				return m21;
			case 8:
				return m22;
			default:
				std::cerr << "index out of range: " << index << std::endl;
				return m00;
			}
		}

		std::string str() const {
			std::stringstream ss;
			ss << row(0) << std::endl << row(1) << std::endl << row(2) << std::endl << std::endl;
			return ss.str();
		}

	private:
		int rc2index(const int& row, const int& column) const {
			return row * 3 + column;
		}
	};

	const Matrix3x3 Matrix3x3::IDENTITY(Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
	const Matrix3x3 Matrix3x3::ZERO;

	static std::ostream& operator << (std::ostream& stream, const Matrix3x3& mat) {
		stream << mat.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Matrix3x3& mat) {
		stream << mat.str();
		return stream;
	}
}
#pragma once
#include <guarneri.hpp>

namespace guarneri {
	struct mat4 {
	public:
		static const mat4 IDENTITY;
		static const mat4 ZERO;

	public:
		mat4(){
			for (int i = 0; i < 16; i++) {
				this->operator[](i) = 0.0f;
			}
		}

		mat4(const float4& column0, const float4& column1, const float4& column2, const float4& column3) {
			m00 = column0.x;
			m01 = column1.x;
			m02 = column2.x;
			m03 = column3.x;
			m10 = column0.y;
			m11 = column1.y;
			m12 = column2.y;
			m13 = column3.y;
			m20 = column0.z;
			m21 = column1.z;
			m22 = column2.z;
			m23 = column3.z;
			m30 = column0.w;
			m31 = column1.w;
			m32 = column2.w;
			m33 = column3.w;
		}

	private:
		float m00;
		float m01;
		float m02;
		float m03;
		float m10;
		float m11;
		float m12;
		float m13;
		float m20;
		float m21;
		float m22;
		float m23;
		float m30;
		float m31;
		float m32;
		float m33;

	public:
		static mat4 translation(const float3& translation) {
			mat4 m = IDENTITY;
			m.at(0, 3) = translation.x;
			m.at(1, 3) = translation.y;
			m.at(2, 3) = translation.z;
			return m;
		}

		static mat4 rotation(const float3& axis, const float& theta) {
			float rad = DEGREE2RAD(theta);
			float s = std::sin(rad);
			float c = std::cos(rad);
			float3 normalized_axis = float3::normalize(axis);
			float x = normalized_axis.x;
			float y = normalized_axis.y;
			float z = normalized_axis.z;
			mat4 m = IDENTITY;
			m.at(0, 0) = c + (1 - c) * x * x;
			m.at(0, 1) = (1 - c) * x * y - s * z;
			m.at(0, 2) = (1 - c) * x * z + s * y;
			m.at(1, 0) = (1 - c) * y * x + s * z;
			m.at(1, 1) = c + (1 - c) * y * y;
			m.at(1, 2) = (1 - c) * y * z - s * x;
			m.at(2, 0) = (1 - c) * z * x - s * y;
			m.at(2, 1) = (1 - c) * z * y + s * x;
			m.at(2, 2) = c + (1 - c) * z * z;
			/*m.at(0, 3) = m.at(1, 3) = m.at(2, 3) = 0.0f;
			m.at(3, 0) = m.at(3, 1) = m.at(3, 2) = 0.0f;
			m.at(3, 3) = 1.0f;*/
			return m;
		}

		static mat4 scale(const float3& scale) {
			mat4 m = IDENTITY;
			m.at(0, 0) = scale.x;
			m.at(1, 1) = scale.y;
			m.at(2, 2) = scale.z;
			return m;
		}

		static mat4 viewport(const int& x, const int& y, const int& w, const int& h) {
			mat4 ret = IDENTITY;
			ret.at(0, 0) = w * 0.5f;
			ret.at(0, 3) = x + (w * 0.5f);
			ret.at(1, 1) = h * 0.5f;
			ret.at(1, 3) = y + (h * 0.5f);
			return ret;
		}

		// perspective matrix
		static mat4 perspective(const float& fov, const float& aspect, const float& near, const float& far) {
			float rad = DEGREE2RAD(fov * 0.5f);
			float cot = std::cos(rad) / std::sin(rad);
			mat4 m = ZERO;
			m.at(0, 0) = cot / aspect;
			m.at(1, 1) = cot;
			m.at(2, 2) = (far + near) / (near - far);
			m.at(3, 2) = -1.0f;
			m.at(2, 3) = 2.0f * (far * near) / (near - far);
			return m;
		}

		// ortho matrix
		static mat4 ortho(const float& left, const float& right, const float& bottom, const float& top, const float& near, const float& far) {
			mat4 m = IDENTITY;
			m.at(0, 0) = 2.0f / (right - left);
			m.at(0, 3) = -(right + left) / (right - left);
			m.at(1, 1) = 2.0f / (top - bottom);
			m.at(1, 3) = -(top + bottom) / (top - bottom);
			m.at(2, 2) = -2.0f / (far - near);
			m.at(2, 3) = -(far + near) / (far - near);
			return m;
		}

		static mat4 lookat_matrix(const float3& eye, const float3& target, const float3& world_up) {
			float3 forward = float3::normalize(target - eye);
			float3 right = float3::normalize(float3::cross(forward, float3::normalize(world_up))); // it's possible that camera's forward vector is parallel to the world up vector
			float3 up = float3::cross(right, forward); // no need to normalize

			// UVN--right up forward
			mat4 view = mat4::IDENTITY;
			view.at(0, 0) = right.x;
			view.at(0, 1) = right.y;
			view.at(0, 2) = right.z;
			view.at(1, 0) = up.x;
			view.at(1, 1) = up.y;
			view.at(1, 2) = up.z;
			view.at(2, 0) = -forward.x;
			view.at(2, 1) = -forward.y;
			view.at(2, 2) = -forward.z;
			view.at(0, 3) = -float3::dot(right, eye);
			view.at(1, 3) = -float3::dot(up, eye);
			view.at(2, 3) = float3::dot(forward, eye);

			return view;
		}

		// view matrix
		static mat4 lookat(const float3& eye, const float3& target, const float3& world_up) {
			float3 forward = float3::normalize(target - eye);
			float3 right = float3::normalize(float3::cross(forward, float3::normalize(world_up))); // it's possible that camera's forward vector is parallel to the world up vector
			float3 up = float3::cross(right, forward); // no need to normalize

			// UVN--right up forward
			mat4 view = mat4::IDENTITY;
			view.at(0, 0) = right.x;
			view.at(0, 1) = right.y;
			view.at(0, 2) = right.z;
			view.at(1, 0) = up.x;
			view.at(1, 1) = up.y;
			view.at(1, 2) = up.z;
			view.at(2, 0) = -forward.x;
			view.at(2, 1) = -forward.y;
			view.at(2, 2) = -forward.z;
			view.at(0, 3) = -float3::dot(right, eye);
			view.at(1, 3) = -float3::dot(up, eye);
			view.at(2, 3) = float3::dot(forward, eye);

			return view;
		}

		float3 transform_point(const float3& point) const {
			float3 ret;
			ret.x = m00 * point.x + m01 * point.y + m02 * point.z + m03;
			ret.y = m10 * point.x + m11 * point.y + m12 * point.z + m13;
			ret.z = m20 * point.x + m21 * point.y + m22 * point.z + m23;
			float num = m30 * point.x + m31 * point.y + m32 * point.z + m33;
			num = 1.0f / num;
			ret.x *= num;
			ret.y *= num;
			ret.z *= num;
			return ret;
		}

		float3 transform_direction(const float3& point) const {
			float3 ret;
			ret.x = m00 * point.x + m01 * point.y + m02 * point.z;
			ret.y = m10 * point.x + m11 * point.y + m12 * point.z;
			ret.z = m20 * point.x + m21 * point.y + m22 * point.z;
			return ret;
		}

		mat4 operator *(const mat4& rhs) const {
			mat4 ret;
			ret.m00 = m00 * rhs.m00 + m01 * rhs.m10 + m02 * rhs.m20 + m03 * rhs.m30;
			ret.m01 = m00 * rhs.m01 + m01 * rhs.m11 + m02 * rhs.m21 + m03 * rhs.m31;
			ret.m02 = m00 * rhs.m02 + m01 * rhs.m12 + m02 * rhs.m22 + m03 * rhs.m32;
			ret.m03 = m00 * rhs.m03 + m01 * rhs.m13 + m02 * rhs.m23 + m03 * rhs.m33;
			ret.m10 = m10 * rhs.m00 + m11 * rhs.m10 + m12 * rhs.m20 + m13 * rhs.m30;
			ret.m11 = m10 * rhs.m01 + m11 * rhs.m11 + m12 * rhs.m21 + m13 * rhs.m31;
			ret.m12 = m10 * rhs.m02 + m11 * rhs.m12 + m12 * rhs.m22 + m13 * rhs.m32;
			ret.m13 = m10 * rhs.m03 + m11 * rhs.m13 + m12 * rhs.m23 + m13 * rhs.m33;
			ret.m20 = m20 * rhs.m00 + m21 * rhs.m10 + m22 * rhs.m20 + m23 * rhs.m30;
			ret.m21 = m20 * rhs.m01 + m21 * rhs.m11 + m22 * rhs.m21 + m23 * rhs.m31;
			ret.m22 = m20 * rhs.m02 + m21 * rhs.m12 + m22 * rhs.m22 + m23 * rhs.m32;
			ret.m23 = m20 * rhs.m03 + m21 * rhs.m13 + m22 * rhs.m23 + m23 * rhs.m33;
			ret.m30 = m30 * rhs.m00 + m31 * rhs.m10 + m32 * rhs.m20 + m33 * rhs.m30;
			ret.m31 = m30 * rhs.m01 + m31 * rhs.m11 + m32 * rhs.m21 + m33 * rhs.m31;
			ret.m32 = m30 * rhs.m02 + m31 * rhs.m12 + m32 * rhs.m22 + m33 * rhs.m32;
			ret.m33 = m30 * rhs.m03 + m31 * rhs.m13 + m32 * rhs.m23 + m33 * rhs.m33;
			return ret;
		}

		mat4 transpose() const {
			mat4 ret = mat4(*this);
			std::swap(ret.at(0, 1), ret.at(1, 0));
			std::swap(ret.at(0, 2), ret.at(2, 0));
			std::swap(ret.at(0, 3), ret.at(3, 0));
			std::swap(ret.at(1, 2), ret.at(2, 1));
			std::swap(ret.at(1, 3), ret.at(3, 1));
			std::swap(ret.at(2, 3), ret.at(3, 2));
			return ret;
		}

		// Gaussian Elimination with Partial Pivoting https://stuff.mit.edu/afs/athena/course/10/10.001/Web/Course_Notes/GaussElimPivoting.html
		// Cramer's rule is O(N^4) where Gaussian Elimination is O(N^3) https://www.reddit.com/r/math/comments/fkec7/why_would_you_use_gaussian_elimination_instead_of/
		mat4 inverse() const
		{
			float r0[8];
			float r1[8];
			float r2[8];
			float r3[8];
			float m0, m1, m2, m3, s;

			r0[0] = at(0, 0);
			r0[1] = at(0, 1);
			r0[2] = at(0, 2);
			r0[3] = at(0, 3);
			r0[4] = 1.0;
			r0[5] = r0[6] = r0[7] = 0.0;

			r1[0] = at(1, 0);
			r1[1] = at(1, 1);
			r1[2] = at(1, 2);
			r1[3] = at(1, 3);
			r1[5] = 1.0;
			r1[4] = r1[6] = r1[7] = 0.0;

			r2[0] = at(2, 0);
			r2[1] = at(2, 1);
			r2[2] = at(2, 2);
			r2[3] = at(2, 3);
			r2[6] = 1.0;
			r2[4] = r2[5] = r2[7] = 0.0;

			r3[0] = at(3, 0);
			r3[1] = at(3, 1);
			r3[2] = at(3, 2);
			r3[3] = at(3, 3);
			r3[7] = 1.0;
			r3[4] = r3[5] = r3[6] = 0.0;

			if (std::abs(r3[0]) > std::abs(r2[0]))
				std::swap(r3, r2);
			if (std::abs(r2[0]) > std::abs(r1[0]))
				std::swap(r2, r1);
			if (std::abs(r1[0]) > std::abs(r0[0]))
				std::swap(r1, r0);

			if (0.0f == r0[0])
				return mat4::ZERO;

			m1 = r1[0] / r0[0]; m2 = r2[0] / r0[0]; m3 = r3[0] / r0[0];

			s = r0[1];
			r1[1] -= m1 * s;
			r2[1] -= m2 * s;
			r3[1] -= m3 * s;

			s = r0[2];
			r1[2] -= m1 * s;
			r2[2] -= m2 * s;
			r3[2] -= m3 * s;

			s = r0[3];
			r1[3] -= m1 * s;
			r2[3] -= m2 * s;
			r3[3] -= m3 * s;
			s = r0[4];

			if (s != 0.0f)
			{
				r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s;
			}
			s = r0[5];
			if (s != 0.0f)
			{
				r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s;
			}
			s = r0[6];
			if (s != 0.0f)
			{
				r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s;
			}
			s = r0[7];
			if (s != 0.0f)
			{
				r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s;
			}

			if (std::abs(r3[1]) > std::abs(r2[1]))
				std::swap(r3, r2);
			if (std::abs(r2[1]) > std::abs(r1[1]))
				std::swap(r2, r1);

			if (0.0f == r1[1])
				return mat4::ZERO;

			m2 = r2[1] / r1[1];
			m3 = r3[1] / r1[1];

			r2[2] -= m2 * r1[2];
			r3[2] -= m3 * r1[2];
			r2[3] -= m2 * r1[3];
			r3[3] -= m3 * r1[3];

			s = r1[4];

			if (0.0f != s)
			{
				r2[4] -= m2 * s;
				r3[4] -= m3 * s;
			}

			s = r1[5];

			if (0.0f != s)
			{
				r2[5] -= m2 * s;
				r3[5] -= m3 * s;
			}

			s = r1[6];

			if (0.0f != s)
			{
				r2[6] -= m2 * s;
				r3[6] -= m3 * s;
			}

			s = r1[7];

			if (0.0f != s)
			{
				r2[7] -= m2 * s;
				r3[7] -= m3 * s;
			}

			if (std::abs(r3[2]) > std::abs(r2[2]))
				std::swap(r3, r2);

			if (0.0f == r2[2])
				return mat4::ZERO;

			m3 = r3[2] / r2[2];
			r3[3] -= m3 * r2[3];
			r3[4] -= m3 * r2[4];
			r3[5] -= m3 * r2[5];
			r3[6] -= m3 * r2[6];
			r3[7] -= m3 * r2[7];

			if (0.0f == r3[3])
				return mat4::ZERO;

			s = 1.0f / r3[3];
			r3[4] *= s;
			r3[5] *= s;
			r3[6] *= s;
			r3[7] *= s;

			m2 = r2[3];
			s = 1.0f / r2[2];
			r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
				r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
			m1 = r1[3];
			r1[4] -= r3[4] * m1; r1[5] -= r3[5] * m1,
				r1[6] -= r3[6] * m1; r1[7] -= r3[7] * m1;
			m0 = r0[3];
			r0[4] -= r3[4] * m0; r0[5] -= r3[5] * m0,
				r0[6] -= r3[6] * m0; r0[7] -= r3[7] * m0;

			m1 = r1[2];
			s = 1.0f / r1[1];
			r1[4] = s * (r1[4] - r2[4] * m1); r1[5] = s * (r1[5] - r2[5] * m1),
				r1[6] = s * (r1[6] - r2[6] * m1); r1[7] = s * (r1[7] - r2[7] * m1);
			m0 = r0[2];
			r0[4] -= r2[4] * m0; r0[5] -= r2[5] * m0,
				r0[6] -= r2[6] * m0; r0[7] -= r2[7] * m0;

			m0 = r0[1];
			s = 1.0f / r0[0];
			r0[4] = s * (r0[4] - r1[4] * m0); r0[5] = s * (r0[5] - r1[5] * m0),
				r0[6] = s * (r0[6] - r1[6] * m0); r0[7] = s * (r0[7] - r1[7] * m0);

			mat4 out;
			out.at(0, 0) = r0[4]; out.at(0, 1) = r0[5], out.at(0, 2) = r0[6]; out.at(0, 3) = r0[7];
			out.at(1, 0) = r1[4]; out.at(1, 1) = r1[5], out.at(1, 2) = r1[6]; out.at(1, 3) = r1[7];
			out.at(2, 0) = r2[4]; out.at(2, 1) = r2[5], out.at(2, 2) = r2[6]; out.at(2, 3) = r2[7];
			out.at(3, 0) = r3[4]; out.at(3, 1) = r3[5], out.at(3, 2) = r3[6]; out.at(3, 3) = r3[7];
			return out;
		}

		float4 operator *(const float4& vector) const {
			float4 ret;
			ret.x = m00 * vector.x + m01 * vector.y + m02 * vector.z + m03 * vector.w;
			ret.y = m10 * vector.x + m11 * vector.y + m12 * vector.z + m13 * vector.w;
			ret.z = m20 * vector.x + m21 * vector.y + m22 * vector.z + m23 * vector.w;
			ret.w = m30 * vector.x + m31 * vector.y + m32 * vector.z + m33 * vector.w;
			return ret;
		}

		bool operator ==(const mat4& rhs) const {
			return !(*this != rhs);
		}

		bool operator !=(const mat4& rhs) const {
			for (int idx = 0; idx < 4; idx++) {
				if (column(idx) != rhs.column(idx)) {
					return true;
				}
			}
			return false;
		}

		float4 row(const int& index) const {
			float4 ret;
			switch (index)
			{
			case 0:
				ret = float4(m00, m01, m02, m03);
				break;
			case 1:
				ret = float4(m10, m11, m12, m13);
				break;
			case 2:
				ret = float4(m20, m21, m22, m23);
				break;
			case 3:
				ret = float4(m30, m31, m32, m33);
				break;
			default:
				std::cerr << "index out of range: " << index << std::endl;
			}
			return ret;
		}

		float4 column(const int& index) const {
			float4 ret;
			switch (index)
			{
			case 0:
				ret = float4(m00, m10, m20, m30);
				break;
			case 1:
				ret = float4(m01, m11, m21, m31);
				break;
			case 2:
				ret = float4(m02, m12, m22, m32);
				break;
			case 3:
				ret = float4(m03, m13, m23, m33);
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
				break;
			case 1:
				return m10;
				break;
			case 2:
				return m20;
				break;
			case 3:
				return m30;
				break;
			case 4:
				return m01;
				break;
			case 5:
				return m11;
				break;
			case 6:
				return m21;
				break;
			case 7:
				return m31;
				break;
			case 8:
				return m02;
				break;
			case 9:
				return m12;
				break;
			case 10:
				return m22;
				break;
			case 11:
				return m32;
				break;
			case 12:
				return m03;
				break;
			case 13:
				return m13;
				break;
			case 14:
				return m23;
				break;
			case 15:
				return m33;
				break;
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
				return m10;
			case 2:
				return m20;
			case 3:
				return m30;
			case 4:
				return m01;
			case 5:
				return m11;
			case 6:
				return m21;
			case 7:
				return m31;
			case 8:
				return m02;
			case 9:
				return m12;
			case 10:
				return m22;
			case 11:
				return m32;
			case 12:
				return m03;
			case 13:
				return m13;
			case 14:
				return m23;
			case 15:
				return m33;
			default:
				std::cerr << "index out of range: " << index << std::endl;
				return m00;
			}
		}

		std::string str() const {
			std::stringstream ss;
			ss << row(0) << std::endl << row(1) << std::endl << row(2) << std::endl << row(3) << std::endl;
			return ss.str();
		}

	private:
		int rc2index(const int& row, const int& column) const {
			return row + (column * 4);
		}
	};

	const mat4 mat4::IDENTITY(float4(1.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 1.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 1.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 1.0f));
	const mat4 mat4::ZERO;

	static std::ostream& operator << (std::ostream& stream, const mat4& mat) {
		stream << mat.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const mat4& mat) {
		stream << mat.str();
		return stream;
	}
}
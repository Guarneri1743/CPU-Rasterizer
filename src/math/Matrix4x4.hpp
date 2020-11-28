#ifndef _MAT4_
#define _MAT4_
#include <Guarneri.hpp>

namespace Guarneri {
	struct Matrix4x4 {
	public:
		static const Matrix4x4 IDENTITY;
		static const Matrix4x4 ZERO;

	public:
		Matrix4x4(){
			for (int i = 0; i < 16; i++) {
				this->operator[](i) = 0.0f;
			}
		}

		Matrix4x4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3) {
			m00 = row0.x;
			m01 = row0.y;
			m02 = row0.z;
			m03 = row0.w;
			m10 = row1.x;
			m11 = row1.y;
			m12 = row1.z;
			m13 = row1.w;
			m20 = row2.x;
			m21 = row2.y;
			m22 = row2.z;
			m23 = row2.w;
			m30 = row3.x;
			m31 = row3.y;
			m32 = row3.z;
			m33 = row3.w;
		}

		Matrix4x4(const float& _m00, const float& _m01, const float& _m02, const float& _m03,
			const float& _m10, const float& _m11, const float& _m12, const float& _m13,
			const float& _m20, const float& _m21, const float& _m22, const float& _m23,
			const float& _m30, const float& _m31, const float& _m32, const float& _m33) {
			m00 = _m00;
			m01 = _m01;
			m02 = _m02;
			m03 = _m03;
			m10 = _m10;
			m11 = _m11;
			m12 = _m12;
			m13 = _m13;
			m20 = _m20;
			m21 = _m21;
			m22 = _m22;
			m23 = _m23;
			m30 = _m30;
			m31 = _m31;
			m32 = _m32;
			m33 = _m33;
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
		static Matrix4x4 translation(const Vector3& translation) {
			Matrix4x4 m = IDENTITY;
			m.at(0, 3) = translation.x;
			m.at(1, 3) = translation.y;
			m.at(2, 3) = translation.z;
			return m;
		}

		static Matrix4x4 rotation(const Vector3& axis, const float& theta) {
			float rad = DEGREE2RAD(theta);
			float s = std::sin(rad);
			float c = std::cos(rad);
			Vector3 normalized_axis = Vector3::normalize(axis);
			float x = normalized_axis.x;
			float y = normalized_axis.y;
			float z = normalized_axis.z;
			Matrix4x4 m = IDENTITY;
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

		static Matrix4x4 scale(const Vector3& scale) {
			Matrix4x4 m = IDENTITY;
			m.at(0, 0) = scale.x;
			m.at(1, 1) = scale.y;
			m.at(2, 2) = scale.z;
			return m;
		}

		Vector3 position() const {
			Vector3 pos;
			pos.x = at(0, 3);
			pos.y = at(1, 3);
			pos.z = at(2, 3);
			return pos;
		}

		Vector3 forward() const {
			Vector3 f;
			f.x = at(2, 0);
			f.y = at(2, 1);
			f.z = at(2, 2);
#ifdef LEFT_HANDED
			return f.normalized();
#else
			return -f.normalized();
#endif
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

		static Matrix4x4 viewport(const int& x, const int& y, const int& w, const int& h) {
			Matrix4x4 ret = IDENTITY;
			ret.at(0, 0) = w * 0.5f;
			ret.at(0, 3) = x + (w * 0.5f);
			ret.at(1, 1) = h * 0.5f;
			ret.at(1, 3) = y + (h * 0.5f);
			return ret;
		}

		// yaw-y pitch-x roll-z
		static Matrix4x4 yaw_pitch_roll(const float& yaw, const float& pitch, const float& roll) {
			return euler_angle_z(roll) * euler_angle_x(pitch) * euler_angle_y(yaw);
		}

		static Matrix4x4 euler_angle_x(const float& angle) {
			float rad = DEGREE2RAD(angle);
			float c = std::cos(rad);
			float s = std::sin(rad);
			return Matrix4x4(
				1, 0, 0, 0,
				0, c, s, 0,
				0, -s, c, 0,
				0, 0, 0, 1
			);
		}

		static Matrix4x4 euler_angle_y(const float& angle) {
			float rad = DEGREE2RAD(angle);
			float c = std::cos(rad);
			float s = std::sin(rad);
			return Matrix4x4(
				c, 0, -s, 0,
				0, 1, 0, 0,
				s, 0, c, 0,
				0, 0, 0, 1
			);
		}

		static Matrix4x4 euler_angle_z(const float& angle) {
			float rad = DEGREE2RAD(angle);
			float c = std::cos(rad);
			float s = std::sin(rad);
			return Matrix4x4(
				c, s, 0, 0,
				-s, c, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			);
		}

		static Matrix4x4 lookat(const Vector3& eye, const Vector3& target, const Vector3& world_up) {
#ifdef LEFT_HANDED
			return lookat_lh(eye, target, world_up);
#else
			return lookat_rh(eye, target, world_up);
#endif
		}

		static Matrix4x4 lookat_lh(const Vector3& eye, const Vector3& target, const Vector3& world_up) {
			Vector3 forward = Vector3::normalize(target - eye);
			Vector3 right = Vector3::normalize(Vector3::cross(Vector3::normalize(world_up), forward));
			Vector3 up = Vector3::cross(forward, right);

			// UVN--right up forward
			Matrix4x4 view = Matrix4x4::IDENTITY;
			view.at(0, 0) = right.x;
			view.at(0, 1) = right.y;
			view.at(0, 2) = right.z;
			view.at(1, 0) = up.x;
			view.at(1, 1) = up.y;
			view.at(1, 2) = up.z;
			view.at(2, 0) = forward.x;
			view.at(2, 1) = forward.y;
			view.at(2, 2) = forward.z;
			view.at(0, 3) = -Vector3::dot(right, eye);
			view.at(1, 3) = -Vector3::dot(up, eye);
			view.at(2, 3) = -Vector3::dot(forward, eye);

			return view;
		}

		static Matrix4x4 lookat_rh(const Vector3& eye, const Vector3& target, const Vector3& world_up) {
			Vector3 forward = Vector3::normalize(target - eye);
			Vector3 right = Vector3::normalize(Vector3::cross(forward, Vector3::normalize(world_up))); 
			Vector3 up = Vector3::cross(right, forward); 

			// UVN--right up forward
			Matrix4x4 view = Matrix4x4::IDENTITY;
			view.at(0, 0) = right.x;
			view.at(0, 1) = right.y;
			view.at(0, 2) = right.z;
			view.at(1, 0) = up.x;
			view.at(1, 1) = up.y;
			view.at(1, 2) = up.z;
			view.at(2, 0) = -forward.x;
			view.at(2, 1) = -forward.y;
			view.at(2, 2) = -forward.z;
			view.at(0, 3) = -Vector3::dot(right, eye);
			view.at(1, 3) = -Vector3::dot(up, eye);
			view.at(2, 3) = Vector3::dot(forward, eye);

			return view;
		}

		static Matrix4x4 perspective(const float& fov, const float& aspect, const float& near, const float& far) {
#ifdef LEFT_HANDED
			return perspective_lh(fov, aspect, near, far);
#else
			return perspective_rh(fov, aspect, near, far);
#endif
		}

		static Matrix4x4 perspective_rh(const float& fov, const float& aspect, const float& near, const float& far) {
			float rad = DEGREE2RAD(fov * 0.5f);
			float cot = std::cos(rad) / std::sin(rad);
			Matrix4x4 m = ZERO;
			m.at(0, 0) = cot / aspect;
			m.at(1, 1) = cot;
			m.at(2, 2) = -(far + near) / (far - near);
			m.at(3, 2) = -1.0f;
			m.at(2, 3) = -2.0f * (far * near) / (far - near);
			return m;
		}

		static Matrix4x4 perspective_lh(const float& fov, const float& aspect, const float& near, const float& far) {
			float rad = DEGREE2RAD(fov * 0.5f);
			float cot = std::cos(rad) / std::sin(rad);
			Matrix4x4 m = ZERO;
			m.at(0, 0) = cot / aspect;
			m.at(1, 1) = cot;
			m.at(2, 2) = (far + near) / (far - near);
			m.at(3, 2) = 1.0f;
			m.at(2, 3) = -2.0f * (far * near) / (far - near);
			return m;
		}

		static Matrix4x4 ortho(const float& left, const float& right, const float& bottom, const float& top, const float& near, const float& far) {
#ifdef LEFT_HANDED
			return ortho_lh(left, right, bottom, top, near, far);
#else
			return ortho_rh(left, right, bottom, top, near, far);
#endif
		}

		static Matrix4x4 ortho_lh(const float& left, const float& right, const float& bottom, const float& top, const float& near, const float& far) {
			Matrix4x4 m = IDENTITY;
			m.at(0, 0) = 2.0f / (right - left);
			m.at(1, 1) = 2.0f / (top - bottom);
			m.at(2, 2) = - 2.0f / (far - near);
			m.at(0, 3) = -(right + left) / 2.0f;
			m.at(1, 3) = -(top + bottom) / 2.0f;
			m.at(2, 3) = - (far + near) / 2.0f;
			return m;
		}

		static Matrix4x4 ortho_rh(const float& left, const float& right, const float& bottom, const float& top, const float& near, const float& far) {
			Matrix4x4 m = IDENTITY;
			m.at(0, 0) = 2.0f / (right - left);
			m.at(1, 1) = 2.0f / (top - bottom);
			m.at(2, 2) = -2.0f / (far - near);
			m.at(0, 3) = -(right + left) / 2.0f;
			m.at(1, 3) = -(top + bottom) / 2.0f;
			m.at(2, 3) = -(far + near) / 2.0f;
			return m;
		}

		Vector3 transform_point(const Vector3& point) const {
			Vector3 ret;
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

		Vector3 transform_direction(const Vector3& point) const {
			Vector3 ret;
			ret.x = m00 * point.x + m01 * point.y + m02 * point.z;
			ret.y = m10 * point.x + m11 * point.y + m12 * point.z;
			ret.z = m20 * point.x + m21 * point.y + m22 * point.z;
			return ret;
		}

		Matrix4x4 operator *(const Matrix4x4& rhs) const {
			Matrix4x4 ret;
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

		Matrix4x4 transpose() const {
			Matrix4x4 ret = Matrix4x4(*this);
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
		Matrix4x4 inverse() const
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
				return Matrix4x4::ZERO;

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
				return Matrix4x4::ZERO;

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
				return Matrix4x4::ZERO;

			m3 = r3[2] / r2[2];
			r3[3] -= m3 * r2[3];
			r3[4] -= m3 * r2[4];
			r3[5] -= m3 * r2[5];
			r3[6] -= m3 * r2[6];
			r3[7] -= m3 * r2[7];

			if (0.0f == r3[3])
				return Matrix4x4::ZERO;

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

			Matrix4x4 out;
			out.at(0, 0) = r0[4]; out.at(0, 1) = r0[5], out.at(0, 2) = r0[6]; out.at(0, 3) = r0[7];
			out.at(1, 0) = r1[4]; out.at(1, 1) = r1[5], out.at(1, 2) = r1[6]; out.at(1, 3) = r1[7];
			out.at(2, 0) = r2[4]; out.at(2, 1) = r2[5], out.at(2, 2) = r2[6]; out.at(2, 3) = r2[7];
			out.at(3, 0) = r3[4]; out.at(3, 1) = r3[5], out.at(3, 2) = r3[6]; out.at(3, 3) = r3[7];
			return out;
		}

		Vector4 operator *(const Vector4& vector) const {
			Vector4 ret;
			ret.x = m00 * vector.x + m01 * vector.y + m02 * vector.z + m03 * vector.w;
			ret.y = m10 * vector.x + m11 * vector.y + m12 * vector.z + m13 * vector.w;
			ret.z = m20 * vector.x + m21 * vector.y + m22 * vector.z + m23 * vector.w;
			ret.w = m30 * vector.x + m31 * vector.y + m32 * vector.z + m33 * vector.w;
			return ret;
		}

		bool operator ==(const Matrix4x4& rhs) const {
			return !(*this != rhs);
		}

		bool operator !=(const Matrix4x4& rhs) const {
			for (int idx = 0; idx < 4; idx++) {
				if (column(idx) != rhs.column(idx)) {
					return true;
				}
			}
			return false;
		}

		Vector4 row(const int& index) const {
			Vector4 ret;
			switch (index)
			{
			case 0:
				ret = Vector4(m00, m01, m02, m03);
				break;
			case 1:
				ret = Vector4(m10, m11, m12, m13);
				break;
			case 2:
				ret = Vector4(m20, m21, m22, m23);
				break;
			case 3:
				ret = Vector4(m30, m31, m32, m33);
				break;
			default:
				std::cerr << "index out of range: " << index << std::endl;
			}
			return ret;
		}

		Vector4 column(const int& index) const {
			Vector4 ret;
			switch (index)
			{
			case 0:
				ret = Vector4(m00, m10, m20, m30);
				break;
			case 1:
				ret = Vector4(m01, m11, m21, m31);
				break;
			case 2:
				ret = Vector4(m02, m12, m22, m32);
				break;
			case 3:
				ret = Vector4(m03, m13, m23, m33);
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
				return m03;
			case 4:
				return m10;
			case 5:
				return m11;
			case 6:
				return m12;
			case 7:
				return m13;
			case 8:
				return m20;
			case 9:
				return m21;
			case 10:
				return m22;
			case 11:
				return m23;
			case 12:
				return m30;
			case 13:
				return m31;
			case 14:
				return m32;
			case 15:
				return m33;
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
				return m03;
			case 4:
				return m10;
			case 5:
				return m11;
			case 6:
				return m12;
			case 7:
				return m13;
			case 8:
				return m20;
			case 9:
				return m21;
			case 10:
				return m22;
			case 11:
				return m23;
			case 12:
				return m30;
			case 13:
				return m31;
			case 14:
				return m32;
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
			return row * 4 + column;
		}
	};

	const Matrix4x4 Matrix4x4::IDENTITY(Vector4(1.0f, 0.0f, 0.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	const Matrix4x4 Matrix4x4::ZERO;

	static std::ostream& operator << (std::ostream& stream, const Matrix4x4& mat) {
		stream << mat.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Matrix4x4& mat) {
		stream << mat.str();
		return stream;
	}
}
#endif
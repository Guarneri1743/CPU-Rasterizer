#ifndef _MAT4_
#define _MAT4_
#include <sstream>
#include <ostream>
#include <string>
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace Guarneri {
	struct Matrix4x4 {
	public:
		static const Matrix4x4 IDENTITY;
		static const Matrix4x4 ZERO;
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
		Matrix4x4();
		Matrix4x4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3);
		Matrix4x4(const float& _m00, const float& _m01, const float& _m02, const float& _m03,
				  const float& _m10, const float& _m11, const float& _m12, const float& _m13,
				  const float& _m20, const float& _m21, const float& _m22, const float& _m23,
				  const float& _m30, const float& _m31, const float& _m32, const float& _m33);

		static Matrix4x4 translation(const Vector3& translation);
		static Matrix4x4 rotation(const Vector3& axis, const float& theta);
		static Matrix4x4 scale(const Vector3& scale);
		static Matrix4x4 viewport(const int& x, const int& y, const int& w, const int& h);
		static Matrix4x4 yaw_pitch_roll(const float& yaw, const float& pitch, const float& roll);
		static Matrix4x4 euler_angle_x(const float& angle);
		static Matrix4x4 euler_angle_y(const float& angle);
		static Matrix4x4 euler_angle_z(const float& angle);
		static Matrix4x4 lookat(const Vector3& eye, const Vector3& target, const Vector3& world_up);
		static Matrix4x4 lookat_lh(const Vector3& eye, const Vector3& target, const Vector3& world_up);
		static Matrix4x4 lookat_rh(const Vector3& eye, const Vector3& target, const Vector3& world_up);
		static Matrix4x4 perspective(const float& fov, const float& aspect, const float& near, const float& far);
		static Matrix4x4 perspective_rh_dx(const float& fov, const float& aspect, const float& near, const float& far);
		static Matrix4x4 perspective_lh_dx(const float& fov, const float& aspect, const float& near, const float& far);
		static Matrix4x4 perspective_rh_gl(const float& fov, const float& aspect, const float& near, const float& far);
		static Matrix4x4 perspective_lh_gl(const float& fov, const float& aspect, const float& near, const float& far);
		static Matrix4x4 ortho(const float& left, const float& right, const float& bottom, const float& top, const float& near, const float& far);
		static Matrix4x4 ortho_rh_dx(const float& left, const float& right, const float& bottom, const float& top, const float& near, const float& far);
		static Matrix4x4 ortho_lh_dx(const float& left, const float& right, const float& bottom, const float& top, const float& near, const float& far);
		static Matrix4x4 ortho_rh_gl(const float& left, const float& right, const float& bottom, const float& top, const float& near, const float& far);
		static Matrix4x4 ortho_lh_gl(const float& left, const float& right, const float& bottom, const float& top, const float& near, const float& far);

		Vector3 position() const;
		Vector3 forward() const;
		Vector3 up() const;
		Vector3 right() const;
		Vector3 transform_point(const Vector3& point) const;
		Vector3 transform_direction(const Vector3& point) const;
		Matrix4x4 operator *(const Matrix4x4& rhs) const;
		Matrix4x4 transpose() const;
		Matrix4x4 inverse() const;

		Vector4 operator *(const Vector4& vector) const;
		bool operator ==(const Matrix4x4& rhs) const;
		bool operator !=(const Matrix4x4& rhs) const;

		Vector4 row(const int& index) const;
		Vector4 column(const int& index) const;

		const float& at(const int& row, const int& column) const;
		float& at(const int& row, const int& column);
		const float& operator[](const int& index) const;
		float& operator[](const int& index);

		int rc2index(const int& row, const int& column) const;
		std::string str() const;
	};

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
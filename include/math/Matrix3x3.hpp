#ifndef _MAT3_
#define _MAT3_
#include <string>
#include "Vector3.hpp"
#include "Matrix4x4.hpp"
#include "rapidjson/document.h"

namespace Guarneri
{
	struct Matrix3x3
	{
	public:
		static const Matrix3x3 IDENTITY;
		static const Matrix3x3 ZERO;
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
		Matrix3x3();
		Matrix3x3(const Matrix4x4& mat);
		Matrix3x3(const Vector3& row0, const Vector3& row1, const Vector3& row2);
		Matrix3x3(const float& _m00, const float& _m01, const float& _m02,
				  const float& _m10, const float& _m11, const float& _m12,
				  const float& _m20, const float& _m21, const float& _m22);

		static Matrix3x3 rotation(const Vector3& axis, const float& theta);
		static Matrix3x3 scale(const Vector3& scale);
		
		Vector3 forward() const;
		Vector3 up() const;
		Vector3 right() const;
		Matrix3x3 operator *(const Matrix3x3& rhs) const;
		Matrix3x3 transpose() const;
		Matrix3x3 inverse() const;

		Vector3 operator *(const Vector3& vector) const;
		bool operator ==(const Matrix3x3& rhs) const;
		bool operator !=(const Matrix3x3& rhs) const;

		Vector3 row(const int& index) const;
		Vector3 column(const int& index) const;

		const float& at(const int& row, const int& column) const;
		float& at(const int& row, const int& column);

		const float& operator[](const int& index) const;
		float& operator[](const int& index);

		int rc2index(const int& row, const int& column) const;

		std::string str() const;
	};
}
#endif
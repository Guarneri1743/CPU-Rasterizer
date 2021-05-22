#include "Vector4.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>

namespace Guarneri
{
	Vector4::Vector4()
	{
		x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f;
	}

	Vector4::Vector4(const float& r)
	{
		this->x = r; this->y = r; this->z = r; this->w = r;
	}

	Vector4::Vector4(const Vector3& v)
	{
		this->x = v.x; this->y = v.y; this->z = v.z; this->w = 1.0f;
	}

	Vector4::Vector4(const float& x, const Vector3& yzw)
	{
		this->x = x; this->y = yzw.x; this->z = yzw.y; this->w = yzw.z;
	}

	Vector4::Vector4(const float& x, const float& y, const Vector2& zw)
	{
		this->x = x; this->y = y; this->z = zw.x; this->w = zw.y;
	}

	Vector4::Vector4(const Vector2& v, const float& z, const float& w)
	{
		this->x = v.x; this->y = v.y; this->z = z; this->w = w;
	}

	Vector4::Vector4(const Vector2& v, const Vector2& zw)
	{
		this->x = v.x; this->y = v.y; this->z = zw.x; this->w = zw.y;
	}

	Vector4::Vector4(const Vector3& v, const float& w)
	{
		this->x = v.x; this->y = v.y; this->z = v.z; this->w = w;
	}

	Vector4::Vector4(const float& x, const float& y, const float& z)
	{
		this->x = x; this->y = y; this->z = z; this->w = 1.0f;
	}

	Vector4::Vector4(const float& x, const float& y, const float& z, const float& w)
	{
		this->x = x; this->y = y; this->z = z; this->w = w;
	}


	Vector4 Vector4::normalized()
	{
		return Vector4::normalize(*this);
	}

	float& Vector4::operator[](const unsigned int& i)
	{
		return (&x)[i];
	}

	const float& Vector4::operator[](const unsigned int& i) const
	{
		return (&x)[i];
	}

	bool Vector4::operator == (const Vector4& other)
	{
		return EQUALS(x, other.x) && EQUALS(y, other.y) && EQUALS(z, other.z) && EQUALS(w, other.w);
	}

	bool Vector4::operator != (const Vector4& other)
	{
		return !(*this == other);
	}

	Vector4& Vector4::operator +=(const Vector4& other)
	{
		x = x + other.x;
		y = y + other.y;
		z = z + other.z;
		w = w + other.w;
		return *this;
	}

	Vector4& Vector4::operator -=(const Vector4& other)
	{
		x = x - other.x;
		y = y - other.y;
		z = z - other.z;
		w = z - other.w;
		return *this;
	}

	Vector4& Vector4::operator /=(const float& val)
	{
		x = x / val;
		y = y / val;
		z = z / val;
		w = w / val;
		return *this;
	}

	Vector4& Vector4::operator *=(const float& val)
	{
		x = x * val;
		y = y * val;
		z = z * val;
		w = w * val;
		return *this;
	}

	Vector4 Vector4::operator +(const Vector4& other) const
	{
		Vector4 ret;
		ret.x = this->x + other.x;
		ret.y = this->y + other.y;
		ret.z = this->z + other.z;
		ret.w = this->w + other.w;
		return ret;
	}

	Vector4 Vector4::operator -(const Vector4& other) const
	{
		Vector4 ret;
		ret.x = this->x - other.x;
		ret.y = this->y - other.y;
		ret.z = this->z - other.z;
		ret.w = this->w - other.w;
		return ret;
	}

	Vector4 Vector4::operator *(const float& scale) const
	{
		Vector4 ret;
		ret.x = this->x * scale;
		ret.y = this->y * scale;
		ret.z = this->z * scale;
		ret.w = this->w * scale;
		return ret;
	}

	Vector4 Vector4::operator +(const float& other) const
	{
		Vector4 ret;
		ret.x = this->x + other;
		ret.y = this->y + other;
		ret.z = this->z + other;
		ret.w = this->w + other;
		return ret;
	}

	Vector4 Vector4::operator -(const float& other) const
	{
		Vector4 ret;
		ret.x = this->x - other;
		ret.y = this->y - other;
		ret.z = this->z - other;
		ret.w = this->w - other;
		return ret;
	}

	Vector4 Vector4::operator -() const
	{
		return Vector4(-x, -y, -z, -w);
	}

	Vector4 Vector4::operator *(const Vector4& b) const
	{
		return Vector4(x * b.x, y * b.y, z * b.z, w * b.w);
	}

	Vector4& Vector4::operator *=(const Vector4& b)
	{
		x *= b.x;
		y *= b.y;
		z *= b.z;
		w *= b.w;
		return *this;
	}

	Vector4 Vector4::operator /(const float& val) const
	{
		Vector4 ret;
		ret.x = this->x / val;
		ret.y = this->y / val;
		ret.z = this->z / val;
		ret.w = this->w / val;
		return ret;
	}

	Vector4 Vector4::operator /(const Vector4& val) const
	{
		Vector4 ret;
		ret.x = this->x / val.x;
		ret.y = this->y / val.y;
		ret.z = this->z / val.z;
		ret.w = this->w / val.w;
		return ret;
	}

	float Vector4::dot(const Vector4& lhs, const Vector4& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * lhs.w;
	}

	float Vector4::magnitude(const Vector4& vector)
	{
		return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w);
	}

	float Vector4::length(const Vector4& a, const Vector4& b)
	{
		return magnitude(a - b);
	}

	Vector4 Vector4::maximum(const Vector4& a, const Vector4& b)
	{
		return Vector4(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w));
	}

	Vector4 Vector4::minimum(const Vector4& a, const Vector4& b)
	{
		return Vector4(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w));
	}

	Vector4 Vector4::abs(const Vector4& v)
	{
		return Vector4(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
	}

	Vector4 Vector4::normalize(const Vector4& value)
	{
		float num = magnitude(value);
		Vector4 retult;
		if (num > EPSILON)
		{
			retult = value / num;
		}
		else
		{
			retult = Vector4();
		}
		return retult;
	}

	rapidjson::Value Vector4::serialize(rapidjson::Document& doc, const Vector4& vec)
	{
		rapidjson::Value v;
		v.SetObject();
		v.AddMember("x", vec.x, doc.GetAllocator());
		v.AddMember("y", vec.y, doc.GetAllocator());
		v.AddMember("z", vec.z, doc.GetAllocator());
		v.AddMember("w", vec.z, doc.GetAllocator());
		return v;
	}

	Vector4 Vector4::deserialize(const rapidjson::Value& v)
	{
		Vector4 vec;
		vec.x = v["x"].GetFloat();
		vec.y = v["y"].GetFloat();
		vec.z = v["z"].GetFloat();
		vec.w = v["w"].GetFloat();
		return vec;
	}

	std::string Vector4::str() const
	{
		std::stringstream ss;
		ss << "(" << std::setprecision(FLOAT_LOG_PRECISION) << this->x << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->y << ", " << std::setprecision(FLOAT_LOG_PRECISION) << this->z << ", " << this->w << ")";
		return ss.str();
	}

	Vector4 Vector4::xxxx() const
	{
		return Vector4(x, x, x, x);
	}

	Vector4 Vector4::xxxy() const
	{
		return Vector4(x, x, x, y);
	}

	Vector4 Vector4::xxxz() const
	{
		return Vector4(x, x, x, z);
	}

	Vector4 Vector4::xxxw() const
	{
		return Vector4(x, x, x, w);
	}

	Vector4 Vector4::xxyx() const
	{
		return Vector4(x, x, y, x);
	}

	Vector4 Vector4::xxyy() const
	{
		return Vector4(x, x, y, y);
	}

	Vector4 Vector4::xxyz() const
	{
		return Vector4(x, x, y, z);
	}

	Vector4 Vector4::xxyw() const
	{
		return Vector4(x, x, y, w);
	}

	Vector4 Vector4::xxzx() const
	{
		return Vector4(x, x, z, x);
	}

	Vector4 Vector4::xxzy() const
	{
		return Vector4(x, x, z, y);
	}

	Vector4 Vector4::xxzz() const
	{
		return Vector4(x, x, z, z);
	}

	Vector4 Vector4::xxzw() const
	{
		return Vector4(x, x, z, w);
	}

	Vector4 Vector4::xyxx() const
	{
		return Vector4(x, y, x, x);
	}

	Vector4 Vector4::xyxy() const
	{
		return Vector4(x, y, x, y);
	}

	Vector4 Vector4::xyxz() const
	{
		return Vector4(x, y, x, z);
	}

	Vector4 Vector4::xyxw() const
	{
		return Vector4(x, y, x, w);
	}

	Vector4 Vector4::xyyx() const
	{
		return Vector4(x, y, y, x);
	}

	Vector4 Vector4::xyyy() const
	{
		return Vector4(x, y, y, y);
	}

	Vector4 Vector4::xyyz() const
	{
		return Vector4(x, y, y, z);
	}

	Vector4 Vector4::xyyw() const
	{
		return Vector4(x, y, y, w);
	}

	Vector4 Vector4::xyzx() const
	{
		return Vector4(x, y, z, x);
	}

	Vector4 Vector4::xyzy() const
	{
		return Vector4(x, y, z, y);
	}

	Vector4 Vector4::xyzz() const
	{
		return Vector4(x, y, z, z);
	}

	Vector4 Vector4::xyzw() const
	{
		return Vector4(x, y, z, w);
	}

	Vector4 Vector4::xzxx() const
	{
		return Vector4(x, z, x, x);
	}

	Vector4 Vector4::xzxy() const
	{
		return Vector4(x, z, x, y);
	}

	Vector4 Vector4::xzxz() const
	{
		return Vector4(x, z, x, z);
	}

	Vector4 Vector4::xzxw() const
	{
		return Vector4(x, z, x, w);
	}

	Vector4 Vector4::xzyx() const
	{
		return Vector4(x, z, y, x);
	}

	Vector4 Vector4::xzyy() const
	{
		return Vector4(x, z, y, y);
	}

	Vector4 Vector4::xzyz() const
	{
		return Vector4(x, z, y, z);
	}

	Vector4 Vector4::xzyw() const
	{
		return Vector4(x, z, y, w);
	}

	Vector4 Vector4::xzzx() const
	{
		return Vector4(x, z, z, x);
	}

	Vector4 Vector4::xzzy() const
	{
		return Vector4(x, z, z, y);
	}

	Vector4 Vector4::xzzz() const
	{
		return Vector4(x, z, z, z);
	}

	Vector4 Vector4::xzzw() const
	{
		return Vector4(x, z, z, w);
	}

	Vector4 Vector4::xwxx() const
	{
		return Vector4(x, w, x, x);
	}

	Vector4 Vector4::xwxy() const
	{
		return Vector4(x, w, x, y);
	}

	Vector4 Vector4::xwxz() const
	{
		return Vector4(x, w, x, z);
	}

	Vector4 Vector4::xwxw() const
	{
		return Vector4(x, w, x, w);
	}

	Vector4 Vector4::xwyx() const
	{
		return Vector4(x, w, y, x);
	}

	Vector4 Vector4::xwyy() const
	{
		return Vector4(x, w, y, y);
	}

	Vector4 Vector4::xwyz() const
	{
		return Vector4(x, w, y, z);
	}

	Vector4 Vector4::xwyw() const
	{
		return Vector4(x, w, y, w);
	}

	Vector4 Vector4::xwzx() const
	{
		return Vector4(x, w, z, x);
	}

	Vector4 Vector4::xwzy() const
	{
		return Vector4(x, w, z, y);
	}

	Vector4 Vector4::xwzz() const
	{
		return Vector4(x, w, z, z);
	}

	Vector4 Vector4::xwzw() const
	{
		return Vector4(x, w, z, w);
	}

	Vector4 Vector4::yxxx() const
	{
		return Vector4(y, x, x, x);
	}

	Vector4 Vector4::yxxy() const
	{
		return Vector4(y, x, x, y);
	}

	Vector4 Vector4::yxxz() const
	{
		return Vector4(y, x, x, z);
	}

	Vector4 Vector4::yxxw() const
	{
		return Vector4(y, x, x, w);
	}

	Vector4 Vector4::yxyx() const
	{
		return Vector4(y, x, y, x);
	}

	Vector4 Vector4::yxyy() const
	{
		return Vector4(y, x, y, y);
	}

	Vector4 Vector4::yxyz() const
	{
		return Vector4(y, x, y, z);
	}

	Vector4 Vector4::yxyw() const
	{
		return Vector4(y, x, y, w);
	}

	Vector4 Vector4::yxzx() const
	{
		return Vector4(y, x, z, x);
	}

	Vector4 Vector4::yxzy() const
	{
		return Vector4(y, x, z, y);
	}

	Vector4 Vector4::yxzz() const
	{
		return Vector4(y, x, z, z);
	}

	Vector4 Vector4::yxzw() const
	{
		return Vector4(y, x, z, w);
	}

	Vector4 Vector4::yyxx() const
	{
		return Vector4(y, y, x, x);
	}

	Vector4 Vector4::yyxy() const
	{
		return Vector4(y, y, x, y);
	}

	Vector4 Vector4::yyxz() const
	{
		return Vector4(y, y, x, z);
	}

	Vector4 Vector4::yyxw() const
	{
		return Vector4(y, y, x, w);
	}

	Vector4 Vector4::yyyx() const
	{
		return Vector4(y, y, y, x);
	}

	Vector4 Vector4::yyyy() const
	{
		return Vector4(y, y, y, y);
	}

	Vector4 Vector4::yyyz() const
	{
		return Vector4(y, y, y, z);
	}

	Vector4 Vector4::yyyw() const
	{
		return Vector4(y, y, y, w);
	}

	Vector4 Vector4::yyzx() const
	{
		return Vector4(y, y, z, x);
	}

	Vector4 Vector4::yyzy() const
	{
		return Vector4(y, y, z, y);
	}

	Vector4 Vector4::yyzz() const
	{
		return Vector4(y, y, z, z);
	}

	Vector4 Vector4::yyzw() const
	{
		return Vector4(y, y, z, w);
	}

	Vector4 Vector4::yzxx() const
	{
		return Vector4(y, z, x, x);
	}

	Vector4 Vector4::yzxy() const
	{
		return Vector4(y, z, x, y);
	}

	Vector4 Vector4::yzxz() const
	{
		return Vector4(y, z, x, z);
	}

	Vector4 Vector4::yzxw() const
	{
		return Vector4(y, z, x, w);
	}

	Vector4 Vector4::yzyx() const
	{
		return Vector4(y, z, y, x);
	}

	Vector4 Vector4::yzyy() const
	{
		return Vector4(y, z, y, y);
	}

	Vector4 Vector4::yzyz() const
	{
		return Vector4(y, z, y, z);
	}

	Vector4 Vector4::yzyw() const
	{
		return Vector4(y, z, y, w);
	}

	Vector4 Vector4::yzzx() const
	{
		return Vector4(y, z, z, x);
	}

	Vector4 Vector4::yzzy() const
	{
		return Vector4(y, z, z, y);
	}

	Vector4 Vector4::yzzz() const
	{
		return Vector4(y, z, z, z);
	}

	Vector4 Vector4::yzzw() const
	{
		return Vector4(y, z, z, w);
	}

	Vector4 Vector4::ywxx() const
	{
		return Vector4(y, w, x, x);
	}

	Vector4 Vector4::ywxy() const
	{
		return Vector4(y, w, x, y);
	}

	Vector4 Vector4::ywxz() const
	{
		return Vector4(y, w, x, z);
	}

	Vector4 Vector4::ywxw() const
	{
		return Vector4(y, w, x, w);
	}

	Vector4 Vector4::ywyx() const
	{
		return Vector4(y, w, y, x);
	}

	Vector4 Vector4::ywyy() const
	{
		return Vector4(y, w, y, y);
	}

	Vector4 Vector4::ywyz() const
	{
		return Vector4(y, w, y, z);
	}

	Vector4 Vector4::ywyw() const
	{
		return Vector4(y, w, y, w);
	}

	Vector4 Vector4::ywzx() const
	{
		return Vector4(y, w, z, x);
	}

	Vector4 Vector4::ywzy() const
	{
		return Vector4(y, w, z, y);
	}

	Vector4 Vector4::ywzz() const
	{
		return Vector4(y, w, z, z);
	}

	Vector4 Vector4::ywzw() const
	{
		return Vector4(y, w, z, w);
	}

	Vector4 Vector4::zxxx() const
	{
		return Vector4(z, x, x, x);
	}

	Vector4 Vector4::zxxy() const
	{
		return Vector4(z, x, x, y);
	}

	Vector4 Vector4::zxxz() const
	{
		return Vector4(z, x, x, z);
	}

	Vector4 Vector4::zxxw() const
	{
		return Vector4(z, x, x, w);
	}

	Vector4 Vector4::zxyx() const
	{
		return Vector4(z, x, y, x);
	}

	Vector4 Vector4::zxyy() const
	{
		return Vector4(z, x, y, y);
	}

	Vector4 Vector4::zxyz() const
	{
		return Vector4(z, x, y, z);
	}

	Vector4 Vector4::zxyw() const
	{
		return Vector4(z, x, y, w);
	}

	Vector4 Vector4::zxzx() const
	{
		return Vector4(z, x, z, x);
	}

	Vector4 Vector4::zxzy() const
	{
		return Vector4(z, x, z, y);
	}

	Vector4 Vector4::zxzz() const
	{
		return Vector4(z, x, z, z);
	}

	Vector4 Vector4::zxzw() const
	{
		return Vector4(z, x, z, w);
	}

	Vector4 Vector4::zyxx() const
	{
		return Vector4(z, y, x, x);
	}

	Vector4 Vector4::zyxy() const
	{
		return Vector4(z, y, x, y);
	}

	Vector4 Vector4::zyxz() const
	{
		return Vector4(z, y, x, z);
	}

	Vector4 Vector4::zyxw() const
	{
		return Vector4(z, y, x, w);
	}

	Vector4 Vector4::zyyx() const
	{
		return Vector4(z, y, y, x);
	}

	Vector4 Vector4::zyyy() const
	{
		return Vector4(z, y, y, y);
	}

	Vector4 Vector4::zyyz() const
	{
		return Vector4(z, y, y, z);
	}

	Vector4 Vector4::zyyw() const
	{
		return Vector4(z, y, y, w);
	}

	Vector4 Vector4::zyzx() const
	{
		return Vector4(z, y, z, x);
	}

	Vector4 Vector4::zyzy() const
	{
		return Vector4(z, y, z, y);
	}

	Vector4 Vector4::zyzz() const
	{
		return Vector4(z, y, z, z);
	}

	Vector4 Vector4::zyzw() const
	{
		return Vector4(z, y, z, w);
	}

	Vector4 Vector4::zzxx() const
	{
		return Vector4(z, z, x, x);
	}

	Vector4 Vector4::zzxy() const
	{
		return Vector4(z, z, x, y);
	}

	Vector4 Vector4::zzxz() const
	{
		return Vector4(z, z, x, z);
	}

	Vector4 Vector4::zzxw() const
	{
		return Vector4(z, z, x, w);
	}

	Vector4 Vector4::zzyx() const
	{
		return Vector4(z, z, y, x);
	}

	Vector4 Vector4::zzyy() const
	{
		return Vector4(z, z, y, y);
	}

	Vector4 Vector4::zzyz() const
	{
		return Vector4(z, z, y, z);
	}

	Vector4 Vector4::zzyw() const
	{
		return Vector4(z, z, y, w);
	}

	Vector4 Vector4::zzzx() const
	{
		return Vector4(z, z, z, x);
	}

	Vector4 Vector4::zzzy() const
	{
		return Vector4(z, z, z, y);
	}

	Vector4 Vector4::zzzz() const
	{
		return Vector4(z, z, z, z);
	}

	Vector4 Vector4::zzzw() const
	{
		return Vector4(z, z, z, w);
	}

	Vector4 Vector4::zwxx() const
	{
		return Vector4(z, w, x, x);
	}

	Vector4 Vector4::zwxy() const
	{
		return Vector4(z, w, x, y);
	}

	Vector4 Vector4::zwxz() const
	{
		return Vector4(z, w, x, z);
	}

	Vector4 Vector4::zwxw() const
	{
		return Vector4(z, w, x, w);
	}

	Vector4 Vector4::zwyx() const
	{
		return Vector4(z, w, y, x);
	}

	Vector4 Vector4::zwyy() const
	{
		return Vector4(z, w, y, y);
	}

	Vector4 Vector4::zwyz() const
	{
		return Vector4(z, w, y, z);
	}

	Vector4 Vector4::zwyw() const
	{
		return Vector4(z, w, y, w);
	}

	Vector4 Vector4::zwzx() const
	{
		return Vector4(z, w, z, x);
	}

	Vector4 Vector4::zwzy() const
	{
		return Vector4(z, w, z, y);
	}

	Vector4 Vector4::zwzz() const
	{
		return Vector4(z, w, z, z);
	}

	Vector4 Vector4::zwzw() const
	{
		return Vector4(z, w, z, w);
	}

	Vector4 Vector4::wxxx() const
	{
		return Vector4(w, x, x, x);
	}

	Vector4 Vector4::wxxy() const
	{
		return Vector4(w, x, x, y);
	}

	Vector4 Vector4::wxxz() const
	{
		return Vector4(w, x, x, z);
	}

	Vector4 Vector4::wxxw() const
	{
		return Vector4(w, x, x, w);
	}

	Vector4 Vector4::wxyx() const
	{
		return Vector4(w, x, y, x);
	}

	Vector4 Vector4::wxyy() const
	{
		return Vector4(w, x, y, y);
	}

	Vector4 Vector4::wxyz() const
	{
		return Vector4(w, x, y, z);
	}

	Vector4 Vector4::wxyw() const
	{
		return Vector4(w, x, y, w);
	}

	Vector4 Vector4::wxzx() const
	{
		return Vector4(w, x, z, x);
	}

	Vector4 Vector4::wxzy() const
	{
		return Vector4(w, x, z, y);
	}

	Vector4 Vector4::wxzz() const
	{
		return Vector4(w, x, z, z);
	}

	Vector4 Vector4::wxzw() const
	{
		return Vector4(w, x, z, w);
	}

	Vector4 Vector4::wyxx() const
	{
		return Vector4(w, y, x, x);
	}

	Vector4 Vector4::wyxy() const
	{
		return Vector4(w, y, x, y);
	}

	Vector4 Vector4::wyxz() const
	{
		return Vector4(w, y, x, z);
	}

	Vector4 Vector4::wyxw() const
	{
		return Vector4(w, y, x, w);
	}

	Vector4 Vector4::wyyx() const
	{
		return Vector4(w, y, y, x);
	}

	Vector4 Vector4::wyyy() const
	{
		return Vector4(w, y, y, y);
	}

	Vector4 Vector4::wyyz() const
	{
		return Vector4(w, y, y, z);
	}

	Vector4 Vector4::wyyw() const
	{
		return Vector4(w, y, y, w);
	}

	Vector4 Vector4::wyzx() const
	{
		return Vector4(w, y, z, x);
	}

	Vector4 Vector4::wyzy() const
	{
		return Vector4(w, y, z, y);
	}

	Vector4 Vector4::wyzz() const
	{
		return Vector4(w, y, z, z);
	}

	Vector4 Vector4::wyzw() const
	{
		return Vector4(w, y, z, w);
	}

	Vector4 Vector4::wzxx() const
	{
		return Vector4(w, z, x, x);
	}

	Vector4 Vector4::wzxy() const
	{
		return Vector4(w, z, x, y);
	}

	Vector4 Vector4::wzxz() const
	{
		return Vector4(w, z, x, z);
	}

	Vector4 Vector4::wzxw() const
	{
		return Vector4(w, z, x, w);
	}

	Vector4 Vector4::wzyx() const
	{
		return Vector4(w, z, y, x);
	}

	Vector4 Vector4::wzyy() const
	{
		return Vector4(w, z, y, y);
	}

	Vector4 Vector4::wzyz() const
	{
		return Vector4(w, z, y, z);
	}

	Vector4 Vector4::wzyw() const
	{
		return Vector4(w, z, y, w);
	}

	Vector4 Vector4::wzzx() const
	{
		return Vector4(w, z, z, x);
	}

	Vector4 Vector4::wzzy() const
	{
		return Vector4(w, z, z, y);
	}

	Vector4 Vector4::wzzz() const
	{
		return Vector4(w, z, z, z);
	}

	Vector4 Vector4::wzzw() const
	{
		return Vector4(w, z, z, w);
	}

	Vector4 Vector4::wwxx() const
	{
		return Vector4(w, w, x, x);
	}

	Vector4 Vector4::wwxy() const
	{
		return Vector4(w, w, x, y);
	}

	Vector4 Vector4::wwxz() const
	{
		return Vector4(w, w, x, z);
	}

	Vector4 Vector4::wwxw() const
	{
		return Vector4(w, w, x, w);
	}

	Vector4 Vector4::wwyx() const
	{
		return Vector4(w, w, y, x);
	}

	Vector4 Vector4::wwyy() const
	{
		return Vector4(w, w, y, y);
	}

	Vector4 Vector4::wwyz() const
	{
		return Vector4(w, w, y, z);
	}

	Vector4 Vector4::wwyw() const
	{
		return Vector4(w, w, y, w);
	}

	Vector4 Vector4::wwzx() const
	{
		return Vector4(w, w, z, x);
	}

	Vector4 Vector4::wwzy() const
	{
		return Vector4(w, w, z, y);
	}

	Vector4 Vector4::wwzz() const
	{
		return Vector4(w, w, z, z);
	}

	Vector4 Vector4::wwzw() const
	{
		return Vector4(w, w, z, w);
	}

	Vector3 Vector4::xxx() const
	{
		return Vector3(x, x, x);
	}

	Vector3 Vector4::xxy() const
	{
		return Vector3(x, x, y);
	}

	Vector3 Vector4::xxz() const
	{
		return Vector3(x, x, z);
	}

	Vector3 Vector4::xxw() const
	{
		return Vector3(x, x, w);
	}

	Vector3 Vector4::xyx() const
	{
		return Vector3(x, y, x);
	}

	Vector3 Vector4::xyy() const
	{
		return Vector3(x, y, y);
	}

	Vector3 Vector4::xyz() const
	{
		return Vector3(x, y, z);
	}

	Vector3 Vector4::xyw() const
	{
		return Vector3(x, y, w);
	}

	Vector3 Vector4::xzx() const
	{
		return Vector3(x, z, x);
	}

	Vector3 Vector4::xzy() const
	{
		return Vector3(x, z, y);
	}

	Vector3 Vector4::xzz() const
	{
		return Vector3(x, z, z);
	}

	Vector3 Vector4::xzw() const
	{
		return Vector3(x, z, w);
	}

	Vector3 Vector4::yxx() const
	{
		return Vector3(y, x, x);
	}

	Vector3 Vector4::yxy() const
	{
		return Vector3(y, x, y);
	}

	Vector3 Vector4::yxz() const
	{
		return Vector3(y, x, z);
	}

	Vector3 Vector4::yxw() const
	{
		return Vector3(y, x, w);
	}

	Vector3 Vector4::yyx() const
	{
		return Vector3(y, y, x);
	}

	Vector3 Vector4::yyy() const
	{
		return Vector3(y, y, y);
	}

	Vector3 Vector4::yyz() const
	{
		return Vector3(y, y, z);
	}

	Vector3 Vector4::yyw() const
	{
		return Vector3(y, y, w);
	}

	Vector3 Vector4::yzx() const
	{
		return Vector3(y, z, x);
	}

	Vector3 Vector4::yzy() const
	{
		return Vector3(y, z, y);
	}

	Vector3 Vector4::yzz() const
	{
		return Vector3(y, z, z);
	}

	Vector3 Vector4::yzw() const
	{
		return Vector3(y, z, w);
	}

	Vector3 Vector4::zxx() const
	{
		return Vector3(z, x, x);
	}

	Vector3 Vector4::zxy() const
	{
		return Vector3(z, x, y);
	}

	Vector3 Vector4::zxz() const
	{
		return Vector3(z, x, z);
	}

	Vector3 Vector4::zxw() const
	{
		return Vector3(z, x, w);
	}

	Vector3 Vector4::zyx() const
	{
		return Vector3(z, y, x);
	}

	Vector3 Vector4::zyy() const
	{
		return Vector3(z, y, y);
	}

	Vector3 Vector4::zyz() const
	{
		return Vector3(z, y, z);
	}

	Vector3 Vector4::zyw() const
	{
		return Vector3(z, y, w);
	}

	Vector3 Vector4::zzx() const
	{
		return Vector3(z, z, x);
	}

	Vector3 Vector4::zzy() const
	{
		return Vector3(z, z, y);
	}

	Vector3 Vector4::zzz() const
	{
		return Vector3(z, z, z);
	}

	Vector3 Vector4::zzw() const
	{
		return Vector3(z, z, w);
	}

	Vector3 Vector4::wxx() const
	{
		return Vector3(w, x, x);
	}

	Vector3 Vector4::wxy() const
	{
		return Vector3(w, x, y);
	}

	Vector3 Vector4::wxz() const
	{
		return Vector3(w, x, z);
	}

	Vector3 Vector4::wxw() const
	{
		return Vector3(w, x, w);
	}

	Vector3 Vector4::wyx() const
	{
		return Vector3(w, y, x);
	}

	Vector3 Vector4::wyy() const
	{
		return Vector3(w, y, y);
	}

	Vector3 Vector4::wyz() const
	{
		return Vector3(w, y, z);
	}

	Vector3 Vector4::wyw() const
	{
		return Vector3(w, y, w);
	}

	Vector3 Vector4::wzx() const
	{
		return Vector3(w, z, x);
	}

	Vector3 Vector4::wzy() const
	{
		return Vector3(w, z, y);
	}

	Vector3 Vector4::wzz() const
	{
		return Vector3(w, z, z);
	}

	Vector3 Vector4::wzw() const
	{
		return Vector3(w, z, w);
	}

	Vector2 Vector4::xx() const
	{
		return Vector2(x, x);
	}

	Vector2 Vector4::xy() const
	{
		return Vector2(x, y);
	}

	Vector2 Vector4::xz() const
	{
		return Vector2(x, z);
	}

	Vector2 Vector4::xw() const
	{
		return Vector2(x, w);
	}

	Vector2 Vector4::yx() const
	{
		return Vector2(y, x);
	}

	Vector2 Vector4::yy() const
	{
		return Vector2(y, y);
	}

	Vector2 Vector4::yz() const
	{
		return Vector2(y, z);
	}

	Vector2 Vector4::yw() const
	{
		return Vector2(y, w);
	}

	Vector2 Vector4::zx() const
	{
		return Vector2(z, z);
	}

	Vector2 Vector4::zy() const
	{
		return Vector2(z, y);
	}

	Vector2 Vector4::zz() const
	{
		return Vector2(z, z);
	}

	Vector2 Vector4::zw() const
	{
		return Vector2(z, w);
	}

	Vector2 Vector4::wx() const
	{
		return Vector2(w, w);
	}

	Vector2 Vector4::wy() const
	{
		return Vector2(w, y);
	}

	Vector2 Vector4::wz() const
	{
		return Vector2(w, z);
	}

	Vector2 Vector4::ww() const
	{
		return Vector2(w, w);
	}

	const Vector4 Vector4::ZERO = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	const Vector4 Vector4::ONE = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}
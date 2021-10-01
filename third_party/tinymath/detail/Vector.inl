TINYMATH_NAMESPACE

#define MODIFY_ELEMENTS(block)  size_t indices[] = { Indices ... };\
                                for (size_t i = 0; i < sizeof...(Indices); i++)\
                                {\
                                    size_t index= indices[i];\
                                    block\
                                }\
                                return *(Container*)this;

#define RETURN_ELEMENTS(block)  Container<Component, sizeof...(Indices)> ret;\
                                size_t indices[] = { Indices ... };\
                                for (size_t i = 0; i < sizeof...(Indices); i++)\
                                {\
                                    size_t index= indices[i];\
                                    block\
                                }\
                                return ret;


template<template<typename, size_t> class Container, typename Component, size_t ...Indices>
class Swizzle
{
private:
	Component data[sizeof...(Indices)];

public:
	Container<Component, sizeof...(Indices)>& operator=(const Container<Component, sizeof...(Indices)>& rhs)
	{
		size_t indices[] = { Indices ... };
		for (size_t i = 0; i < sizeof...(Indices); i++) { data[indices[i]] = rhs[i]; }
		return *(Container<Component, sizeof...(Indices)>*)this;
	}

	operator Container<Component, sizeof...(Indices)>() const
	{
		return Container<Component, sizeof...(Indices)>(data[Indices]...);
	}

	Container<Component, sizeof...(Indices)> operator- () const
	{
		RETURN_ELEMENTS(
			ret[i] = -data[index];
		);
	}

	Container<Component, sizeof...(Indices)> operator+ (const Container<Component, sizeof...(Indices)>& rhs) const
	{
		RETURN_ELEMENTS(
			ret[i] = data[index] + rhs[i];
		);
	}

	Container<Component, sizeof...(Indices)> operator- (const Container<Component, sizeof...(Indices)>& rhs) const
	{
		RETURN_ELEMENTS(
			ret[i] = data[index] - rhs[i];
		);
	}

	Container<Component, sizeof...(Indices)> operator* (const Container<Component, sizeof...(Indices)>& rhs) const
	{
		RETURN_ELEMENTS(
			ret[i] = data[index] * rhs[i];
		);
	}

	Container<Component, sizeof...(Indices)> operator/ (const Container<Component, sizeof...(Indices)>& rhs) const
	{
		RETURN_ELEMENTS(
			ret[i] = data[index] / rhs[i];
		);
	}

	Container<Component, sizeof...(Indices)>& operator+= (const Container<Component, sizeof...(Indices)>& rhs)
	{
		MODIFY_ELEMENTS(
			data[index] += rhs[i];
		);
	}

	Container<Component, sizeof...(Indices)>& operator-= (const Container<Component, sizeof...(Indices)>& rhs)
	{
		MODIFY_ELEMENTS(
			data[index] -= rhs[i];
		);
	}

	Container<Component, sizeof...(Indices)>& operator*= (const Container<Component, sizeof...(Indices)>& rhs)
	{
		MODIFY_ELEMENTS(
			data[index] *= rhs[i];
		);
	}

	Container<Component, sizeof...(Indices)>& operator/= (const Container<Component, sizeof...(Indices)>& rhs)
	{
		MODIFY_ELEMENTS(
			data[index] /= rhs[i];
		);
	}

	Container<Component, sizeof...(Indices)> operator+ (const Component& rhs) const
	{
		RETURN_ELEMENTS(
			ret[i] = data[index] + rhs;
		);
	}

	Container<Component, sizeof...(Indices)> operator- (const Component& rhs) const
	{
		RETURN_ELEMENTS(
			ret[i] = data[index] - rhs;
		);
	}

	Container<Component, sizeof...(Indices)> operator* (const Component& rhs) const
	{
		RETURN_ELEMENTS(
			ret[i] = data[index] * rhs;
		);
	}

	Container<Component, sizeof...(Indices)> operator/ (const Component& rhs) const
	{
		RETURN_ELEMENTS(
			ret[i] = data[index] / rhs;
		);
	}

	Container<Component, sizeof...(Indices)>& operator+= (const Component& rhs)
	{
		MODIFY_ELEMENTS(
			data[index] += rhs;
		);
	}

	Container<Component, sizeof...(Indices)>& operator-= (const Component& rhs)
	{
		MODIFY_ELEMENTS(
			data[index] -= rhs;
		);
	}

	Container<Component, sizeof...(Indices)>& operator*= (const Component& rhs)
	{
		MODIFY_ELEMENTS(
			data[index] *= rhs;
		);
	}

	Container<Component, sizeof...(Indices)>& operator/= (const Component& rhs)
	{
		MODIFY_ELEMENTS(
			data[index] /= rhs;
		);
	}
};

#include "SwizzlePermutation"

template <typename Component, size_t N>
struct Vector
{
	Component data[N];
	Vector<Component, N>() {}
	Component& operator[](const size_t& index) { return data[index]; }
	const Component& operator[](const size_t& index) const { return data[index]; }
};

#define COMPONENT_WWISE_FUNC(func)\
template<typename Component, size_t N>\
Vector<Component, N> func(const Vector<Component, N>& v)\
{\
    Vector<Component, N> ret = Vector<Component, N>();\
    for(size_t i = 0; i < N; ++i)\
        ret[i] = (Component)func(v[i]);\
    return ret;\
}

#define COMPONENT_WWISE_FUNC_2(func)\
template<typename Component, size_t N>\
Vector<Component, N> func(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs)\
{\
    Vector<Component, N> ret = Vector<Component, N>();\
    for(size_t i = 0; i < N; ++i)\
        ret[i] = (Component)func(lhs[i], rhs[i]);\
    return ret;\
}

COMPONENT_WWISE_FUNC(sgn);
COMPONENT_WWISE_FUNC(sin);
COMPONENT_WWISE_FUNC(asin);
COMPONENT_WWISE_FUNC(cos);
COMPONENT_WWISE_FUNC(acos);
COMPONENT_WWISE_FUNC(tan);
COMPONENT_WWISE_FUNC(floor);
COMPONENT_WWISE_FUNC(ceil);
COMPONENT_WWISE_FUNC(abs);
COMPONENT_WWISE_FUNC(exp);
COMPONENT_WWISE_FUNC(exp2);
COMPONENT_WWISE_FUNC(trunc);
COMPONENT_WWISE_FUNC(sqrt);
COMPONENT_WWISE_FUNC(log);
COMPONENT_WWISE_FUNC(log10);
COMPONENT_WWISE_FUNC(log2);
COMPONENT_WWISE_FUNC_2(pow);


template<typename Component, size_t N>
bool operator!=(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	for (size_t i = 0; i < N; ++i) { if (lhs[i] != rhs[i]) return true; } return false;
}

template<typename Component, size_t N>
bool operator==(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	for (size_t i = 0; i < N; ++i) { if (lhs[i] != rhs[i]) return false; } return true;
}

template<typename Component, size_t N>
Vector<Component, N> operator-(const Vector<Component, N>& v)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = -v[i]; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> operator+(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = lhs[i] + rhs[i]; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> operator-(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = lhs[i] - rhs[i]; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> operator*(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = lhs[i] * rhs[i]; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> operator/(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = lhs[i] / rhs[i]; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> operator+(const Vector<Component, N>& lhs, const Component& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = lhs[i] + rhs; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> operator-(const Vector<Component, N>& lhs, const Component& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = lhs[i] - rhs; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> operator*(const Vector<Component, N>& lhs, const Component& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = lhs[i] * rhs; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> operator/(const Vector<Component, N>& lhs, const Component& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = lhs[i] / rhs; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> operator+(const Component& lhs, const Vector<Component, N>& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = lhs + rhs[i]; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> operator-(const Component& lhs, const Vector<Component, N>& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = lhs - rhs[i]; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> operator*(const Component& lhs, const Vector<Component, N>& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = lhs * rhs[i]; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> operator/(const Component& lhs, const Vector<Component, N>& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = lhs / rhs[i]; } return ret;
}

template<typename Component, size_t N>
Vector<Component, N>& operator+=(Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	for (size_t i = 0; i < N; ++i) { lhs[i] = lhs[i] + rhs[i]; } return lhs;
}

template<typename Component, size_t N>
Vector<Component, N>& operator-=(Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	for (size_t i = 0; i < N; ++i) { lhs[i] = lhs[i] - rhs[i]; } return lhs;
}

template<typename Component, size_t N>
Vector<Component, N>& operator*=(Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	for (size_t i = 0; i < N; ++i) { lhs[i] = lhs[i] * rhs[i]; } return lhs;
}

template<typename Component, size_t N>
Vector<Component, N>& operator/=(Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	for (size_t i = 0; i < N; ++i) { lhs[i] = lhs[i] / rhs[i]; } return lhs;
}

template<typename Component, size_t N>
Vector<Component, N> operator+=(Vector<Component, N>& lhs, const Component& rhs)
{
	for (size_t i = 0; i < N; ++i) { lhs[i] = lhs[i] + rhs; } return lhs;
}

template<typename Component, size_t N>
Vector<Component, N> operator-=(Vector<Component, N>& lhs, const Component& rhs)
{
	for (size_t i = 0; i < N; ++i) { lhs[i] = lhs[i] - rhs; } return lhs;
}

template<typename Component, size_t N>
Vector<Component, N> operator*=(Vector<Component, N>& lhs, const Component& rhs)
{
	for (size_t i = 0; i < N; ++i) { lhs[i] = lhs[i] * rhs; } return lhs;
}

template<typename Component, size_t N>
Vector<Component, N> operator/=(Vector<Component, N>& lhs, const Component& rhs)
{
	for (size_t i = 0; i < N; ++i) { lhs[i] = lhs[i] / rhs; } return lhs;
}

// suppress c4201
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4201)
#endif

template <typename Component, size_t N>
using SwizzleContainer = Vector<Component, N>;

template <typename Component>
struct Vector<Component, 2>
{
	union
	{
		Component data[2];
		struct { Component x, y; };

		SWIZZLE_2;
	};

	Vector<Component, 2>() {}
	constexpr Vector<Component, 2>(const Component& v) : x(v), y(v) {}
	constexpr Vector<Component, 2>(const Component& c1, const Component& c2) : x(c1), y(c2) {}
	constexpr Vector<Component, 2>(const Vector<Component, 2>& v) : x(v.x), y(v.y) {}

	Component& operator[](const size_t& index) { return data[index]; }
	const Component& operator[](const size_t& index) const { return data[index]; }
};

template <typename Component>
struct Vector<Component, 3>
{
	union
	{
		Component data[3];
		struct { Component x, y, z; };

		SWIZZLE_3;
	};

	Vector<Component, 3>() {}
	constexpr Vector<Component, 3>(const Component& v) : x(v), y(v), z(v) {}
	constexpr Vector<Component, 3>(const Vector<Component, 2>& v) : x(v.data[0]), y(v.data[1]), z(0.0f) {}
	constexpr Vector<Component, 3>(const Component& c1, const Component& c2, const Component& c3) : x(c1), y(c2), z(c3) { }
	//constexpr Vector<Component, 3>(const Vector<Component, 3>& v) : x(v.x), y(v.y), z(v.z) {}

	Component& operator[](const size_t& index) { return data[index]; }
	const Component& operator[](const size_t& index) const { return data[index]; }
};

template <typename Component>
struct Vector<Component, 4>
{
	union
	{
		Component data[4];
		struct { Component x, y, z, w; };

		SWIZZLE_4;
	};

	Vector<Component, 4>() {}
	constexpr Vector<Component, 4>(const Component& v) : x(v), y(v), z(v), w(v) {}
	constexpr Vector<Component, 4>(const Vector<Component, 2>& v) : x(v.data[0]), y(v.data[1]), z(0.0f) {}
	constexpr Vector<Component, 4>(const Vector<Component, 3>& v) : x(v.data[0]), y(v.data[1]), z(v.data[2]), w(0.0f) {}
	constexpr Vector<Component, 4>(const Component& c1, const Component& c2, const Component& c3, const Component& c4) : x(c1), y(c2), z(c3), w(c4) {}
	//constexpr Vector<Component, 4>(const Vector<Component, 4>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

	Component& operator[](const size_t& index) { return data[index]; }
	const Component& operator[](const size_t& index) const { return data[index]; }
};

constexpr Vector<float, 2> kVec2fZero = Vector<float, 2>(0.f);
constexpr Vector<float, 2> kVec2fOne = Vector<float, 2>(1.f);
constexpr Vector<int, 2> kVec2iZero = Vector<int, 2>(0);
constexpr Vector<int, 2> kVec2iOne = Vector<int, 2>(1);
constexpr Vector<float, 3> kVec3fZero = Vector<float, 3>(0.f);
constexpr Vector<float, 3> kVec3fOne = Vector<float, 3>(1.f);
constexpr Vector<float, 3> kVec3fUp = Vector<float, 3>(0.f, 1.f, 0.f);
constexpr Vector<float, 3> kVec3fDown = Vector<float, 3>(0.f, -1.f, 0.f);
constexpr Vector<float, 3> kVec3fLeft = Vector<float, 3>(-1.f, 0.f, 0.f);
constexpr Vector<float, 3> kVec3fRight = Vector<float, 3>(1.f, 0.f, 0.f);
constexpr Vector<float, 3> kVec3fForward = Vector<float, 3>(0.f, 0.f, 1.f);
constexpr Vector<float, 3> kVec3fBack = Vector<float, 3>(0.f, 0.f, -1.f);
constexpr Vector<int, 3> kVec3iZero = Vector<int, 3>(0);
constexpr Vector<int, 3> kVec3iOne = Vector<int, 3>(1);
constexpr Vector<int, 3> kVec3iUp = Vector<int, 3>(0, 1, 0);
constexpr Vector<int, 3> kVec3iDown = Vector<int, 3>(0, -1, 0);
constexpr Vector<int, 3> kVec3iLeft = Vector<int, 3>(-1, 0, 0);
constexpr Vector<int, 3> kVec3iRight = Vector<int, 3>(1, 0, 0);
constexpr Vector<int, 3> kVec3iForward = Vector<int, 3>(0, 0, 1);
constexpr Vector<int, 3> kVec3iBack = Vector<int, 3>(0, 0, -1);
constexpr Vector<float, 4> kVec4fZero = Vector<float, 4>(0.f);
constexpr Vector<float, 4> kVec4fOne = Vector<float, 4>(1.f);
constexpr Vector<int, 4> kVec4iZero = Vector<int, 4>(0);
constexpr Vector<int, 4> kVec4iOne = Vector<int, 4>(1);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


template<typename Component, size_t N>
Component dot(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	Component ret = Component(); for (size_t i = 0; i < N; ++i) { ret += lhs[i] * rhs[i]; } return ret;
}

template<typename Component>
Vector<Component, 3> cross(const Vector<Component, 3>& lhs, const Vector<Component, 3>& rhs)
{
	return Vector<Component, 3>(lhs[1] * rhs[2] - lhs[2] * rhs[1], lhs[2] * rhs[0] - lhs[0] * rhs[2], lhs[0] * rhs[1] - lhs[1] * rhs[0]);
}

template<typename Component, size_t N>
Component magnitude(const Vector<Component, N>& vector)
{
	Component ret = Component(); for (size_t i = 0; i < N; ++i) { ret += vector[i] * vector[i]; } return tinymath::sqrt(ret);
}

template<typename Component, size_t N>
Component length(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	return magnitude(lhs - rhs);
}

template<typename Component, size_t N>
Vector<Component, N> max(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = tinymath::max(lhs[i], rhs[i]); } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> min(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = tinymath::min(lhs[i], rhs[i]); } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> clamp(const Vector<Component, N>& v, const Vector<Component, N>& min, const Vector<Component, N>& max)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = tinymath::clamp(v[i], min[i], max[i]); } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> clamp(const Vector<Component, N>& v, const Component& min, const Component& max)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = tinymath::clamp(v[i], min, max); } return ret;
}

template<typename Component, size_t N>
Vector<Component, N> normalize(const Vector<Component, N>& v)
{
	Component num = magnitude(v);
	return num > EPSILON ? (v / num) : Vector<Component, N>();
}

template<typename Component, size_t N>
Vector<Component, N> lerp(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs, float t)
{
	Vector<Component, N> ret = Vector<Component, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = tinymath::lerp(lhs[i], rhs[i], t); } return ret;
}

template<size_t N>
Vector<float, N> saturate(const Vector<float, N>& v)
{
	Vector<float, N> ret = Vector<float, N>(); for (size_t i = 0; i < N; ++i) { ret[i] = tinymath::clamp(v[i], 0.0f, 1.0f); } return ret;
}

template<typename Component, size_t N>
bool approx(const Vector<Component, N>& lhs, const Vector<Component, N>& rhs)
{
	for (size_t i = 0; i < N; ++i) {  if (abs(lhs[i] - rhs[i]) > EPSILON) { return false; } }  return true;
}

template<typename Component>
void calculate_right_up(const Vector<Component, 3>& forward, Vector<Component, 3>& right, Vector<Component, 3>& up)
{
#ifdef LEFT_HANDED
	right = tinymath::normalize(tinymath::cross(kVec3fUp, forward));
	up = tinymath::cross(forward, right);
#else 
	right = tinymath::normalize(tinymath::cross(forward, kVec3fUp));
	up = tinymath::cross(right, forward);
#endif
}

END_NAMESPACE
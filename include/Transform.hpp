#ifndef _TRANSFORM_
#define _TRANSFORM_
#include "Vector3.hpp"
#include "Matrix4x4.hpp"

namespace Guarneri
{
	//todo: hierarchical Transform
	struct Transform
	{
	public:
		Vector3 position;
		Vector3 axis;
		float rotationTheta;
		Vector3 local_scale;
		Matrix4x4 local2world;

	public:
		Transform();
		Transform(const Transform& other);
		Vector3 forward() const;
		Vector3 up() const;
		Vector3 right()  const;
		void translate(const Vector3& translation);
		void move_forward(const float& distance);
		void move_backward(const float& distance);
		void move_left(const float& distance);
		void move_right(const float& distance);
		void move_ascend(const float& distance);
		void move_descend(const float& distance);
		void rotate(const Vector3& _axis, const float& angle);
		void scale(const Vector3& _scale);
		void sync();
		Transform& operator =(const Transform& other);
	};
}
#endif
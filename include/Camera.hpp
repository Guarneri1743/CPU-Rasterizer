#ifndef _CAMERA_
#define _CAMERA_
#include "Define.hpp"
#include "Object.hpp"
#include "Vector3.hpp"
#include "Matrix4x4.hpp"

namespace Guarneri
{
	class Camera : public Object
	{
	public:
		float fov;
		float aspect;
		float near;
		float far;
		Vector3 forward;
		Vector3 right;
		Vector3 up;
		Vector3 position;
		Matrix4x4 p;
		Matrix4x4 v;
		float yaw;
		float pitch;
		Projection projection;

	public:
		Camera();
		~Camera();
		static std::unique_ptr<Camera> create(const Vector3& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far);
		void initialize(const Vector3& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far, const Projection& _proj_type);
		void rotate(const float& yaw_offset, const float& pitch_offset);
		void lookat(const Vector3& target);
		void update_camera();
		Matrix4x4 view_matrix() const;
		const Matrix4x4 projection_matrix() const;
		void move(const Vector3& t);
		void move_forward(const float& distance);
		void move_backward(const float& distance);
		void move_left(const float& distance);
		void move_right(const float& distance);
		void move_ascend(const float& distance);
		void move_descend(const float& distance);
		void set_near(const float& _near);
		void set_far(const float& _far);
		void set_fov(const float& _fov);
		void set_projection(const Projection& proj);
		void update_proj_mode();
		std::string str() const;
	};
}
#endif
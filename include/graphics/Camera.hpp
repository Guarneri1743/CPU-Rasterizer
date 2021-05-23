#ifndef _CAMERA_
#define _CAMERA_
#include "Define.hpp"
#include "Object.hpp"
#include "Vector3.hpp"
#include "Matrix4x4.hpp"
#include "Transform.hpp"
#include "rapidjson/document.h"

#undef near
#undef far

namespace Guarneri
{
	class Camera : public Object
	{
	public:
		float fov;
		float aspect;
		float near;
		float far;
		Matrix4x4 proj_matrix;
		std::unique_ptr<Transform> transform;
		Projection projection;
		bool enable_msaa;

	public:
		~Camera();
		static std::unique_ptr<Camera> create(const Vector3& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far);
		void initialize(const Vector3& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far, const Projection& _proj_type);
		Matrix4x4 view_matrix() const;
		const Matrix4x4 projection_matrix() const;
		void focus(const Vector3& position);
		void set_near(const float& _near);
		void set_far(const float& _far);
		void set_fov(const float& _fov);
		void set_projection(const Projection& proj);
		void update_proj_mode();
		static rapidjson::Value serialize(rapidjson::Document& doc, const Camera& cam);
		static std::unique_ptr<Camera> deserialize(const rapidjson::Value& v);
		std::string str() const;
		static Camera* get_main_camera() { return main_cam; }
		static void set_main_camera(Camera* cam) { main_cam = cam; }

	private:
		Camera();
		static Camera* main_cam;
	};
}
#endif
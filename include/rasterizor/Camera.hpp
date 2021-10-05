#pragma once
#include "Define.hpp"
#include "tinymath.h"

#undef near
#undef far

namespace CpuRasterizor
{
	class Transform;
	class Camera 
	{
	public:
		float fov;
		float aspect;
		float near;
		float far;
		tinymath::mat4x4 proj_matrix;
		std::unique_ptr<Transform> transform;
		Projection projection;
		bool enable_msaa;

	public:
		Camera();
		~Camera();
		static std::unique_ptr<Camera> create(const tinymath::vec3f& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far);
		void initialize(const tinymath::vec3f& _position, const float& _aspect, const float& _fov, const float& _near, const float& _far, const Projection& _proj_type);
		tinymath::mat4x4 view_matrix() const;
		const tinymath::mat4x4 projection_matrix() const;
		void focus(const tinymath::vec3f& position);
		void set_near(const float& _near);
		void set_far(const float& _far);
		void set_fov(const float& _fov);
		void set_projection(const Projection& proj);
		void update_proj_mode();
		static Camera* get_main_camera() { return main_cam; }
		static void set_main_camera(Camera* cam) { main_cam = cam; }

	private:
		static Camera* main_cam;
	};
}
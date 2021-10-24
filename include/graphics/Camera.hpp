#pragma once
#include <memory>
#include "Define.hpp"
#include "tinymath.h"

#undef near
#undef far

namespace CpuRasterizer
{
	class Transform;

	class Camera 
	{
	public:
		tinymath::FrustumParam frustum_param;
		tinymath::mat4x4 proj_matrix;
		std::unique_ptr<Transform> transform;
		bool enable_msaa;

	public:
		Camera();
		~Camera();
		static std::unique_ptr<Camera> create(const tinymath::vec3f& _position, const tinymath::FrustumParam& frustum);
		void initialize(const tinymath::vec3f& _position, const tinymath::FrustumParam& frustum);
		tinymath::mat4x4 view_matrix() const;
		const tinymath::mat4x4 projection_matrix() const;
		void focus(const tinymath::vec3f& position);
		void set_near(float _near);
		void set_far(float _far);
		void set_fov(float _fov);
		void set_projection(tinymath::Projection proj);
		void update_projection_matrix();
		static Camera* get_main_camera() { return main_cam; }
		static void set_main_camera(Camera* cam) { main_cam = cam; }

	private:
		static Camera* main_cam;
	};
}
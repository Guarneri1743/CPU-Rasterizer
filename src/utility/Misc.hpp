#ifndef _GRAPHICS_UTILITY_
#define _GRAPHICS_UTILITY_
#include <Guarneri.hpp>

namespace Guarneri {
	struct MiscParameter {
		MiscParameter() {
			cam_near = 0.1f;
			cam_far = 500.0f;
			camera_pos = Vector3::ZERO;
			view_matrix = Matrix4x4::IDENTITY;
			proj_matrix = Matrix4x4::IDENTITY;
			main_light = DirectionalLight();
			render_flag = RenderFlag::DISABLE;
			persample_op_flag = PerSampleOperation::SCISSOR_TEST | PerSampleOperation::STENCIL_TEST | PerSampleOperation::DEPTH_TEST | PerSampleOperation::BLENDING;
			culling_clipping_flag = CullingAndClippingFlag::APP_FRUSTUM_CULLING | CullingAndClippingFlag::NEAR_PLANE_CLIPPING | CullingAndClippingFlag::SCREEN_CLIPPING | CullingAndClippingFlag::BACK_FACE_CULLING;
			enable_shadow = false;
		}

		float cam_near;
		float cam_far;
		Vector3 camera_pos;
		Matrix4x4 view_matrix;
		Matrix4x4 proj_matrix;
		DirectionalLight main_light;
		std::vector<PointLight> point_lights;
		RenderFlag render_flag;
		PerSampleOperation persample_op_flag;
		CullingAndClippingFlag culling_clipping_flag;
		bool enable_shadow;
	};

	static MiscParameter misc_param;

	GraphicsDevice& Graphics() {
		return Singleton<GraphicsDevice>::get();
	}

	InputManager& InputMgr() {
		return Singleton<InputManager>::get();
	}

	GDIWindow& Window() {
		return Singleton<GDIWindow>::get();
	}

	ResourceManager<Texture>& TextureMgr() {
		return Singleton<ResourceManager<Texture>>::get();
	}
}
#endif
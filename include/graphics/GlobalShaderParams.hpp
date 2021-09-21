#ifndef _GRAPHICS_UTILITY_
#define _GRAPHICS_UTILITY_
#include <vector>
#include "Define.hpp"
#include "Light.hpp"
#include "Traits.hpp"

namespace Guarneri {
	struct GlobalShaderParams {
		GlobalShaderParams() {
			cam_near = 0.1f;
			cam_far = 500.0f;
			camera_pos = Vector3::ZERO;
			view_matrix = Matrix4x4::IDENTITY;
			proj_matrix = Matrix4x4::IDENTITY;
			main_light = DirectionalLight();
			render_flag = RenderFlag::DISABLE;
			workflow = PBRWorkFlow::Metallic;
			shadow_bias = 0.02f;
			enable_shadow = true;
			pcf_on = true;
			enable_msaa = false;
			enable_ibl = true;
			color_space = ColorSpace::Gamma;
			persample_op_flag =
				PerSampleOperation::SCISSOR_TEST |
				PerSampleOperation::STENCIL_TEST |
				PerSampleOperation::DEPTH_TEST |
				PerSampleOperation::BLENDING;
			culling_clipping_flag =
				CullingAndClippingFlag::APP_FRUSTUM_CULLING |
				CullingAndClippingFlag::NEAR_PLANE_CLIPPING |
				CullingAndClippingFlag::SCREEN_CLIPPING |
				CullingAndClippingFlag::BACK_FACE_CULLING;
		}

		float cam_near;
		float cam_far;
		Vector3 camera_pos;
		Matrix4x4 view_matrix;
		Matrix4x4 proj_matrix;
		DirectionalLight main_light;
		PBRWorkFlow workflow;
		ColorSpace color_space;
		std::vector<PointLight> point_lights;
		RenderFlag render_flag;
		PerSampleOperation persample_op_flag;
		CullingAndClippingFlag culling_clipping_flag;

		bool enable_shadow;
		bool enable_ibl;
		bool pcf_on;
		float shadow_bias;
		bool enable_msaa;
	};
}
#endif
#pragma once
#include <vector>
#include "Define.hpp"
#include "Light.hpp"
#include "tinymath.h"

#define CpuRasterSharedData Singleton<CpuRasterizer::GlobalShaderParams>::get()

namespace CpuRasterizer {
	struct GlobalShaderParams 
	{
		GlobalShaderParams() 
		{
			cam_near = 0.1f;
			cam_far = 500.0f;
			camera_pos = tinymath::kVec3fZero;
			view_matrix = tinymath::kMat4x4Identity;
			proj_matrix = tinymath::kMat4x4Identity;
			main_light = DirectionalLight();
			debug_flag = RenderFlag::kNone;
			workflow = PBRWorkFlow::kMetallic;
			shadow_bias = 0.02f;
			enable_shadow = true;
			pcf_on = true;
			enable_ibl = true;
			enable_gizmos = true;
			width = 800;
			height = 600;

			// raster
			enable_mipmap = false;
			msaa_dirty = true;
			color_space = ColorSpace::kGamma;
		}

		size_t width;
		size_t height;
		float cam_near;
		float cam_far;
		tinymath::vec3f camera_pos;
		tinymath::mat4x4 view_matrix;
		tinymath::mat4x4 proj_matrix;
		DirectionalLight main_light;
		PBRWorkFlow workflow;
		ColorSpace color_space;
		std::vector<PointLight> point_lights;
		RenderFlag debug_flag;
		bool enable_shadow;
		bool enable_ibl;
		bool pcf_on;
		float shadow_bias;
		bool enable_gizmos;

		bool enable_mipmap;
		bool msaa_dirty;
	};
}

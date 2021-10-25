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
			enable_msaa = false;
			msaa_subsample_count = 4;
			color_space = ColorSpace::kGamma;
			multi_sample_frequency = MultiSampleFrequency::kPixelFrequency;
			raster_flag =
				RasterFlag::kScissorTest |
				RasterFlag::kStencilTest |
				RasterFlag::kDepthTest |
				RasterFlag::kBlending | 
				RasterFlag::kFaceCulling |
				RasterFlag::kZWrite;

			face_culling = FaceCulling::Back;
			vert_order = VertexOrder::CW;
			this->color_mask = (ColorMask::kRed | ColorMask::kGreen | ColorMask::kBlue | ColorMask::kAlpha);
			this->stencil_func = CompareFunc::kAlways;
			this->stencil_pass_op = StencilOp::kKeep;
			this->stencil_fail_op = StencilOp::kKeep;
			this->stencil_zfail_op = StencilOp::kKeep;
			this->stencil_read_mask = 0xFF;
			this->stencil_write_mask = 0xFF;
			this->stencil_ref_val = 0;
			this->ztest_func = CompareFunc::kLEqual;
			this->src_factor = BlendFactor::kSrcAlpha;
			this->dst_factor = BlendFactor::kOneMinusSrcAlpha;
			this->blend_op = BlendOp::kAdd;
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

		// raster
		ColorMask color_mask;
		CompareFunc stencil_func;
		StencilOp stencil_pass_op;
		StencilOp stencil_fail_op;
		StencilOp stencil_zfail_op;
		uint8_t stencil_ref_val;
		uint8_t stencil_write_mask;
		uint8_t stencil_read_mask;
		CompareFunc ztest_func;
		BlendFactor src_factor;
		BlendFactor dst_factor;
		BlendOp blend_op;
		RasterFlag raster_flag;
		FaceCulling face_culling;
		VertexOrder vert_order;
		MultiSampleFrequency multi_sample_frequency;
		uint8_t msaa_subsample_count;
		bool enable_msaa;
		bool enable_mipmap;
		bool msaa_dirty;
	};
}

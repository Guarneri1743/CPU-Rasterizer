#pragma once
#include "Define.hpp"
#include "tinymath.h"

struct GraphicsContext
{
	// pipeline feature flag
	PipelineFeature pipeline_feature_flag;

	// stencil
	CompareFunc stencil_func;
	StencilOp stencil_pass_op;
	StencilOp stencil_fail_op;
	StencilOp stencil_zfail_op;
	stencil_t stencil_ref_val;
	stencil_t stencil_write_mask;
	stencil_t stencil_read_mask;

	// depth testing
	CompareFunc ztest_func;

	// blending
	BlendFactor src_factor;
	BlendFactor dst_factor;
	BlendFunc blend_op;

	// color mask
	ColorMask color_mask;

	// culling
	FaceCulling face_culling;
	VertexOrder vertex_order;

	// msaa
	MultiSampleFrequency multi_sample_frequency;
	uint8_t msaa_subsample_count;
};
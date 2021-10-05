#pragma once
#include <stdint.h>
#include <ostream>
#include <sstream>
#include "Marcos.h"
#include "Traits.hpp"

namespace Guarneri{
	constexpr int kMaxMip = 8;
	constexpr int kBoundsPadding = 1;

	typedef uint32_t property_name;

	// pbr
	const property_name albedo_prop = 0; //"texture_diffuse/albedo";
	const property_name normal_prop = 1; // "texture_normal";
	const property_name ao_prop = 2; // "texture_ao";
	const property_name emission_prop = 3; // "texture_emission"
	const property_name height_prop = 4; // "texture_emission"
	const property_name f0_prop = 5; // "texture_f0"

	// specular/glossiness workflow
	const property_name specular_prop = 11; // "texture_specular";
	const property_name glossiness_prop = 12; // "texture_glossiness";

	// metallic/roughness workflow
	const property_name metallic_prop = 13; // "texture_metallic"
	const property_name roughness_prop = 14; // "texture_roughness"
	const property_name base_color_prop = 15; //"texture_base_color";

	// misc 
	const property_name cubemap_prop = 20; // "skybox_cubemap";
	const property_name shadowmap_prop = 21; // "shadowmap";

	// coefficients
	const property_name roughness_multiplier_prop = 30;
	const property_name roughness_offset_prop = 31;
	const property_name metallic_multiplier_prop = 32;
	const property_name metallic_offset_prop = 33;

	// colors
	const property_name tint_color_prop = 40;


	// statistics
	struct GraphicsStatistic
	{
		size_t triangle_count;
		size_t culled_triangle_count;
		size_t culled_backface_triangle_count;
		size_t earlyz_optimized;
	};

	typedef uint8_t image_ubyte;

	// pipeline defines
	enum class RasterizerStrategy {
		SCANBLOCK,
		SCANLINE
	};

	enum class ColorSpace
	{
		Gamma = 0, 
		Linear = 1
	};

	enum class PBRWorkFlow
	{
		Metallic = 0,
		Specular = 1
	};

	enum class Projection {
		PERSPECTIVE = 0,
		ORTHO = 1
	};

	enum class CompareFunc {
		NEVER = 0,
		ALWAYS = 1,
		LESS = 2,
		LEQUAL = 3,
		EQUAL = 4,
		GEQUAL = 5,
		GREATER = 6,
		NOT_EQUAL = 7
	};

	enum class ZWrite {
		OFF = 0,
		ON = 1
	};

	enum class StencilOp {
		KEEP = 0,
		ZERO = 1,
		REPLACE = 2,
		INCR = 3,
		INCR_WRAP = 4,
		DECR = 5,
		DECR_WRAP = 6,
		INVERT = 7
	};

	enum class BlendFactor {
		ONE = 0,
		SRC_COLOR = 1,
		SRC_ALPHA = 2,
		ONE_MINUS_SRC_ALPHA = 3,
		ONE_MINUS_SRC_COLOR = 4,
		DST_COLOR = 5,
		DST_ALPHA = 6,
		ONE_MINUS_DST_ALPHA = 7,
		ONE_MINUS_DST_COLOR = 8
	};

	enum class BlendOp {
		ADD = 0,
		SUB = 1
	};

	enum class ColorMask {
		ZERO = 0,
		R = 1 << 0,
		G = 1 << 1,
		B = 1 << 2,
		A = 1 << 3
	};

	enum class CullingAndClippingFlag {
		DISABLE = 0,
		APP_FRUSTUM_CULLING = 1 << 0,
		NEAR_PLANE_CLIPPING = 1 << 1,
		SCREEN_CLIPPING = 1 << 2,
		BACK_FACE_CULLING = 1 << 3
	};

	enum class PerSampleOperation {
		DISABLE = 0,
		SCISSOR_TEST = 1 << 0,
		ALPHA_TEST = 1 << 1,
		STENCIL_TEST = 1 << 2,
		DEPTH_TEST = 1 << 3,
		BLENDING = 1 << 4
	};

	enum class BufferFlag {
		NONE = 0,
		COLOR = 1 << 0,
		DEPTH = 1 << 1,
		STENCIL = 1 << 2
	};

	enum class RenderFlag {
		DISABLE = 0,
		WIREFRAME = 1 << 0,
		SHADOWMAP = 1 << 1,
		DEPTH = 1 << 2,
		UV = 1 << 3,
		VERTEX_COLOR = 1 << 4,
		NORMAL = 1 << 5,
		MIPMAP = 1 << 6,
		STENCIL = 1 << 7,
		SCANLINE = 1 << 8,
		FRAME_TILE = 1 << 9,
		CULLED_BACK_FACE = 1 << 10,
		SPECULAR = 1 << 11,
		EARLY_Z_DEBUG = 1 << 12,
		IRRADIANCE_MAP = 1 << 13,
		ROUGHNESS = 1 << 14,
		METALLIC = 1 << 15,
		AO = 1 << 16,
		INDIRECT_DIFFUSE = 1 << 17,
		INDIRECT_SPECULAR = 1 << 18
	};

	enum class FrameContent
	{
		None = 0,
		Color = 1 << 0,
		Depth = 1 << 1,
		Stencil = 1 << 2,
		Coverage = 1 << 3
	};

	enum class MultiSampleFrequency
	{
		kPixelFrequency,
		kSubsampleFrequency
	};

#ifdef OPAQUE
#undef OPAQUE
#endif
#ifdef TRANSPARENT
#undef TRANSPARENT
#endif

	enum class RenderPass {
		OBJECT,
		SHADOW,
		SKYBOX
	};

	template<>
	struct support_bitwise_enum<RenderFlag> : std::true_type {};

	template<>
	struct support_bitwise_enum<BufferFlag> : std::true_type {};

	template<>
	struct support_bitwise_enum<PerSampleOperation> : std::true_type {};

	template<>
	struct support_bitwise_enum<CullingAndClippingFlag> : std::true_type {};

	template<>
	struct support_bitwise_enum<ColorMask> : std::true_type {};

	template<>
	struct support_bitwise_enum<FrameContent> : std::true_type {};
}
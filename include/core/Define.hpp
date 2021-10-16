#pragma once
#include <stdint.h>
#include "Marcos.h"
#include <type_traits>


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

// common
const property_name mat_model = 50;
const property_name mat_view = 51;
const property_name mat_projection = 52;
const property_name mat_vp = 53;
const property_name mat_mvp = 54;

// light
const property_name light_position = 80;
const property_name light_ambient = 81;
const property_name light_diffuse = 82;
const property_name light_specular = 83;
const property_name light_direction = 84;
const property_name light_intensity = 85;
const property_name light_distance = 86;


// statistics
struct GraphicsStatistic
{
	size_t triangle_count;
	size_t culled_triangle_count;
	size_t culled_backface_triangle_count;
	size_t earlyz_optimized;
};

typedef uint8_t image_ubyte;

enum class WrapMode
{
	kRepeat = 0,
	kClampToEdge = 1,
	kClampToBorder = 2
};

enum class Filtering
{
	kPoint = 0,
	kBilinear = 1,
	kMax = 2,
	kMin = 3
};

enum class TextureFormat
{
	kInvalid = 0,
	kRGB = 1,
	kRGBA = 2,
	kRG = 3,
	kGray = 4,
	kRGB16 = 5,
	kRGBA16 = 6
};

// pipeline defines
enum class RasterizerStrategy
{
	kScanblock,
	kScanline
};

enum class ColorSpace
{
	kGamma = 0,
	kLinear = 1
};

enum class PBRWorkFlow
{
	kMetallic = 0,
	kSpecular = 1
};

enum class CompareFunc
{
	kNever = 0,
	kAlways = 1,
	kLess = 2,
	kLEqual = 3,
	kEqual = 4,
	kGEqual = 5,
	kGreater = 6,
	kNotEqual = 7
};

enum class ZWrite
{
	kOff = 0,
	kOn = 1
};

enum class StencilOp
{
	kKeep = 0,
	kZero = 1,
	kReplace = 2,
	kIncrement = 3,
	kIncrementWrap = 4,
	kDecrement = 5,
	kDecrementWrap = 6,
	kInvert = 7
};

enum class BlendFactor
{
	kOne = 0,
	kSrcColor = 1,
	kSrcAlpha = 2,
	kOneMinusSrcAlpha = 3,
	kOneMinusSrcColor = 4,
	kDstColor = 5,
	kDstAlpha = 6,
	kOneMinusDstAlpha = 7,
	kOneMinusDstColor = 8
};

enum class BlendOp
{
	kAdd = 0,
	kSub = 1
};

enum class ColorMask
{
	kZero = 0,
	kRed = 1 << 0,
	kGreen = 1 << 1,
	kBlue = 1 << 2,
	kAlpha = 1 << 3
};

enum class CullingAndClippingFlag
{
	kNone = 0,
	kAppFrustumCulling = 1 << 0,
	kNearPlaneClipping = 1 << 1,
	kScreenClipping = 1 << 2,
	kBackFaceCulling = 1 << 3
};

enum class PerSampleOperation
{
	kNone = 0,
	kScissorTest = 1 << 0,
	kAlphaTest = 1 << 1,
	kStencilTest = 1 << 2,
	kDepthTest = 1 << 3,
	kBlending = 1 << 4
};

enum class BufferFlag
{
	kNone = 0,
	kColor = 1 << 0,
	kDepth = 1 << 1,
	kStencil = 1 << 2
};

enum class RenderFlag
{
	kNone = 0,
	kWireFrame = 1 << 0,
	kShadowMap = 1 << 1,
	kDepth = 1 << 2,
	kUV = 1 << 3,
	kVertexColor = 1 << 4,
	kNormal = 1 << 5,
	kMipmap = 1 << 6,
	kStencil = 1 << 7,
	kScanline = 1 << 8,
	kFrameTile = 1 << 9,
	kReserved0 = 1 << 10,
	kSpecular = 1 << 11,
	kReserved1 = 1 << 12,
	kIrradianceMap = 1 << 13,
	kAlbedo = 1 << 14,
	kRoughness = 1 << 15,
	kMetallic = 1 << 16,
	kAO = 1 << 17,
	kIndirectDiffuse = 1 << 18,
	kIndirectSpecular = 1 << 19
};

enum class FrameContent
{
	kNone = 0,
	kColor = 1 << 0,
	kDepth = 1 << 1,
	kStencil = 1 << 2,
	kCoverage = 1 << 3
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

enum class RenderPass
{
	kObject,
	kShadow,
	kSkybox
};

template<typename TEnumType>
struct support_bitwise_enum : std::false_type {};

template<typename TEnumType>
typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
operator&(TEnumType left, TEnumType right)
{
	return static_cast<TEnumType>(
		static_cast<std::underlying_type_t<TEnumType>>(left) &
		static_cast<std::underlying_type_t<TEnumType>>(right));
}

template<typename TEnumType>
typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
operator|(TEnumType left, TEnumType right)
{
	return static_cast<TEnumType>(
		static_cast<std::underlying_type_t<TEnumType>>(left) |
		static_cast<std::underlying_type_t<TEnumType>>(right));
}

template<typename TEnumType>
typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
operator^(TEnumType left, TEnumType right)
{
	return static_cast<TEnumType>(
		static_cast<std::underlying_type_t<TEnumType>>(left) ^
		static_cast<std::underlying_type_t<TEnumType>>(right));
}

template<typename TEnumType>
typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
operator~(TEnumType value)
{
	return static_cast<TEnumType>(
		~static_cast<std::underlying_type_t<TEnumType>>(value));
}

template<typename TEnumType>
typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
operator&=(TEnumType& left, TEnumType right)
{
	left = left & right;
	return left;
}

template<typename TEnumType>
typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
operator|=(TEnumType& left, TEnumType right)
{
	left = left | right;
	return left;
}

template<typename TEnumType>
typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
operator^=(TEnumType& left, TEnumType right)
{
	left = left ^ right;
	return left;
}

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

#pragma once
#include <stdint.h>
#include "tinymath.h"
#include "Define.hpp"
#include "RasterAttributes.hpp"
#include "Shader.hpp"

#define BUILD_CGL // todo

#define cglVec2 tinymath::vec2f
#define cglVec3 tinymath::vec3f
#define cglVec4 tinymath::vec4f
#define cglVec2Int tinymath::vec2i
#define cglVec3Int tinymath::vec3i
#define cglVec4Int tinymath::vec4i
#define cglMat3 tinymath::mat3x3
#define cglMat4 tinymath::mat4x4
#define cglColor tinymath::Color
#define cglDot tinymath::dot
#define cglCross tinymath::cross
#define cglMag tinymath::magnitude
#define cglLen tinymath::length
#define cglMax tinymath::max
#define cglMin tinymath::min
#define cglClamp tinymath::clamp
#define cglNormalize tinymath::normalize
#define cglLerp tinymath::lerp
#define cglSaturate tinymath::saturate
#define cglApprox tinymath::approx
#define cglSin tinymath::sin
#define cglCos tinymath::cos
#define cglAsin tinymath::asin
#define cglAcos tinymath::acos
#define cglTan tinymath::tan
#define cglAtan tinymath::atan
#define cglAbs tinymath::abs
#define cglFloor tinymath::floor
#define cglCeil tinymath::ceil
#define cglLog tinymath::log
#define cglLog2 tinymath::log2
#define cglPow tinymath::pow
#define cglExp tinymath::exp
#define cglExp2 tinymath::exp2
#define cglTrunc tinymath::trunc
#define cglSqrt tinymath::sqrt
#define cglFromEuler tinymath::from_euler
#define cglRotation tinymath::rotation
#define cglDet tinymath::determinant
#define cglInverse tinymath::inverse
#define cglTranspose tinymath::transpose
#define cglTranslation tinymath::translation
#define cglScale tinymath::scale
#define cglViewportMat tinymath::viewport
#define cglLookat tinymath::lookat
#define cglPerspective tinymath::perspective
#define cglOrtho tinymath::ortho
#define cglVec2Zero tinymath::kVec2fZero
#define cglVec3Zero tinymath::kVec3fZero
#define cglVec4Zero tinymath::kVec4fZero
#define cglVec2One tinymath::kVec2fOne
#define cglVec3One tinymath::kVec3fOne
#define cglVec4One tinymath::kVec4fOne
#define cglVec3Up tinymath::kVec3fUp
#define cglVec3Down tinymath::kVec3fDown
#define cglVec3Left tinymath::kVec3fLeft
#define cglVec3Right tinymath::kVec3fRight
#define cglVec3Forward tinymath::kVec3fForward
#define cglVec3Back tinymath::kVec3fBack
#define cglWhite tinymath::kColorWhite
#define cglBlack tinymath::kColorBlack
#define cglRed tinymath::kColorRed
#define cglBlue tinymath::kColorBlue
#define cglGreen tinymath::kColorGreen
#define cglMat3Identity tinymath::kMat3x3Identity
#define cglMat4Identity tinymath::kMat4x4Identity
#define cglMat3Zero tinymath::kMat3x3Zero
#define cglMat4Zero tinymath::kMat4x4Zero
#define cglVert CpuRasterizor::Vertex
#define cglShader CpuRasterizor::Shader // todo: abstract shader
#define cglPrint(...) Logger::log(Logger::Severity::kLog, __VA_ARGS__)
#define cglError(...) Logger::log(Logger::Severity::kError, __VA_ARGS__)

// flags
#define cglFrameContent FrameContent
#define cglCompareFunc CompareFunc
#define cglMultisampleFrequency MultiSampleFrequency
#define cglTextureFormat TextureFormat
#define cglFiltering Filtering
#define cglBlendOp BlendOp
#define cglWrapMode WrapMode
#define cglZWrite ZWrite
#define cglStencilOp StencilOp
#define cglBlendFactor BlendFactor
#define cglColorMask ColorMask

// APIs
#if (defined(WIN32) || defined(_WIN32))
#if defined(BUILD_CGL)
#define CGL_EXTERN  __declspec(dllexport)
#else
#define CGL_EXTERN  __declspec(dllimport)
#endif
#else
#define CGL_EXTERN 
#endif

#ifdef __cplusplus
extern "C" {
#endif

	CGL_EXTERN void cglInitWindow(const char* title, size_t w, size_t h);
	CGL_EXTERN void cglGetMainWindowSize(size_t& w, size_t& h);
	CGL_EXTERN void cglAddResizeEvent(void(*on_resize)(size_t w, size_t h, void* ud), void* user_data);
	CGL_EXTERN int cglIsMainWindowOpen();
	CGL_EXTERN void cglSwapBuffer(int draw_immediately);
	CGL_EXTERN void cglCloseMainWindow();
	CGL_EXTERN void cglClearMainWindow();

	CGL_EXTERN void cglSetViewPort(size_t w, size_t h);
	CGL_EXTERN void cglSetSubSampleCount(uint8_t count);
	CGL_EXTERN uint8_t cglGetSubSampleCount();
	CGL_EXTERN int cglSetMSAAEnabled(int enabled);
	CGL_EXTERN int cglIsMSAAEnabled();
	CGL_EXTERN void cglSetMultisampleFrequency(cglMultisampleFrequency frequency);
	CGL_EXTERN void cglSetClearColor(cglColor clear_color);
	CGL_EXTERN void cglClearBuffer(cglFrameContent content);
	CGL_EXTERN void cglSubmitPrimitive(cglShader* shader, cglVert v1, cglVert v2, cglVert v3);
	CGL_EXTERN void cglFencePrimitives();
	CGL_EXTERN void cglFencePixels();
	CGL_EXTERN int cglGenId(uint32_t&);

#ifdef __cplusplus
}
#endif



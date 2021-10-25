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
#define cglVert CpuRasterizer::Vertex
#define cglPrint(...) Logger::log(Logger::Severity::kLog, __VA_ARGS__)
#define cglError(...) Logger::log(Logger::Severity::kError, __VA_ARGS__)

// flags
#define cglFrameContent FrameContent
#define cglCompareFunc CompareFunc
#define cglMultisampleFrequency MultiSampleFrequency
#define cglTextureFormat TextureFormat
#define cglFiltering Filtering
#define cglBlendOp BlendFunc
#define cglWrapMode WrapMode
#define cglStencilOp StencilOp
#define cglBlendFunc BlendFunc
#define cglBlendFactor BlendFactor
#define cglColorMask ColorMask
#define cglPointer void*

#define cglStencilValue stencil_t

// colors
#define cglColorGray tinymath::color_gray
#define cglColorRg tinymath::color_rg
#define cglColorRgb tinymath::color_rgb
#define cglColorRgba tinymath::color_rgba
#define cglColorRgb16 tinymath::color_rgb16f
#define cglColorRgba16 tinymath::color_rgba16f

#define cglEncodeGray ColorEncoding::encode_gray
#define cglEncodeRg ColorEncoding::encode_rg
#define cglEncodeRgb ColorEncoding::encode_rgb
#define cglEncodeRgba ColorEncoding::encode_rgba
#define cglEncodeRgb16 ColorEncoding::encode_rgb16f
#define cglEncodeRgba16 ColorEncoding::encode_rgba16f
#define cglDecode ColorEncoding::decode

#define cglPipelineFeature PipelineFeature
#define cglFaceCulling FaceCulling
#define cglVertexOrder VertexOrder
#define cglCompareFunc CompareFunc
#define cglDepthTest cglPerSampleOp::kDepthTest
#define cglStencilTest cglPerSampleOp::kStencilTest
#define cglBlend cglPerSampleOp::kBlending

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

	// window
	CGL_EXTERN void cglInitWindow(const char* title, size_t w, size_t h);
	CGL_EXTERN void cglGetMainWindowSize(size_t& w, size_t& h);
	CGL_EXTERN void cglAddResizeEvent(void(*on_resize)(size_t w, size_t h, void* ud), void* user_data);
	CGL_EXTERN int cglIsMainWindowOpen();
	CGL_EXTERN void cglSwapBuffer(int draw_immediately);
	CGL_EXTERN void cglCloseMainWindow();
	CGL_EXTERN void cglClearMainWindow();

	// context state
	CGL_EXTERN void cglEnable(cglPipelineFeature flag);
	CGL_EXTERN void cglDisable(cglPipelineFeature flag);
	CGL_EXTERN void cglCullFace(cglFaceCulling face);
	CGL_EXTERN void cglFrontFace(cglVertexOrder order);
	CGL_EXTERN void cglDepthFunc(cglCompareFunc func);	
	CGL_EXTERN void cglSetBlendFunc(cglBlendFunc func);
	CGL_EXTERN void cglSetStencilFunc(cglCompareFunc func);
	CGL_EXTERN void cglStencilMask(cglStencilValue ref_val, cglStencilValue write_mask, cglStencilValue read_mask);
	CGL_EXTERN void cglSetStencilOp(cglStencilOp pass_op, cglStencilOp fail_op, cglStencilOp zfail_op);
	CGL_EXTERN void cglSetBlendFactor(cglBlendFactor src_factor, cglBlendFactor dst_factor);
	CGL_EXTERN void cglSetColorMask(cglColorMask mask);

	// misc
	CGL_EXTERN void cglSetViewPort(size_t x, size_t y, size_t width, size_t height);
	CGL_EXTERN void cglGetViewport(size_t& x, size_t& y, size_t& width, size_t& height);
	CGL_EXTERN void cglSetSubSampleCount(uint8_t count);
	CGL_EXTERN uint8_t cglGetSubSampleCount();
	CGL_EXTERN void cglSetMultisampleFrequency(cglMultisampleFrequency frequency);
	CGL_EXTERN void cglSetClearColor(cglColor clear_color);
	CGL_EXTERN void cglClearBuffer(cglFrameContent content);
	CGL_EXTERN void cglSubmitPrimitive(cglVert v1, cglVert v2, cglVert v3);
	CGL_EXTERN void cglDrawSegment(cglVec3 start, cglVec3 end, cglMat4 mvp, cglColor col);
	CGL_EXTERN void cglFencePrimitives();
	CGL_EXTERN void cglFencePixels();
	CGL_EXTERN void cglSetActiveRenderTarget(uint32_t id);
	CGL_EXTERN void cglResetActiveRenderTarget();
	CGL_EXTERN void* cglGetTargetColorBuffer();
	CGL_EXTERN int cglGenId(uint32_t& id);
	CGL_EXTERN uint32_t cglCreateBuffer(size_t width, size_t height, cglFrameContent content);

	// texture
	CGL_EXTERN bool cglGenTexture(uint32_t& id);
	CGL_EXTERN bool cglActivateTexture(uint32_t id);
	CGL_EXTERN void cglTexImage2D(uint32_t id, size_t width, size_t height, cglTextureFormat fmt, cglPointer data);
	CGL_EXTERN void cglTexImage3D(uint32_t id, size_t width, size_t height, size_t layer_count, cglTextureFormat fmt, cglPointer data);
	CGL_EXTERN void cglGenerateMipmap();

	// shader
	CGL_EXTERN bool cglCreateProgram(CpuRasterizer::Shader* shader, uint32_t& id);
	CGL_EXTERN bool cglUseProgram(uint32_t id);
	CGL_EXTERN void cglUniform1i(uint32_t id, property_name prop_id, int v);
	CGL_EXTERN void cglUniform1f(uint32_t id, property_name prop_id, float v);
	CGL_EXTERN void cglUniform4fv(uint32_t id, property_name prop_id, cglVec4 v);
	CGL_EXTERN void cglUniformMatrix4fv(uint32_t id, property_name prop_id, cglMat4 mat);

	// tools
	CGL_EXTERN void cglDrawCoordinates(const cglVec3& pos, const cglVec3& forward, const cglVec3& up, const cglVec3& right, const cglMat4& m, const cglMat4& v, const cglMat4& p);

#ifdef __cplusplus
}
#endif



#pragma once
#include "tinymath.h"
#include "Window.hpp"
#include "GraphicsDevice.hpp"
#include "InputManager.hpp"
#include "Logger.hpp"

#define crApi CpuRasterApi
#define crGlobalData CpuRasterSharedData
#define crInput InputMgr

#define crWindowInit(title, w, h) CpuRasterizor::Window::initialize_main_window(title, w, h)
#define crMainWindow (*CpuRasterizor::Window::main())

#define crVert CpuRasterizor::Vertex

#define crGenId(id) crApi.try_alloc_id(id)

#define crMath tinymath

typedef crMath::vec2f crVec2;
typedef crMath::vec3f crVec3;
typedef crMath::vec4f crVec4;
typedef crMath::vec2i crVec2Int;
typedef crMath::vec3i crVec3Int;
typedef crMath::vec4i crVec4Int;
typedef crMath::mat3x3 crMat3;
typedef crMath::mat4x4 crMat4;

#define crDot tinymath::dot
#define crCross tinymath::cross
#define crMag tinymath::magnitude
#define crLen tinymath::length
#define crMax tinymath::max
#define crMin tinymath::min
#define crClamp tinymath::clamp
#define crNormalize tinymath::normalize
#define crLerp tinymath::lerp
#define crSaturate tinymath::saturate
#define crApprox tinymath::approx
#define crSin tinymath::sin
#define crCos tinymath::cos
#define crAsin tinymath::asin
#define crAcos tinymath::acos
#define crTan tinymath::tan
#define crAtan tinymath::atan
#define crAbs tinymath::abs
#define crFloor tinymath::floor
#define crCeil tinymath::ceil
#define crLog tinymath::log
#define crLog2 tinymath::log2
#define crPow tinymath::pow
#define crExp tinymath::exp
#define crExp2 tinymath::exp2
#define crTrunc tinymath::trunc
#define crSqrt tinymath::sqrt
#define crRotation tinymath::rotation
#define crDet tinymath::determinant
#define crInverse tinymath::inverse
#define crTranspose tinymath::transpose
#define crTranslation tinymath::translation
#define crScal tinymath::scale
#define crViewportMat tinymath::viewport
#define crLookat tinymath::lookat
#define crPerspective tinymath::perspective
#define crOrtho tinymath::ortho

#define crVec2Zero tinymath::kVec2fZero
#define crVec3Zero tinymath::kVec3fZero
#define crVec4Zero tinymath::kVec4fZero
#define crVec2One tinymath::kVec2fOne
#define crVec3One tinymath::kVec3fOne
#define crVec4One tinymath::kVec4fOne
#define crVec3Up tinymath::kVec3fZero
#define crVec3Down tinymath::kVec3fZero
#define crVec3Left tinymath::kVec3fZero
#define crVec3Right tinymath::kVec3fZero
#define crVec3Forward tinymath::kVec3fZero
#define crVec3Back tinymath::kVec3fZero

#define crMat3Identity tinymath::kMat3x3Identity
#define crMat4Identity tinymath::kMat4x4Identity
#define crMat3Zero tinymath::kMat3x3Zero
#define crMat4Zero tinymath::kMat4x4Zero

#define crPrint(...) Logger::log(Logger::Severity::kLog, __VA_ARGS__)
#define crError(...) Logger::log(Logger::Severity::kError, __VA_ARGS__)
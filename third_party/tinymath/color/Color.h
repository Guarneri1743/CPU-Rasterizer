#pragma once
#include "tinymath/MathDefine.h"
#include "tinymath/Vector.h"

TINYMATH_NAMESPACE

typedef Vector<float, 4> Color;

typedef struct
{
	uint8_t r; uint8_t g; uint8_t b;
} color_rgb;

typedef struct
{
	float r; float g; float b;
} color_rgb16f;

typedef struct
{
	uint8_t r; uint8_t g; uint8_t b; uint8_t a;
} color_rgba;

typedef struct
{
	float r; float g; float b; float a;
} color_rgba16f;

typedef struct
{
	uint8_t b; uint8_t g; uint8_t r; uint8_t a;
} color_bgra;

typedef struct
{
	uint8_t r; uint8_t g;
}color_rg;

typedef struct
{
	uint8_t gray;
}color_gray;


END_NAMESPACE
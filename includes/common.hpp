#pragma once
#include <cstdlib>
#include <cmath>
#include <limits.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <assert.h>
#include <vector>
#include <stack>

namespace guarneri{
	#define EPSILON 1e-05f
	#define EQUALS(a, b) std::abs(a-b) <= EPSILON
	#define FLOAT_LOG_PRECISION 6
	#define DEGREE2RAD(degree) degree * 0.0174532925f
	#define PI 3.1415926
	#define SCREEN_WIDTH 1024
	#define SCREEN_HEIGHT 768
	#define INVALID_TEXTURE_ID 0
	#define FLOOR(val) (int)(val)
	#define CEIL(val) (int)(val + 0.5f)
	

	typedef unsigned int int32_t;
	typedef unsigned int id_t;
	typedef int color_t;

	color_t CLAMP(color_t x, color_t min, color_t max) { return (x < min) ? min : ((x > max) ? max : x); }

	color_t encode_color(const float& r, const float& g, const float& b) {
		int R = (int)(r * 255.0f);
		int G = (int)(g * 255.0f);
		int B = (int)(b * 255.0f);
		R = CLAMP(R, 0, 255);
		G = CLAMP(G, 0, 255);
		B = CLAMP(B, 0, 255);
		color_t c = (R << 16) | (G << 8) | (B);
		return c;
	}
}
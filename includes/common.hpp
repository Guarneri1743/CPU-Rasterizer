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
	#define EPSILON 1e-04f
	#define EQUALS(a, b) std::abs(a-b) <= EPSILON
	#define FLOAT_LOG_PRECISION 6
	#define DEGREE2RAD(degree) degree * 0.0174532925f
	#define PI 3.1415926
	#define SCREEN_WIDTH 1024
	#define SCREEN_HEIGHT 768
	#define INVALID_TEXTURE_ID 0
	#define FLOOR(val) (int)(val)
	#define CEIL(val) (int)(val + 0.5f)
	#define FRAC(val) val - (long)val
	#define STEP(y, x) x >= y ? 1 : 0 
	
	typedef unsigned int int32_t;
	typedef unsigned int id_t;
	typedef int color_t;
	typedef std::string texture_id;
	typedef std::string property_name;
	typedef std::string shader_id;

	typedef struct {
		float camera_near;
		float camera_far;
		float camera_fov;
		float screen_width;
		float screen_height;
	} misc;

	static misc misc_params;

	void update_misc_params(const float& width, const float& height, const float& near, const float& far, const float& fov) {
		misc_params.screen_width = width;
		misc_params.screen_height = height;
		misc_params.camera_near = near;
		misc_params.camera_far = far;
		misc_params.camera_fov = fov;
	}
}
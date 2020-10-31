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
#include <filesystem>

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
	#define FLOOR_UINT(val) (uint32_t)(val)
	#define CEIL_UINT(val) (uint32_t)(val + 0.5f)
	#define FRAC(val) val - (long)val
	#define STEP(y, x) x >= y ? 1 : 0 
	#define FS std::filesystem
	
	typedef std::string texture_id;
	typedef std::string property_name;
	typedef std::string shader_id;

	typedef struct {
		float camera_near;
		float camera_far;
		float camera_fov;
		uint32_t screen_width;
		uint32_t screen_height;
	} misc;

	static misc misc_params;

	static void update_misc_params(const uint32_t& width, const uint32_t& height, const float& near, const float& far, const float& fov) {
		misc_params.screen_width = width;
		misc_params.screen_height = height;
		misc_params.camera_near = near;
		misc_params.camera_far = far;
		misc_params.camera_fov = fov;
	}

	static std::string replace(std::string str, std::string pattern, std::string content) {
		while (str.find(pattern) != std::string::npos) {
			str.replace(str.find(pattern), 1, content);
		}
		return str;
	}

	static std::string cur_path() {
		std::string ret = FS::current_path().string();
		return replace(ret, "\\", "/") + "/..";
	}

	static std::string res_path() {
		return cur_path() + "/res";
	}
}
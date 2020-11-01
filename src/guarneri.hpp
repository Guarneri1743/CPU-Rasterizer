#pragma once
#include <cstdlib>
#include <cmath>
#include <limits.h>
#include <algorithm>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <assert.h>
#include <filesystem>
#include <singleton.hpp>

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
	
	// resource id
	typedef std::string texture_id;
	typedef std::string property_name;
	typedef std::string shader_id;

	// color 
	typedef struct { unsigned char r; unsigned char g; unsigned char b; } color_rgb;
	typedef struct { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } color_rgba;
	typedef struct { unsigned char b; unsigned char g; unsigned char r; unsigned char a; } color_bgra;

	// simple texture properties
	const property_name albedo_prop = "texture_diffuse";
	const property_name specular_prop = "texture_specular";
	const property_name normal_prop = "texture_normal";
	const property_name height_prop = "texture_height";

	// utils
	static std::string replace(std::string str, std::string pattern, std::string content) {
		while (str.find(pattern) != std::string::npos) {
			str.replace(str.find(pattern), 1, content);
		}
		return str;
	}

	static std::string cur_path() {
		std::string ret = FS::current_path().string();
#if _DEBUG
		ret += "/bin/Debug";
#else
		ret += "/bin/Release";
#endif
		return replace(ret, "\\", "/");
	}

	static std::string res_path() {
		return cur_path() + "/res";
	}

	template <class T>
	inline void unused(T const&){}

	#define REF(obj) unused(obj)

	// forward declarations
	struct float2;
	struct float3;
	struct float4;
	struct mat4;
	struct transform;
	struct color;
	struct light;
	struct vertex;
	struct ray;
	struct line;
	struct aabb;
	struct aabb2d;
	struct plane;
	struct triangle;
	struct segment;
	struct cylinder;
	struct capsule;
	struct light;

	class mesh;
	class camera;
	class line_drawer;
	class texture;
	class shader;
	class material;
	class model;
	class renderer;
	class render_device;
	class gdi_window;
}

#define NOMINMAX

#include <gdi_window.hpp>

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

#if WIN32
#define WIN32_LEAN_AND_MEAN
#endif

#include <float2.hpp>
#include <float3.hpp>
#include <float4.hpp>
#include <color.hpp>
#include <mat4.hpp>
#include <transform.hpp>
#include <vertex.hpp>
#include <ray.hpp>
#include <line.hpp>
#include <segment.hpp>
#include <cylinder.hpp>
#include <capsule.hpp>
#include <aabb.hpp>
#include <aabb2d.hpp>
#include <plane.hpp>
#include <triangle.hpp>
#include <light.hpp>
#include <raw_buffer.hpp>
#include <texture.hpp>
#include <camera.hpp>
#include <line_drawer.hpp>
#include <noise.hpp>
#include <shader.hpp>
#include <material.hpp>
#include <mesh.hpp>
#include <model.hpp>
#include <render_device.hpp>
#include <renderer.hpp>
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
#include <iomanip>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <filesystem>

namespace guarneri {
	#define LEFT_HANDED
	#define MAX_ID UINT_MAX
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
	#define CAMERA_ROTATE_SPEED 0.25f
	#define CAMERA_MOVE_SPEED 0.1f
	#define CAMERA_ZOOM_SPEED 1.0f

	// resource id
	typedef std::string texture_id;
	typedef std::string property_name;
	typedef std::string shader_id;

	// simple texture properties
	const property_name albedo_prop = "texture_diffuse";
	const property_name specular_prop = "texture_specular";
	const property_name normal_prop = "texture_normal";
	const property_name height_prop = "texture_height";

	// color 
	typedef struct { unsigned char r; unsigned char g; unsigned char b; } color_rgb;
	typedef struct { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } color_rgba;
	typedef struct { unsigned char b; unsigned char g; unsigned char r; unsigned char a; } color_bgra;
	typedef unsigned char stb_uchar;

	enum class projection {
		perspective,
		orthographic
	};

	enum class wrap_mode {
		repeat,
		clamp
	};

	enum class filtering {
		point,
		bilinear
	};

	// todo: support other formats
	enum class texture_format {
		invalid,
		rgb,
		rgba
	};

	enum class ztest {
		always,
		less,
		less_equal,
		equal,
		greater_equal,
		greater,
		not_equal
	};

	enum class zwrite {
		on,
		off,
		fragment_z,
		early_z
	};

	enum class render_flag {
		disable = 0,
		wire_frame = 1 << 0,
		shaded = 1 << 1,
		depth = 1 << 2,
		uv = 1 << 3,
		vertex_color = 1 << 4,
		normal = 1 << 5,
		scanline = 1 << 6,
		transparent = 1 << 7
	};

	// forward declarations
	class object;
	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct Matrix3x3;
	struct Matrix4x4;
	struct transform;
	struct color;
	struct light;
	struct vertex;
	struct Ray;
	struct Line;
	struct BoundingBox;
	struct BoundingBox2D;
	struct Plane;
	struct Triangle;
	struct Segment;
	struct Cylinder;
	struct Capsule;
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
	class PrimitiveFactory;
	class scene;
	class id_allocator;
	class input_manager;
	struct directional_light;

	static std::string to_string(const texture_format& fmt) {
		switch (fmt) {
		case texture_format::invalid:
			return "invalid";
		case texture_format::rgb:
			return "rgb";
		case texture_format::rgba:
			return "rgba";
		}
		return "invalid";
	};

	static std::ostream& operator <<(std::ostream& os, const texture_format& fmt) {
		os << to_string(fmt);
		return os;
	};

	static std::stringstream& operator <<(std::stringstream& ss, const texture_format& fmt) {
		ss << to_string(fmt);
		return ss;
	};
}

// common
#include <singleton.hpp>
#include <resource_manager.hpp>
#include <id_allocator.hpp>

// math
#include <Vector2.hpp>
#include <Vector3.hpp>
#include <Vector4.hpp>
#include <color.hpp>
#include <Matrix4x4.hpp>
#include <Matrix3x3.hpp>

// lighting 
#include <light.hpp>

namespace guarneri{
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

	render_device& graphics() {
		return singleton<render_device>::get();
	}

	input_manager& input_mgr() {
		return singleton<input_manager>::get();
	}

	gdi_window& window() {
		return singleton<gdi_window>::get();
	}

	resource_manager<texture>& tex_mgr() {
		return singleton<resource_manager<texture>>::get();
	}

	id_allocator idalloc(INVALID_ID + 1, MAX_ID);
	#define ALLOC_ID() idalloc.alloc();
	#define FREE_ID(id) idalloc.free(id);

	typedef struct {
		float cam_near;
		float cam_far;
		float screen_width;
		float screen_height;
		Vector3 camera_pos;
		Matrix4x4 view_matrix;
		Matrix4x4 proj_matrix;
		directional_light main_light;
		render_flag flag;
	} misc_parameter;
	static misc_parameter misc_param;

	struct lighting_data {
		float glossiness;
	};
}

// windows api
#define NOMINMAX
#include <windows.h>
#include <tchar.h>
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

// rasterizer
#include <object.hpp>
#include <input_manager.hpp>
#include <transform.hpp>
#include <vertex.hpp>
#include <Plane.hpp>
#include <Ray.hpp>
#include <Line.hpp>
#include <Segment.hpp>
#include <Cylinder.hpp>
#include <Capsule.hpp>
#include <BoundingBox.hpp>
#include <BoundingBox2D.hpp>
#include <Triangle.hpp>
#include <light.hpp>
#include <raw_buffer.hpp>
#include <texture.hpp>
#include <line_drawer.hpp>
#include <noise.hpp>
#include <shader.hpp>
#include <material.hpp>
#include <mesh.hpp>
#include <model.hpp>
#include <render_device.hpp>
#include <camera.hpp>
#include <renderer.hpp>
#include <PrimitiveFactory.hpp>
#include <scene.hpp>

namespace guarneri {
	void prepare(const uint32_t w, const uint32_t h, LPCSTR title) {
		window().initialize(w, h, title, input_mgr().event_callback);
		input_mgr().add_on_key_down_evt([](key_code code, void* data) { if (code == key_code::ESC) window().dispose(); }, nullptr);
		graphics().initialize(window().framebuffer, window().width, window().height);
	}

	void kick_off(scene& scene) {
		while (window().is_valid()) {
			graphics().clear_buffer();
			input_mgr().update();
			graphics().update();
			scene.update();
			scene.render();
			window().flush();
			Sleep(0);
		}
	}
}
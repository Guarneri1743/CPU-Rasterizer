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

namespace Guarneri {
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

	// simple Texture properties
	const property_name albedo_prop = "texture_diffuse";
	const property_name specular_prop = "texture_specular";
	const property_name normal_prop = "texture_normal";
	const property_name height_prop = "texture_height";

	// Color 
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
	class Object;
	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct Matrix3x3;
	struct Matrix4x4;
	struct Transform;
	struct Color;
	struct Light;
	struct Vertex;
	struct Ray;
	struct Line;
	struct BoundingBox;
	struct BoundingBox2D;
	struct Plane;
	struct Triangle;
	struct Segment;
	struct Cylinder;
	struct Capsule;
	struct Light;
	class Mesh;
	class Camera;
	class SegmentDrawer;
	class Texture;
	class Shader;
	class Material;
	class Model;
	class Renderer;
	class GraphicsDevice;
	class GDIWindow;
	class PrimitiveFactory;
	class Scene;
	class IdAllocator;
	class InputManager;
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
#include <Singleton.hpp>
#include <ResourceManager.hpp>
#include <IdAllocator.hpp>

// math
#include <Vector2.hpp>
#include <Vector3.hpp>
#include <Vector4.hpp>
#include <Color.hpp>
#include <Matrix4x4.hpp>
#include <Matrix3x3.hpp>

// lighting 
#include <Light.hpp>

namespace Guarneri{
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

	GraphicsDevice& graphics() {
		return Singleton<GraphicsDevice>::get();
	}

	InputManager& input_mgr() {
		return Singleton<InputManager>::get();
	}

	GDIWindow& window() {
		return Singleton<GDIWindow>::get();
	}

	ResourceManager<Texture>& tex_mgr() {
		return Singleton<ResourceManager<Texture>>::get();
	}

	IdAllocator idalloc(INVALID_ID + 1, MAX_ID);
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
#include <GDIWindow.hpp>
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
#include <Object.hpp>
#include <InputManager.hpp>
#include <Transform.hpp>
#include <Vertex.hpp>
#include <Plane.hpp>
#include <Ray.hpp>
#include <Line.hpp>
#include <Segment.hpp>
#include <Cylinder.hpp>
#include <Capsule.hpp>
#include <BoundingBox.hpp>
#include <BoundingBox2D.hpp>
#include <Triangle.hpp>
#include <Light.hpp>
#include <RawBuffer.hpp>
#include <Texture.hpp>
#include <SegmentDrawer.hpp>
#include <Noise.hpp>
#include <Shader.hpp>
#include <Material.hpp>
#include <Mesh.hpp>
#include <Model.hpp>
#include <GraphicsDevice.hpp>
#include <Camera.hpp>
#include <Renderer.hpp>
#include <PrimitiveFactory.hpp>
#include <Scene.hpp>

namespace Guarneri {
	void prepare(const uint32_t w, const uint32_t h, LPCSTR title) {
		window().initialize(w, h, title, input_mgr().event_callback);
		input_mgr().add_on_key_down_evt([](key_code code, void* data) { if (code == key_code::ESC) window().dispose(); }, nullptr);
		graphics().initialize(window().framebuffer, window().width, window().height);
	}

	void kick_off(Scene& Scene) {
		while (window().is_valid()) {
			graphics().clear_buffer();
			input_mgr().update();
			graphics().update();
			Scene.update();
			Scene.render();
			window().flush();
			Sleep(0);
		}
	}
}
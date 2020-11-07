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
#include <chrono>

namespace Guarneri {
	#define LEFT_HANDED
	#define MAX_ID UINT_MAX
	#define EPSILON 1e-04f
	#define EQUALS(a, b) (std::abs(a-b) <= EPSILON)
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

	enum class Projection {
		PERSPECTIVE,
		ORTHO
	};

	enum class WrapMode {
		REPEAT,
		CLAMP
	};

	enum class Filtering {
		POINT,
		BILINEAR
	};

	// todo: support other formats
	enum class TextureFormat {
		INVALID,
		rgb,
		rgba
	};

	enum class ZTest {
		ALWAYS,
		LESS,
		LEQUAL,
		EQUAL,
		GEQUAL,
		GREATER,
		NOT_EQUAL
	};

	enum class ZWrite {
		ON,
		OFF,
		EARLY_Z
	};

	enum class BlendFactor {
		ONE,
		SRC_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,
		ONE_MINUS_SRC_COLOR,
		DST_COLOR,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA,
		ONE_MINUS_DST_COLOR
	};

	enum class BlendOp {
		ADD,
		SUB
	};

	enum class RenderFlag {
		DISABLE = 0,
		WIREFRAME = 1 << 0,
		SHADED = 1 << 1,
		DEPTH = 1 << 2,
		UV = 1 << 3,
		VERTEX_COLOR = 1 << 4,
		NORMAL = 1 << 5,
		SCANLINE = 1 << 6,
		SEMI_TRANSPARENT = 1 << 7
	};

	enum class KeyCode {
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		ESC,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		SPACE,
		CTRL_L,
		ALT_L,
		CTRL_R,
		ALT_R,
		SHIFT_L,
		SHIFT_R,
		ENTER,
		WIN
	};

	enum class MouseButton {
		LEFT,
		RIGHT,
		MIDDLE
	};

	enum class MouseWheel {
		UP,
		DOWN
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
	struct Ray2D;
	struct Line;
	struct BoundingBox;
	struct BoundingBox2D;
	struct Plane;
	struct Triangle;
	struct Segment;
	struct Cylinder;
	struct Capsule;
	struct Light;
	struct DirectionalLight;
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
	class CubeMap;
	class SkyboxShader;
	class SkyboxRenderer;

	unsigned char CLAMP(unsigned char x, unsigned char min, unsigned char max) { return (x < min) ? min : ((x > max) ? max : x); }
	uint32_t CLAMP_UINT(uint32_t x, uint32_t min, uint32_t max) { return (x < min) ? min : ((x > max) ? max : x); }
	int CLAMP_INT(int x, int min, int max) { return (x < min) ? min : ((x > max) ? max : x); }
	float CLAMP_FLT(float x, float min, float max) { return (x < min) ? min : ((x > max) ? max : x); }

	static std::string to_string(const TextureFormat& fmt) {
		switch (fmt) {
		case TextureFormat::INVALID:
			return "invalid";
		case TextureFormat::rgb:
			return "rgb";
		case TextureFormat::rgba:
			return "rgba";
		}
		return "invalid";
	};

	static std::ostream& operator <<(std::ostream& os, const TextureFormat& fmt) {
		os << to_string(fmt);
		return os;
	};

	static std::stringstream& operator <<(std::stringstream& ss, const TextureFormat& fmt) {
		ss << to_string(fmt);
		return ss;
	};

	static std::chrono::steady_clock::time_point start_time;

	static void start_watch() {
		start_time = std::chrono::steady_clock::now();
	}

	static float stop_watch() {
		auto end = std::chrono::steady_clock::now();
		return (float)std::chrono::duration_cast<std::chrono::microseconds>(end - start_time).count() / (float)1000.0f;
	}
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
#include <Plane.hpp>
#include <Ray.hpp>
#include <Ray2D.hpp>
#include <Line.hpp>
#include <Segment.hpp>
#include <Cylinder.hpp>
#include <Capsule.hpp>
#include <BoundingBox.hpp>
#include <BoundingBox2D.hpp>
#include <Frustum.hpp>

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

	#define REF(obj) unused(obj);

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
		int screen_width;
		int screen_height;
		Vector3 camera_pos;
		Matrix4x4 view_matrix;
		Matrix4x4 proj_matrix;
		DirectionalLight main_light;
		RenderFlag flag;
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
#include <CubeMap.hpp>
#include <SkyboxShader.hpp>
#include <SkyboxRenderer.hpp>
#include <Scene.hpp>


namespace Guarneri {
	float fps;
	float cost;
	float frame_count;

	void prepare(const uint32_t w, const uint32_t h, LPCSTR title) {
		window().initialize(w, h, title, input_mgr().event_callback);
		input_mgr().add_on_key_down_evt([](KeyCode code, void* data) { 
			REF(data)
			if (code == KeyCode::ESC) window().dispose(); 
		}, nullptr);
		graphics().initialize(window().framebuffer, window().width, window().height);
	}

	void kick_off(Scene& scene) {
		while (window().is_valid()) {
			start_watch();
			graphics().clear_buffer();
			input_mgr().update();
			graphics().update();
			scene.update();
			scene.render();
			window().flush();
			frame_count += 1.0f;
			cost += stop_watch() / 1000.0f;
			fps = frame_count / cost;
			std::stringstream ss;
			ss << "SoftRasterizer  FPS: " << (int)fps;
			window().set_title(ss.str().c_str());
			Sleep(0);
		}
	}
}
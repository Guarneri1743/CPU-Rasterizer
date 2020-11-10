#ifndef _COMMON_DEF_
#define _COMMON_DEF_

namespace Guarneri {
	#define MAX_ID UINT_MAX
	#define EPSILON 1e-04f
	#define ONE_MINUS_EPSILON 1 - EPSILON
	#define EQUALS(a, b) std::abs(a-b) <= EPSILON
	#define LESS_THAN(a, b) (a - b < -EPSILON)
	#define GREATER_THAN(a, b) (a - b > EPSILON)
	#define GREATAER_EQUAL(a, b) (a - b > EPSILON || EQUALS(a, b))
	#define LESS_EQUAL(a, b) (a - b < -EPSILON || EQUALS(a, b))
	#define FLOAT_LOG_PRECISION 6
	#define DEGREE2RAD(degree) degree * 0.0174532925f
	#define PI 3.1415926
	#define INVALID_TEXTURE_ID 0
	#define FRAC(val) val - (long)val
	#define STEP(y, x) x >= y ? 1 : 0 
	#define FS std::filesystem
	#define CAMERA_ROTATE_SPEED 0.25f
	#define CAMERA_MOVE_SPEED 0.1f
	#define CAMERA_ZOOM_SPEED 1.0f
	#define RGB_CHANNEL 3
	#define RGBA_CHANNEL 4
	#define TEXTURE_MAX_SIZE 4096

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
	struct Sphere;
	struct Triangle;
	struct Segment;
	struct Cylinder;
	struct Capsule;
	struct Light;
	struct DirectionalLight;
	struct MiscParameter;
	struct LightingData;
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
	class Clipper;

	typedef std::string property_name;
	const property_name albedo_prop = "texture_diffuse";
	const property_name specular_prop = "texture_specular";
	const property_name glossiness_prop = "texture_glossiness";
	const property_name normal_prop = "texture_normal";
	const property_name ao_prop = "texture_ao";
	const property_name skybox_cubemap_prop = "skybox_cubemap";

	// statistics
	struct GraphicsStatistic{
		uint32_t triangle_count;
		uint32_t culled_triangle_count;
	};

	// framebuffer color 
	typedef struct { unsigned char r; unsigned char g; unsigned char b; } color_rgb;
	typedef struct { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } color_rgba;
	typedef struct { unsigned char b; unsigned char g; unsigned char r; unsigned char a; } color_bgra;
	typedef unsigned char image_ubyte;

	GraphicsDevice& Graphics();
	InputManager& InputMgr();
	GDIWindow& Window();
	ResourceManager<Texture>& TextureMgr();

	enum class MouseButton {
		LEFT,
		RIGHT,
		MIDDLE
	};

	enum class MouseWheel {
		UP,
		DOWN
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
}

#endif
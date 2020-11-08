#ifndef _COMMON_DEF_
#define _COMMON_DEF_

namespace Guarneri {
	#define MAX_ID UINT_MAX
	#define EPSILON 1e-04f
	#define EQUALS(a, b) (std::abs(a-b) <= EPSILON)
	#define FLOAT_LOG_PRECISION 6
	#define DEGREE2RAD(degree) degree * 0.0174532925f
	#define PI 3.1415926
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
	const property_name normal_prop = "texture_normal";
	const property_name height_prop = "texture_height";

	// framebuffer color 
	typedef struct { unsigned char r; unsigned char g; unsigned char b; } color_rgb;
	typedef struct { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } color_rgba;
	typedef struct { unsigned char b; unsigned char g; unsigned char r; unsigned char a; } color_bgra;
	typedef unsigned char stb_uchar;

	GraphicsDevice& graphics();
	InputManager& input_mgr();
	GDIWindow& window();
	ResourceManager<Texture>& tex_mgr();

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
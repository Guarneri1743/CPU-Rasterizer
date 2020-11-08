#ifndef _PIPELINE_DEF_
#define _PIPELINE_DEF_

namespace Guarneri{
	#define LEFT_HANDED

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
		OFF
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

	enum class CullingAndClippingFlag {
		DISABLE = 0,
		APP_FRUSTUM_CULLING = 1 << 0,
		NEAR_PLANE_CLIPPING = 1 << 1,
		SCREEN_CLIPPING = 1 << 2,
		BACK_FACE_CULLING = 1 << 3
	};

	enum class PerSampleOperation {
		DISABLE = 0,
		SCISSOR_TEST = 1 << 0,
		ALPHA_TEST = 1 << 1,
		STENCIL_TEST = 1 << 2,
		DEPTH_TEST = 1 << 3,
		BLENDING = 1 << 4
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

	struct MiscParameter {
		float cam_near;
		float cam_far;
		int screen_width;
		int screen_height;
		Vector3 camera_pos;
		Matrix4x4 view_matrix;
		Matrix4x4 proj_matrix;
		DirectionalLight main_light;
		RenderFlag flag;
	};

	struct LightingData {
		float glossiness;
	};

	static MiscParameter misc_param;
}

#endif
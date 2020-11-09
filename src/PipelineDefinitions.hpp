#ifndef _PIPELINE_DEF_
#define _PIPELINE_DEF_

namespace Guarneri{
	#define LEFT_HANDED
	#define FAR_Z 1.0f
	#define DEFAULT_STENCIL 0

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

	enum class BufferFlag {
		NONE = 0,
		COLOR = 1 << 0,
		DEPTH = 1 << 1,
		STENCIL = 1 << 2
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

	template<>
	struct support_bitwise_enum<RenderFlag> : std::true_type {};

	template<>
	struct support_bitwise_enum<BufferFlag> : std::true_type {};

	template<>
	struct support_bitwise_enum<PerSampleOperation> : std::true_type {};

	template<>
	struct support_bitwise_enum<CullingAndClippingFlag> : std::true_type {};
}

#endif
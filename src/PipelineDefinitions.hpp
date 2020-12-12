#ifndef _PIPELINE_DEF_
#define _PIPELINE_DEF_

namespace Guarneri{
	#define LEFT_HANDED
	#define FAR_Z 1.0f
	#define DEFAULT_STENCIL 0x00

	enum class RasterizerStrategy {
		SCANBLOCK,
		SCANLINE
	};

	enum class PBRWorkFlow
	{
		Metallic,
		Specular
	};

	enum class Projection {
		PERSPECTIVE,
		ORTHO
	};

	enum class WrapMode {
		REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER
	};

	enum class Filtering {
		POINT,
		BILINEAR,
		MAX,
		MIN
	};

	enum class TextureFormat {
		INVALID,
		rgb,
		rgba,
		r32
	};

	enum class CompareFunc {
		NEVER,
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

	enum class StencilOp {
		KEEP,
		ZERO,
		REPLACE,
		INCR,
		INCR_WRAP,
		DECR,
		DECR_WRAP,
		INVERT
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

	enum class ColorMask {
		ZERO = 0,
		R = 1 << 0,
		G = 1 << 1,
		B = 1 << 2,
		A = 1 << 3
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
		SHADOWMAP = 1 << 1,
		DEPTH = 1 << 2,
		UV = 1 << 3,
		VERTEX_COLOR = 1 << 4,
		NORMAL = 1 << 5,
		MIPMAP = 1 << 6,
		STENCIL = 1 << 7,
		SCANLINE = 1 << 8,
		FRAME_TILE = 1 << 9,
		SPECULAR = 1 << 10
	};

#ifdef OPAQUE
#undef OPAQUE
#endif
#ifdef TRANSPARENT
#undef TRANSPARENT
#endif

	enum class RenderPass {
		OBJECT,
		SHADOW,
		SKYBOX
	};

	template<>
	struct support_bitwise_enum<RenderFlag> : std::true_type {};

	template<>
	struct support_bitwise_enum<BufferFlag> : std::true_type {};

	template<>
	struct support_bitwise_enum<PerSampleOperation> : std::true_type {};

	template<>
	struct support_bitwise_enum<CullingAndClippingFlag> : std::true_type {};

	template<>
	struct support_bitwise_enum<ColorMask> : std::true_type {};


	static std::ostream& operator << (std::ostream& stream, const RenderFlag& flag) {
		int count = 0;
		if ((flag & RenderFlag::WIREFRAME) != RenderFlag::DISABLE) {
			stream << "WIREFRAME";
			count++;
		}
		if ((flag & RenderFlag::DEPTH) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | DEPTH" : "DEPTH");
			count++;
		}
		if ((flag & RenderFlag::UV) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | UV" : "UV");
			count++;
		}
		if ((flag & RenderFlag::VERTEX_COLOR) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | VERTEX_COLOR" : "VERTEX_COLOR");
			count++;
		}
		if ((flag & RenderFlag::NORMAL) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | NORMAL" : "NORMAL");
			count++;
		}
		if ((flag & RenderFlag::MIPMAP) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | MIPMAP" : "MIPMAP");
			count++;
		}
		if ((flag & RenderFlag::STENCIL) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | STENCIL" : "STENCIL");
			count++;
		}
		if ((flag & RenderFlag::SCANLINE) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | SCANLINE" : "SCANLINE");
			count++;
		}
		if ((flag & RenderFlag::FRAME_TILE) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | FRAME_TILE" : "FRAME_TILE");
			count++;
		}
		if ((flag & RenderFlag::SHADOWMAP) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | SHADOWMAP" : "SHADOWMAP");
			count++;
		}
		if ((flag & RenderFlag::SPECULAR) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | SPECULAR" : "SPECULAR");
			count++;
		}
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const RenderFlag& flag) {
		int count = 0;
		if ((flag & RenderFlag::WIREFRAME) != RenderFlag::DISABLE) {
			stream << "WIREFRAME";
			count++;
		}
		if ((flag & RenderFlag::DEPTH) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | DEPTH" : "DEPTH");
			count++;
		}
		if ((flag & RenderFlag::UV) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | UV" : "UV");
			count++;
		}
		if ((flag & RenderFlag::VERTEX_COLOR) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | VERTEX_COLOR" : "VERTEX_COLOR");
			count++;
		}
		if ((flag & RenderFlag::NORMAL) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | NORMAL" : "NORMAL");
			count++;
		}
		if ((flag & RenderFlag::MIPMAP) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | MIPMAP" : "MIPMAP");
			count++;
		}
		if ((flag & RenderFlag::STENCIL) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | STENCIL" : "STENCIL");
			count++;
		}
		if ((flag & RenderFlag::SCANLINE) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | SCANLINE" : "SCANLINE");
			count++;
		}
		if ((flag & RenderFlag::FRAME_TILE) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | FRAME_TILE" : "FRAME_TILE");
			count++;
		}
		if ((flag & RenderFlag::SHADOWMAP) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | SHADOWMAP" : "SHADOWMAP");
			count++;
		}
		if ((flag & RenderFlag::SPECULAR) != RenderFlag::DISABLE) {
			stream << (count > 0 ? " | SPECULAR" : "SPECULAR");
			count++;
		}
		return stream;
	}

	static std::ostream& operator << (std::ostream& stream, const CullingAndClippingFlag& flag) {
		int count = 0;
		if ((flag & CullingAndClippingFlag::APP_FRUSTUM_CULLING) != CullingAndClippingFlag::DISABLE) {
			stream << "APP_FRUSTUM_CULLING";
			count++;
		}
		if ((flag & CullingAndClippingFlag::BACK_FACE_CULLING) != CullingAndClippingFlag::DISABLE) {
			stream << (count > 0 ? " | BACK_FACE_CULLING" : "BACK_FACE_CULLING");
			count++;
		}
		if ((flag & CullingAndClippingFlag::NEAR_PLANE_CLIPPING) != CullingAndClippingFlag::DISABLE) {
			stream << (count > 0 ? " | NEAR_PLANE_CLIPPING" : "NEAR_PLANE_CLIPPING");
			count++;
		}
		if ((flag & CullingAndClippingFlag::SCREEN_CLIPPING) != CullingAndClippingFlag::DISABLE) {
			stream << (count > 0 ? " | SCREEN_CLIPPING" : "SCREEN_CLIPPING");
			count++;
		}
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const CullingAndClippingFlag& flag) {
		int count = 0;
		if ((flag & CullingAndClippingFlag::APP_FRUSTUM_CULLING) != CullingAndClippingFlag::DISABLE) {
			stream << "APP_FRUSTUM_CULLING";
			count++;
		}
		if ((flag & CullingAndClippingFlag::BACK_FACE_CULLING) != CullingAndClippingFlag::DISABLE) {
			stream << (count > 0 ? " | BACK_FACE_CULLING" : "BACK_FACE_CULLING");
			count++;
		}
		if ((flag & CullingAndClippingFlag::NEAR_PLANE_CLIPPING) != CullingAndClippingFlag::DISABLE) {
			stream << (count > 0 ? " | NEAR_PLANE_CLIPPING" : "NEAR_PLANE_CLIPPING");
			count++;
		}
		if ((flag & CullingAndClippingFlag::SCREEN_CLIPPING) != CullingAndClippingFlag::DISABLE) {
			stream << (count > 0 ? " | SCREEN_CLIPPING" : "SCREEN_CLIPPING");
			count++;
		}
		return stream;
	}
}

#endif
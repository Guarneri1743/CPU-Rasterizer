#ifndef _GRAPHICS_UTILITY_
#define _GRAPHICS_UTILITY_
#include <Guarneri.hpp>

namespace Guarneri {
	struct MiscParameter {
		float cam_near;
		float cam_far;
		Vector3 camera_pos;
		Matrix4x4 view_matrix;
		Matrix4x4 proj_matrix;
		DirectionalLight main_light;
		RenderFlag flag;
	};
	static MiscParameter misc_param;

	struct LightingData {
		float glossiness;
	};

	GraphicsDevice& Graphics() {
		return Singleton<GraphicsDevice>::get();
	}

	InputManager& InputMgr() {
		return Singleton<InputManager>::get();
	}

	GDIWindow& Window() {
		return Singleton<GDIWindow>::get();
	}

	ResourceManager<Texture>& TextureMgr() {
		return Singleton<ResourceManager<Texture>>::get();
	}
}

#endif
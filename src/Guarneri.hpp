#ifndef _GUARNERI_
#define _GUARNERI_

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

#define NOMINMAX
#include <windows.h>
#include <tchar.h>
#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif
#if WIN32
#define WIN32_LEAN_AND_MEAN
#endif

// Rasterizer Core
#include <GDIWindow.hpp>
#include <Singleton.hpp>
#include <IdAllocator.hpp>
#include <ResourceManager.hpp>
#include <CommonDefinitions.hpp>
#include <Utility.hpp>
#include <Time.hpp>
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
#include <Sphere.hpp>
#include <Cylinder.hpp>
#include <Capsule.hpp>
#include <BoundingBox.hpp>
#include <BoundingBox2D.hpp>
#include <Frustum.hpp>
#include <Vertex.hpp>
#include <Triangle.hpp>
#include <Light.hpp>
#include <PipelineDefinitions.hpp>
#include <Object.hpp>
#include <Clipper.hpp>
#include <InputManager.hpp>
#include <Transform.hpp>
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
	class GuarneriRasterizer {
	public:
		static void prepare(const uint32_t w, const uint32_t h, LPCSTR title) {
			window().initialize(w, h, title, input_mgr().event_callback);
			input_mgr().add_on_key_down_evt([](KeyCode code, void* data) {
				REF(data)
					if (code == KeyCode::ESC) window().dispose();
				}, nullptr);
			graphics().initialize(window().framebuffer, window().width, window().height);
			Time::start();
		}

		static void kick_off(Scene& scene) {
			while (window().is_valid()) {
				graphics().clear_buffer();
				input_mgr().update();
				scene.update();
				scene.render();
				window().flush();
				Time::update();
				std::stringstream ss;
				ss << "SoftRasterizer  FPS: " << (int)Time::fps;
				window().set_title(ss.str().c_str());
				Sleep(0);
			}
		}
	};
}
#endif
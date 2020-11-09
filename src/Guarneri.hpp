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
#include <BitwiseEnum.hpp>
#include <PipelineDefinitions.hpp>
#include <GDIWindow.hpp>
#include <Singleton.hpp>
#include <IdAllocator.hpp>
#include <ResourceManager.hpp>
#include <CommonDefinitions.hpp>
#include <Utility.hpp>
#include <Object.hpp>
#include <Time.hpp>
#include <Vector2.hpp>
#include <Vector3.hpp>
#include <Vector4.hpp>
#include <Color.hpp>
#include <Light.hpp>
#include <Matrix4x4.hpp>
#include <Matrix3x3.hpp>
#include <Misc.hpp>
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
			Window().initialize(w, h, title, InputMgr().event_callback);
			InputMgr().add_on_key_down_evt([](KeyCode code, void* data) {
					REF(data)
					if (code == KeyCode::ESC) Window().dispose();
				}, nullptr);
			Graphics().initialize(Window().framebuffer, Window().width, Window().height);
			Time::start();
		}

		static void kick_off(Scene& scene) {
			while (Window().is_valid()) {
				Graphics().clear_buffer(BufferFlag::COLOR | BufferFlag::DEPTH | BufferFlag::STENCIL);
				InputMgr().update();
				scene.update();
				scene.render();
				Window().flush();
				Time::update();
				std::stringstream ss;
				ss << "SoftRasterizer  FPS: " << (int)Time::fps;
				Window().set_title(ss.str().c_str());
				Sleep(0);
			}
		}
	};
}
#endif
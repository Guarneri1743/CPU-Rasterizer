#ifndef _GUARNERI_
#define _GUARNERI_

#define MULTI_THREAD

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
#include <thread>
#include <mutex>

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

template <class T>
inline void unused(T const&) {}
#define REF(obj) unused(obj);

// Rasterizer Core
#include <BitwiseEnum.hpp>
#include <ThreadPool.hpp>
#include <RingBuffer.hpp>
#include <SafeQueue.hpp>
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
#include <Matrix4x4.hpp>
#include <Matrix3x3.hpp>
#include <Light.hpp>
#include <RawBuffer.hpp>
#include <Texture.hpp>
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
#include <VertexBufferObject.hpp>
#include <VertexArrayObject.hpp>
#include <FrameBufferObject.hpp>
#include <Triangle.hpp>
#include <Clipper.hpp>
#include <InputManager.hpp>
#include <Transform.hpp>
#include <SegmentDrawer.hpp>
#include <Shader.hpp>
#include <ShadowShader.hpp>
#include <Material.hpp>
#include <FrameTile.hpp>
#include <GraphicsCommand.hpp>
#include <GraphicsDevice.hpp>
#include <Noise.hpp>
#include <Mesh.hpp>
#include <Model.hpp>
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
				Time::frame_start();
				Graphics().clear_buffer(BufferFlag::COLOR | BufferFlag::DEPTH | BufferFlag::STENCIL);
				InputMgr().update();
				scene.update();
				scene.render_shadow();
				Graphics().render();
				scene.render();
				Graphics().render();
				{
					std::stringstream ss;
					ss << "FPS: " << (int)Time::fps;
					Window().draw_text(2, 16, 300, 20, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "Triangles: " << Graphics().statistics.triangle_count;
					Window().draw_text(2, 32, 300, 20, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "Culled: " << Graphics().statistics.culled_triangle_count;
					Window().draw_text(2, 48, 300, 20, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "EarlyZ_Optimized_Pixels: " << Graphics().statistics.earlyz_optimized;
					Window().draw_text(2, 64, 300, 20, ss.str().c_str());
				}
				Window().flush();
				Time::frame_end();
				Sleep(1);
			}
		}
	};
}
#endif
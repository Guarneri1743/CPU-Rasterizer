#ifndef _APPLICATION_
#define _APPLICATION_

#include <vector>
#include <stdint.h>
#include "Marcos.h"
#include "Define.hpp"
#include "Singleton.hpp"
#include "IdAllocator.hpp"
#include "GlobalShaderParams.hpp"
#include "RingBuffer.hpp"
#include "SafeQueue.hpp"
#include "Time.hpp"
#include "Traits.hpp"
#include "Utility.hpp"

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Matrix3x3.hpp"
#include "Matrix4x4.hpp"

#include "BoundingBox.hpp"
#include "Capsule.hpp"
#include "Cylinder.hpp"
#include "Line.hpp"
#include "Plane.hpp"
#include "Ray.hpp"
#include "Rect.hpp"
#include "Segment.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"

#include "Color.hpp"
#include "Camera.hpp"
#include "CubeMap.hpp"
#include "InputManager.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Renderer.hpp"
#include "SkyboxRenderer.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Scene.hpp"

#include "Shader.hpp"
#include "ShadowShader.hpp"
#include "SkyboxShader.hpp"
#include "LightShader.hpp"
#include "PrimitiveFactory.hpp"

#include "Window.hpp"
#include "BaseEditor.hpp"
#include "GraphicsDevice.hpp"

namespace Guarneri
{
	class Application
	{
	public:
		static void play();
		static void load_scene(const char* path);

	private:
		static bool playing;
		static std::unique_ptr<Scene> scene;
		static std::vector<std::unique_ptr<BaseEditor>> editors;
		Application() = delete;
	};
}

#endif


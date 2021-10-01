#pragma once
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

#include "tinymath.h"
#include "Triangle.hpp"

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
		static void initialize();
		static void loop();
		static void stop();

	private:
		static bool playing;
		static std::unique_ptr<Scene> scene;
		static std::vector<std::unique_ptr<BaseEditor>> editors;
		Application() = delete;
	};
}


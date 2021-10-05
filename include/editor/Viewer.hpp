#pragma once
#include <vector>
#include <memory>

namespace CpuRasterizor
{
	class Scene;
	class BaseEditor;
	class Viewer
	{
	public:
		static void initialize();
		static void run();
		static void stop();

	private:
		static bool playing;
		static std::unique_ptr<Scene> scene;
		static std::vector<std::unique_ptr<BaseEditor>> editors;
		Viewer() = delete;
	};
}


#include "Viewer.hpp"
#include <iostream>
#include "InspectorEditor.hpp"
#include "HierarchyEditor.hpp"
#include "MainEditor.hpp"
#include "SceneViewEditor.hpp"
#include "ExplorerEditor.hpp"
#include "Window.hpp"
#include "GraphicsDevice.hpp"
#include "Time.hpp"
#include "Scene.hpp"

namespace CpuRasterizor
{
	bool Viewer::playing = false;
	std::vector<std::unique_ptr<BaseEditor>> Viewer::editors;

	void Viewer::initialize()
	{
		if (playing) return;
		Window::initialize_main_window("CPU Rasterizer Viewer");
		int w, h;
		w = (int)Window::main()->get_width();
		h = (int)Window::main()->get_height();
		auto main_editor = std::make_unique<MainEditor>(0, 0, w, h);
		auto sceneview = std::make_unique<SceneViewEditor>(kLeftWidth, kTopHeight, w - kRightWidth - kLeftWidth, h - kTopHeight - kBottomHeight);
		auto setting = std::make_unique<InspectorEditor>(0, kTopHeight, kLeftWidth, h - kTopHeight - kBottomHeight);
		auto hierarchy = std::make_unique<HierarchyEditor>(w - kRightWidth, kTopHeight, kRightWidth, h - kTopHeight - kBottomHeight);
		auto explorer = std::make_unique<ExplorerEditor>(0, h - kBottomHeight, w, kBottomHeight);
		editors.emplace_back(std::move(main_editor));
		editors.emplace_back(std::move(setting));
		editors.emplace_back(std::move(hierarchy));
		editors.emplace_back(std::move(sceneview));
		editors.emplace_back(std::move(explorer));
		CpuRasterDevice.set_viewport(600, 400);
		Time::start();
		CpuRasterizor::Scene::open_scene("/scenes/default_scene.scene");
		playing = true;
	}

	void Viewer::run()
	{
		if (!playing) return;


		while (Window::main()->is_open())
		{
			Time::frame_start();

			// main loop
			{
				// clear color buffer
				Window::main()->clear();

				// render scene
				if (Scene::current() != nullptr)
				{
					Scene::current()->update();
					Scene::current()->render();
				}

				// blit framebuffer to screen
				Window::main()->blit2screen(reinterpret_cast<uint8_t*>(CpuRasterDevice.get_target_color_buffer()),
											CpuRasterDevice.get_width(),
											CpuRasterDevice.get_height(),
											false);

				// render editors
				BaseEditor::pre_render();
				for (auto& editor : editors)
				{
					editor->render();
				}
				BaseEditor::post_render();

				// flush
				Window::main()->swap_buffer();
			}

			Time::frame_end();
		}
	}

	void Viewer::stop()
	{
		Window::main()->close();
	}
}
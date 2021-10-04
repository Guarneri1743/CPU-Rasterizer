#include "Application.hpp"
#include "InspectorEditor.hpp"
#include "HierarchyEditor.hpp"
#include "MainEditor.hpp"
#include "SceneViewEditor.hpp"
#include "ExplorerEditor.hpp"
#include <iostream>

namespace Guarneri
{
	bool Application::playing = false;
	std::vector<std::unique_ptr<BaseEditor>> Application::editors;

	void Application::initialize()
	{
		if (playing) return;
		Window::initialize_main_window("CPU-Rasterizer");
		auto main_editor = std::make_unique<MainEditor>(0, 0, Window::main()->get_width(), Window::main()->get_height());
		auto sceneview = std::make_unique<SceneViewEditor>(kLeftWidth, kTopHeight, Window::main()->get_width() - kRightWidth - kLeftWidth, Window::main()->get_height() - kTopHeight - kBottomHeight);
		auto setting = std::make_unique<InspectorEditor>(0, kTopHeight, kLeftWidth, Window::main()->get_height() - kTopHeight - kBottomHeight);
		auto hierarchy = std::make_unique<HierarchyEditor>(Window::main()->get_width() - kRightWidth, kTopHeight, kRightWidth, Window::main()->get_height() - kTopHeight - kBottomHeight);
		auto explorer = std::make_unique<ExplorerEditor>(0, Window::main()->get_height() - kBottomHeight, Window::main()->get_width(), kBottomHeight);
		editors.emplace_back(std::move(main_editor));
		editors.emplace_back(std::move(setting));
		editors.emplace_back(std::move(hierarchy));
		editors.emplace_back(std::move(sceneview));
		editors.emplace_back(std::move(explorer));
		INST(GraphicsDevice).initialize(600, 400);
		Time::start();
		playing = true;
	}

	void Application::loop()
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
				Window::main()->blit2screen(reinterpret_cast<uint8_t*>(INST(GraphicsDevice).get_target_color_buffer()),
											INST(GraphicsDevice).get_width(),
											INST(GraphicsDevice).get_height());

				// render editors
				BaseEditor::pre_render();
				for (auto& editor : editors)
				{
					editor->render();
				}
				BaseEditor::post_render();

				// flush
				Window::main()->flush();
			}

			Time::frame_end();
			Sleep(0);
		}
	}

	void Application::stop()
	{
		Window::main()->close();
	}
}
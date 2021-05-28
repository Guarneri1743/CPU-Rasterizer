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
		auto main_editor = std::make_unique<MainEditor>(0.0f, 0.0f, (float)Window::main()->get_width(), (float)Window::main()->get_height());
		auto sceneview = std::make_unique<SceneViewEditor>((float)kLeftWidth, (float)kTopHeight, (float)Window::main()->get_width() - (float)kRightWidth - (float)kLeftWidth, (float)Window::main()->get_height() - (float)kTopHeight - (float)kBottomHeight);
		auto setting = std::make_unique<InspectorEditor>(0.0f, (float)kTopHeight, (float)kLeftWidth, (float)Window::main()->get_height() - (float)kTopHeight - (float)kBottomHeight);
		auto hierarchy = std::make_unique<HierarchyEditor>((float)Window::main()->get_width() - (float)kRightWidth, (float)kTopHeight, (float)kRightWidth, (float)Window::main()->get_height() - (float)kTopHeight - (float)kBottomHeight);
		auto explorer = std::make_unique<ExplorerEditor>(0.0f, (float)Window::main()->get_height() - (float)kBottomHeight, (float)Window::main()->get_width(), (float)kBottomHeight);
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
				Window::main()->blit2screen(reinterpret_cast<uint8_t*>(INST(GraphicsDevice).get_framebuffer()),
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
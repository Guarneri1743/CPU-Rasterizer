#include "Application.hpp"
#include "SettingEditor.hpp"
#include "HierarchyEditor.hpp"
#include "MainEditor.hpp"
#include <iostream>

namespace Guarneri
{
	bool Application::playing = false;
	std::unique_ptr<Scene> Application::scene;
	std::vector<std::unique_ptr<BaseEditor>> Application::editors;

	void Application::play()
	{
		if (playing) return;

		Window::initialize_main_window("CPU-Rasterizer");
		editors.emplace_back(std::move(std::make_unique<MainEditor>()));
		editors.emplace_back(std::move(std::make_unique<SettingEditor>()));
		editors.emplace_back(std::move(std::make_unique<HierarchyEditor>()));
		INST(GraphicsDevice).initialize(600, 400);
		Time::start();

		playing = true;

		while (Window::main()->is_open())
		{
			Time::frame_start();

			// main loop
			{
				// clear color buffer
				Window::main()->clear();

				// render scene
				if (scene != nullptr)
				{
					scene->update();
					scene->render();
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
		Window::main()->close();
	}

	void Application::load_scene(const char* path)
	{
		INST(InputManager).clear_evts();
		scene = Guarneri::Scene::load_asset(path);
		Guarneri::Scene::set_current(scene.get());
		INST(GlobalShaderParams).enable_shadow = scene->enable_shadow;
		INST(GlobalShaderParams).pcf_on = scene->pcf_on;
		INST(GlobalShaderParams).shadow_bias = scene->shadow_bias;
		INST(GlobalShaderParams).color_space = scene->color_space;
		INST(GlobalShaderParams).workflow = scene->work_flow;
		std::cout << "open scene: " << scene->name << std::endl;
	}
}
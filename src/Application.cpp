#include "Application.hpp"
#include "SettingEditor.hpp"
#include "HierarchyEditor.hpp"
#include "MainEditor.hpp"
#include "SceneViewEditor.hpp"
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
		editors.emplace_back(std::move(std::make_unique<MainEditor>(0.0f, 0.0f, (float)Window::main()->get_width(), (float)Window::main()->get_height())));
		auto sceneview = std::make_unique<SceneViewEditor>((float)kLeftWidth, (float)kTopHeight, (float)Window::main()->get_width() - (float)kRightWidth - (float)kLeftWidth, (float)Window::main()->get_height() - (float)kTopHeight - (float)kBottomHeight);
		auto setting = std::make_unique<SettingEditor>(0.0f, (float)kTopHeight, (float)kLeftWidth, (float)Window::main()->get_height() - (float)kTopHeight - (float)kBottomHeight);
		auto hierarchy = std::make_unique<HierarchyEditor>((float)Window::main()->get_width() - (float)kRightWidth, (float)kTopHeight, (float)kRightWidth, (float)Window::main()->get_height() - (float)kTopHeight - (float)kBottomHeight);
		sceneview->add_left(hierarchy.get());
		//hierarchy->add_right(sceneview.get());
		sceneview->add_right(setting.get());
		//setting->add_left(sceneview.get());
		editors.emplace_back(std::move(setting));
		editors.emplace_back(std::move(hierarchy));
		editors.emplace_back(std::move(sceneview));
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
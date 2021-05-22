#include "Application.hpp"
#include <iostream>

namespace Guarneri
{
	Application::Application(const char* title)
	{
		Window::initialize_main_window(title);
		setting_editor = std::make_unique<SettingEditor>();
		INST(GlobalShaderParams).width = 400;
		INST(GlobalShaderParams).height = 300;
		INST(GraphicsDevice).initialize(INST(GlobalShaderParams).width, INST(GlobalShaderParams).height);
	}

	Application::~Application() {}

	void Application::kickoff(Scene& scene)
	{
		INST(GlobalShaderParams).enable_shadow = scene.enable_shadow;
		INST(GlobalShaderParams).pcf_on = scene.pcf_on;
		INST(GlobalShaderParams).shadow_bias = scene.shadow_bias;
		INST(GlobalShaderParams).color_space = scene.color_space;
		INST(GlobalShaderParams).workflow = scene.work_flow;

		Time::start();
		INST(InputManager).add_on_key_down_evt([](KeyCode code, void* data)
		{
			auto win = reinterpret_cast<Window*>(data);
			if (code == KeyCode::ESC) win->close();
		}, Window::main()->get());

		std::cout << "kickoff scene: " << scene.name << std::endl;

		while (Window::main()->is_open())
		{
			Time::frame_start();
			// main loop
			{
				// clear color buffer
				Window::main()->clear();
				// render scene
				scene.update();
				scene.render();
				// blit framebuffer to screen
				Window::main()->blit2screen(reinterpret_cast<uint8_t*>(INST(GraphicsDevice).get_framebuffer()), INST(GlobalShaderParams).width, INST(GlobalShaderParams).height);
				// render editor gui
				setting_editor->render();
				// flush
				Window::main()->flush();
			}
			Time::frame_end();
			Sleep(1);
		}

		Window::main()->close();
	}
}
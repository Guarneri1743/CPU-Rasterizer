#include "Application.hpp"
#include <iostream>

namespace Guarneri
{
	Application::Application(const char* title, int width, int height)
	{
		window = std::make_unique<Window>(title, width, height);
		editor = std::make_unique<Editor>(window->get(), kGlslVersion);
		INST(GlobalShaderParams).width = width;
		INST(GlobalShaderParams).height = height;
		INST(GraphicsDevice).initialize(width, height);
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
		}, window.get());

		std::cout << "kickoff scene: " << scene.name << std::endl;

		while (window->is_open())
		{
			Time::frame_start();
			double x, y;
			window->get_cursor_pos(x, y);
			INST(InputManager).update(x, y);

			window->clear();
			scene.update();
			scene.render();
			window->blit2screen(reinterpret_cast<uint8_t*>(INST(GraphicsDevice).get_framebuffer()));
			editor->render();
			editor->present();
			window->flush();

			Time::frame_end();
			Sleep(1);
		}

		window->close();
	}
}
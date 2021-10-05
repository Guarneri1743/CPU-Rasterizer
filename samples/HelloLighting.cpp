#include "Window.hpp"
#include "GraphicsDevice.hpp"
#include "SampleShader.shader"

using namespace CpuRasterizor;

int main()
{
	Window::initialize_main_window("Demo", 600, 400);
	INST(GraphicsDevice).set_viewport(Window::main()->get_width(), Window::main()->get_height());
	Window::main()->add_on_resize_evt([](int w, int h, void* ud)
	{
		INST(GraphicsDevice).set_viewport(w, h);
	}, nullptr);

	SampleShader* default_shader = new SampleShader();

	Vertex v1(tinymath::vec4f(-0.5f, -0.5f, 0.0f, 1.0f), tinymath::kVec3fZero, tinymath::kVec2fZero);
	Vertex v2(tinymath::vec4f(0.5f, -0.5f, 0.0f, 1.0f), tinymath::kVec3fZero, tinymath::kVec2fZero);
	Vertex v3(tinymath::vec4f(0.0f, 0.5f, 0.0f, 1.0f), tinymath::kVec3fZero, tinymath::kVec2fZero);

	default_shader->local_properties.set_mat4x4(mat_model, tinymath::kMat4x4Identity);
	default_shader->local_properties.set_mat4x4(mat_view, tinymath::kMat4x4Identity);
	default_shader->local_properties.set_mat4x4(mat_projection, tinymath::kMat4x4Identity);
	default_shader->local_properties.set_mat4x4(mat_vp, tinymath::kMat4x4Identity);
	default_shader->local_properties.set_mat4x4(mat_mvp, tinymath::kMat4x4Identity);

	default_shader->double_face = true;
	default_shader->ztest_func = CompareFunc::kAlways;
	default_shader->transparent = false;

	INST(GraphicsDevice).set_clear_color(tinymath::kColorBlue);

	while (Window::main()->is_open())
	{
		Window::main()->clear();

		INST(GraphicsDevice).clear_buffer(FrameContent::kColor | FrameContent::kDepth | FrameContent::kStencil);
		INST(GraphicsDevice).submit_draw_command(default_shader, v1, v2, v3);
		INST(GraphicsDevice).fence_draw_commands();
		INST(GraphicsDevice).present();

		auto w = INST(GraphicsDevice).get_width();
		auto h = INST(GraphicsDevice).get_height();

		Window::main()->blit2screen(reinterpret_cast<uint8_t*>(INST(GraphicsDevice).get_target_color_buffer()), w, h, true);

		Window::main()->flush();

		Sleep(0);
	}

	delete default_shader;

	Window::main()->close();

	return 0;
}

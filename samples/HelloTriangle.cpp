#include "Window.hpp"
#include "GraphicsDevice.hpp"
#include "HelloTriangleShader.hpp"

using namespace CpuRasterizor;

int main()
{
	// initialize window
	Window::initialize_main_window("Demo", 600, 400);

	// set viewport
	INST(GraphicsDevice).set_viewport(Window::main()->get_width(), Window::main()->get_height());

	// resize callback
	Window::main()->add_on_resize_evt([](int w, int h, void* ud)
	{
		INST(GraphicsDevice).set_viewport(w, h);
	}, nullptr);

	HelloTriangleShader shader;

	// a triangle 
	Vertex v1(tinymath::vec4f(-0.5f, -0.5f, 0.0f, 1.0f), tinymath::kVec3fZero, tinymath::kVec2fZero);
	Vertex v2(tinymath::vec4f(0.5f, -0.5f, 0.0f, 1.0f), tinymath::kVec3fZero, tinymath::kVec2fZero);
	Vertex v3(tinymath::vec4f(0.0f, 0.5f, 0.0f, 1.0f), tinymath::kVec3fZero, tinymath::kVec2fZero);

	// setup shader properties
	shader.local_properties.set_mat4x4(mat_model, tinymath::kMat4x4Identity);
	shader.local_properties.set_mat4x4(mat_view, tinymath::kMat4x4Identity);
	shader.local_properties.set_mat4x4(mat_projection, tinymath::kMat4x4Identity);
	shader.local_properties.set_mat4x4(mat_vp, tinymath::kMat4x4Identity);
	shader.local_properties.set_mat4x4(mat_mvp, tinymath::kMat4x4Identity);

	shader.double_face = true;
	shader.ztest_func = CompareFunc::kAlways;
	shader.transparent = false;

	// set background color
	INST(GraphicsDevice).set_clear_color(tinymath::kColorBlue);

	while (Window::main()->is_open())
	{
		// clear window buffer
		Window::main()->clear();

		// clear buffer
		INST(GraphicsDevice).clear_buffer(FrameContent::kColor | FrameContent::kDepth | FrameContent::kStencil);

		// submit a triangle to draw
		INST(GraphicsDevice).submit_draw_command(&shader, v1, v2, v3);

		// sync draw commands submitted
		INST(GraphicsDevice).fence_draw_commands();

		// render all content to render texture
		INST(GraphicsDevice).present();

		auto w = INST(GraphicsDevice).get_width();
		auto h = INST(GraphicsDevice).get_height();

		// blit render texture to screen
		Window::main()->blit2screen(reinterpret_cast<uint8_t*>(INST(GraphicsDevice).get_target_color_buffer()), w, h, true);

		// swap front/back buffer
		Window::main()->flush();

		Sleep(0);
	}

	Window::main()->close();

	return 0;
}

#include "GraphicsApi.h"
#include "HelloTextureShader.hpp"

using namespace CpuRasterizor;

int main()
{
	// initialize window
	crWindowInit("Demo", 600, 400);

	// set viewport
	crApi.set_viewport(Window::main()->get_width(), Window::main()->get_height());

	// resize callback
	crMainWindow.add_on_resize_evt([](int w, int h, void* ud)
	{
		crApi.set_viewport(w, h);
	}, nullptr);

	HelloTextureShader shader;

	// a triangle 
	crVert v1(crVec4(-0.5f, -0.5f, 0.0f, 1.0f), crVec3Zero, crVec2(0.0f, 0.0f));
	crVert v2(crVec4(0.5f, -0.5f, 0.0f, 1.0f), crVec3Zero, crVec2(1.0f, 0.0f));
	crVert v3(crVec4(0.0f, 0.5f, 0.0f, 1.0f), crVec3Zero, crVec2(0.5f, 1.0f));

	// setup shader properties
	shader.local_properties.set_mat4x4(mat_model, crMat4Identity);
	shader.local_properties.set_mat4x4(mat_view, crMat4Identity);
	shader.local_properties.set_mat4x4(mat_projection, crMat4Identity);
	shader.local_properties.set_mat4x4(mat_vp, crMat4Identity);
	shader.local_properties.set_mat4x4(mat_mvp, crMat4Identity);

	shader.double_face = true;
	shader.ztest_func = CompareFunc::kAlways;
	shader.transparent = false;

	auto tex = std::make_shared<Texture>(128, 128, TextureFormat::kRGB);

	// generate a checkerboard texture
	for (int r = 0; r< tex->width; r++)
	{
		for (int c = 0; c < tex->height; c++)
		{
			auto val = (((r & 0x8) == 0) ^ ((c & 0x8) == 0));
			tex->write((size_t)r, (size_t)c, { (float)val, (float)val, (float)val, 1.0f });
		}
	}

	property_name prop_id = 123;
	shader.local_properties.set_texture(prop_id, tex);

	// set background color
	crApi.set_clear_color(tinymath::kColorBlue);

	while (crMainWindow.is_open())
	{
		// clear window buffer
		crMainWindow.clear();

		// clear buffer
		crApi.clear_buffer(FrameContent::kColor | FrameContent::kDepth | FrameContent::kStencil);

		// submit a triangle to draw
		crApi.submit_draw_command(&shader, v1, v2, v3); 

		// sync draw commands submitted
		crApi.fence_draw_commands(); 

		// render all content to render texture
		crApi.present(); 

		// blit render texture to screen
		crMainWindow.blit2screen(reinterpret_cast<uint8_t*>(crApi.get_target_color_buffer()), crApi.get_width(), crApi.get_height(), true);

		// swap front/back buffer
		crMainWindow.flush();

		Sleep(0);
	}

	crMainWindow.close();

	return 0;
}

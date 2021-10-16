#include "CGL.h"
#include "HelloTextureShader.hpp"

int main()
{
	// initialize window
	cglInitWindow("Demo", 600, 400);

	// set viewport
	size_t w, h;
	cglGetMainWindowSize(w, h);
	cglSetViewPort(w, h);

	// resize callback
	cglAddResizeEvent([](size_t w, size_t h, void* ud)
	{
		cglSetViewPort(w, h);
	}, nullptr);

	HelloTextureShader shader;

	// a triangle 
	cglVert v1(cglVec4(-0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2(0.0f, 0.0f));
	cglVert v2(cglVec4(0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2(1.0f, 0.0f));
	cglVert v3(cglVec4(0.0f, 0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2(0.5f, 1.0f));

	// setup shader properties
	shader.local_properties.set_mat4x4(mat_model, cglMat4Identity);
	shader.local_properties.set_mat4x4(mat_view, cglMat4Identity);
	shader.local_properties.set_mat4x4(mat_projection, cglMat4Identity);
	shader.local_properties.set_mat4x4(mat_vp, cglMat4Identity);
	shader.local_properties.set_mat4x4(mat_mvp, cglMat4Identity);

	shader.double_face = true;
	shader.ztest_func = cglCompareFunc::kAlways;
	shader.transparent = false;

	auto tex = std::make_shared<Texture>(128, 128, cglTextureFormat::kRGB);

	// generate a checkerboard texture
	for (int r = 0; r< tex->width; r++)
	{
		for (int c = 0; c < tex->height; c++)
		{
			auto val = ((r & 0x8) == 0) ^ ((c & 0x8) == 0);
			tex->write((size_t)r, (size_t)c, { (float)val, (float)val, (float)val, 1.0f });
		}
	}

	property_name prop_id = 123;
	shader.local_properties.set_texture(prop_id, tex);

	// set background color
	cglSetClearColor(tinymath::kColorBlue);

	while (cglIsMainWindowOpen())
	{
		// clear window buffer
		cglClearMainWindow();

		// clear buffer
		cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);

		// submit primitive
		cglSubmitPrimitive(&shader, v1, v2, v3); 

		// fence primitive tasks
		cglFencePrimitives(); 

		// fence pixel tasks
		cglFencePixels(); 

		cglSwapBuffer(true);
	}

	cglCloseMainWindow();

	return 0;
}

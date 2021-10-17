#include "CGL.h"
#include "HelloTriangleShader.hpp"

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
		UNUSED(ud);
		cglSetViewPort(w, h);
	}, nullptr);

	HelloTriangleShader shader;

	// a triangle 
	cglVert v1(cglVec4(-0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2Zero);
	cglVert v2(cglVec4(0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2Zero);
	cglVert v3(cglVec4(0.0f, 0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2Zero);

	// setup shader properties
	shader.local_properties.set_mat4x4(mat_model, cglMat4Identity);
	shader.local_properties.set_mat4x4(mat_view, cglMat4Identity);
	shader.local_properties.set_mat4x4(mat_projection, cglMat4Identity);
	shader.local_properties.set_mat4x4(mat_vp, cglMat4Identity);
	shader.local_properties.set_mat4x4(mat_mvp, cglMat4Identity);

	shader.double_face = true;
	shader.ztest_func = cglCompareFunc::kAlways;
	shader.transparent = false;

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

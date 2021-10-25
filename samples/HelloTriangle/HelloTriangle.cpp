#include "CGL.h"
#include "HelloTriangleShader.hpp"

int main()
{
	size_t w = 600;
	size_t h = 400;
	// initialize window
	cglInitWindow("Demo", w, h);

	// set viewport
	cglSetViewPort(0, 0, w, h);

	// resize callback
	cglAddResizeEvent([](size_t resized_w, size_t resized_h, void* ud)
	{
		UNUSED(ud);
		cglSetViewPort(0, 0, resized_w, resized_h);
	}, nullptr);

	HelloTriangleShader shader;
	uint32_t shader_id;
	cglCreateProgram(&shader, shader_id);

	// a triangle 
	cglVert v1(cglVec4(-0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2Zero);
	cglVert v2(cglVec4(0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2Zero);
	cglVert v3(cglVec4(0.0f, 0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2Zero);

	// setup shader properties
	cglUniformMatrix4fv(shader_id, mat_model_prop, cglMat4Identity);
	cglUniformMatrix4fv(shader_id, mat_view_prop, cglMat4Identity);
	cglUniformMatrix4fv(shader_id, mat_projection_prop, cglMat4Identity);
	cglUniformMatrix4fv(shader_id, mat_vp_prop, cglMat4Identity);
	cglUniformMatrix4fv(shader_id, mat_mvp_prop, cglMat4Identity);

	cglDisable(cglPipelineFeature::kBlending);
	cglDisable(cglPipelineFeature::kFaceCulling);
	cglDepthFunc(cglCompareFunc::kAlways);

	// set background color
	cglSetClearColor(tinymath::kColorBlue);

	while (cglIsMainWindowOpen())
	{
		// clear window buffer
		cglClearMainWindow();

		// clear buffer
		cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);

		cglUseProgram(shader_id);

		// submit primitive
		cglSubmitPrimitive(v1, v2, v3);

		// fence primitive tasks
		cglFencePrimitives();

		// fence pixel tasks
		cglFencePixels();

		cglSwapBuffer(true);
	}

	cglCloseMainWindow();

	return 0;
}

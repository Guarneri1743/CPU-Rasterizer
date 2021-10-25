#include "CGL.h"
#include "HelloTextureShader.hpp"

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

	HelloTextureShader shader;
	uint32_t shader_id;
	cglCreateProgram(&shader, shader_id);

	// a triangle 
	cglVert v1(cglVec4(-0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2(0.0f, 0.0f));
	cglVert v2(cglVec4(0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2(1.0f, 0.0f));
	cglVert v3(cglVec4(0.0f, 0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2(0.5f, 1.0f));

	// setup shader properties
	cglUniformMatrix4fv(shader_id, mat_model_prop, cglMat4Identity);
	cglUniformMatrix4fv(shader_id, mat_view_prop, cglMat4Identity);
	cglUniformMatrix4fv(shader_id, mat_projection_prop, cglMat4Identity);
	cglUniformMatrix4fv(shader_id, mat_vp_prop, cglMat4Identity);
	cglUniformMatrix4fv(shader_id, mat_mvp_prop, cglMat4Identity);

	cglDisable(cglPipelineFeature::kBlending);
	cglDisable(cglPipelineFeature::kFaceCulling);
	cglDepthFunc(cglCompareFunc::kAlways);

	cglColorRgb* tex_buf = new cglColorRgb[64 * 64 * sizeof(cglColorRgb)];

	// generate a checkerboard texture
	for (size_t r = 0; r < 64; ++r)
	{
		for (size_t c = 0; c < 64; ++c)
		{
			auto val = ((r & 0x8) == 0) ^ ((c & 0x8) == 0);
			tex_buf[r * 64 + c] = cglEncodeRgb((float)val, (float)val, (float)val);
		}
	}

	uint32_t tex_id;
	cglGenTexture(tex_id);
	cglTexImage2D(tex_id, 64, 64, cglTextureFormat::kRGB, tex_buf);

	// set background color
	cglSetClearColor(tinymath::kColorBlue);

	while (cglIsMainWindowOpen())
	{
		// clear window buffer
		cglClearMainWindow();

		// clear buffer
		cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);

		cglUseProgram(shader_id);

		cglActivateTexture(tex_id);

		// todo: strinify the key
		property_name tex_prop = 123;
		cglUniform1i(shader_id, tex_prop, tex_id);

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

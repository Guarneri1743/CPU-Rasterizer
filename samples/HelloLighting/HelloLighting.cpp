#include "CGL.h"
#include "HelloLightingShader.hpp"

cglVert cube_vertices[36] = {
		cglVert(cglVec4(-0.5f, -0.5f, -0.5f, 1.0f), cglVec3(0.0f,  0.0f, -1.0f), cglVec2(0.0f,  0.0f)),
		cglVert(cglVec4(0.5f, -0.5f, -0.5f, 1.0f), cglVec3(0.0f,  0.0f, -1.0f), cglVec2(1.0f,  0.0f)),
		cglVert(cglVec4(0.5f,  0.5f, -0.5f, 1.0f), cglVec3(0.0f,  0.0f, -1.0f), cglVec2(1.0f,  1.0f)),
		cglVert(cglVec4(0.5f,  0.5f, -0.5f, 1.0f), cglVec3(0.0f,  0.0f, -1.0f), cglVec2(1.0f,  1.0f)),
		cglVert(cglVec4(-0.5f,  0.5f, -0.5f, 1.0f), cglVec3(0.0f,  0.0f, -1.0f), cglVec2(0.0f,  1.0f)),
		cglVert(cglVec4(-0.5f, -0.5f, -0.5f, 1.0f), cglVec3(0.0f,  0.0f, -1.0f), cglVec2(0.0f,  0.0f)),

		cglVert(cglVec4(-0.5f, -0.5f,  0.5f, 1.0f), cglVec3(0.0f,  0.0f, 1.0f), cglVec2(0.0f,  0.0f)),
		cglVert(cglVec4(0.5f, -0.5f,  0.5f, 1.0f), cglVec3(0.0f,  0.0f, 1.0f), cglVec2(1.0f,  0.0f)),
		cglVert(cglVec4(0.5f,  0.5f,  0.5f, 1.0f), cglVec3(0.0f,  0.0f, 1.0f), cglVec2(1.0f,  1.0f)),
		cglVert(cglVec4(0.5f,  0.5f,  0.5f, 1.0f), cglVec3(0.0f,  0.0f, 1.0f), cglVec2(1.0f, 1.0f)),
		cglVert(cglVec4(-0.5f,  0.5f,  0.5f, 1.0f), cglVec3(0.0f,  0.0f, 1.0f), cglVec2(0.0f, 1.0f)),
		cglVert(cglVec4(-0.5f, -0.5f,  0.5f, 1.0f), cglVec3(0.0f,  0.0f, 1.0f), cglVec2(0.0f,  0.0f)),

		cglVert(cglVec4(-0.5f,  0.5f,  0.5f, 1.0f), cglVec3(-1.0f,  0.0f,  0.0f), cglVec2(1.0f,  0.0f)),
		cglVert(cglVec4(-0.5f,  0.5f, -0.5f, 1.0f), cglVec3(-1.0f,  0.0f,  0.0f), cglVec2(1.0f,  1.0f)),
		cglVert(cglVec4(-0.5f, -0.5f, -0.5f, 1.0f), cglVec3(-1.0f,  0.0f,  0.0f), cglVec2(0.0f,  1.0f)),
		cglVert(cglVec4(-0.5f, -0.5f, -0.5f, 1.0f), cglVec3(-1.0f,  0.0f,  0.0f), cglVec2(0.0f,  1.0f)),
		cglVert(cglVec4(-0.5f, -0.5f,  0.5f, 1.0f), cglVec3(-1.0f,  0.0f,  0.0f), cglVec2(0.0f,  0.0f)),
		cglVert(cglVec4(-0.5f,  0.5f,  0.5f, 1.0f), cglVec3(-1.0f,  0.0f,  0.0f), cglVec2(1.0f,  0.0f)),

		cglVert(cglVec4(0.5f,  0.5f,  0.5f, 1.0f), cglVec3(1.0f,  0.0f,  0.0f), cglVec2(1.0f,  0.0f)),
		cglVert(cglVec4(0.5f,  0.5f, -0.5f, 1.0f), cglVec3(1.0f,  0.0f,  0.0f), cglVec2(1.0f,  1.0f)),
		cglVert(cglVec4(0.5f, -0.5f, -0.5f, 1.0f), cglVec3(1.0f,  0.0f,  0.0f), cglVec2(0.0f,  1.0f)),
		cglVert(cglVec4(0.5f, -0.5f, -0.5f, 1.0f), cglVec3(1.0f,  0.0f,  0.0f), cglVec2(0.0f,  1.0f)),
		cglVert(cglVec4(0.5f, -0.5f,  0.5f, 1.0f), cglVec3(1.0f,  0.0f,  0.0f), cglVec2(0.0f,  0.0f)),
		cglVert(cglVec4(0.5f,  0.5f,  0.5f, 1.0f), cglVec3(1.0f,  0.0f,  0.0f), cglVec2(1.0f,  0.0f)),

		cglVert(cglVec4(-0.5f, -0.5f, -0.5f, 1.0f), cglVec3(0.0f, -1.0f,  0.0f), cglVec2(0.0f,  1.0f)),
		cglVert(cglVec4(0.5f, -0.5f, -0.5f, 1.0f), cglVec3(0.0f, -1.0f,  0.0f), cglVec2(1.0f,  1.0f)),
		cglVert(cglVec4(0.5f, -0.5f,  0.5f, 1.0f), cglVec3(0.0f, -1.0f,  0.0f), cglVec2(1.0f,  0.0f)),
		cglVert(cglVec4(0.5f, -0.5f,  0.5f, 1.0f), cglVec3(0.0f, -1.0f,  0.0f), cglVec2(1.0f,  0.0f)),
		cglVert(cglVec4(-0.5f, -0.5f,  0.5f, 1.0f), cglVec3(0.0f, -1.0f,  0.0f), cglVec2(0.0f,  0.0f)),
		cglVert(cglVec4(-0.5f, -0.5f, -0.5f, 1.0f), cglVec3(0.0f, -1.0f,  0.0f), cglVec2(0.0f,  1.0f)),

		cglVert(cglVec4(-0.5f,  0.5f, -0.5f, 1.0f), cglVec3(0.0f, 1.0f,  0.0f), cglVec2(0.0f,  1.0f)),
		cglVert(cglVec4(0.5f,  0.5f, -0.5f, 1.0f), cglVec3(0.0f, 1.0f,  0.0f), cglVec2(1.0f,  1.0f)),
		cglVert(cglVec4(0.5f,  0.5f,  0.5f, 1.0f), cglVec3(0.0f, 1.0f,  0.0f), cglVec2(1.0f,  0.0f)),
		cglVert(cglVec4(0.5f, 0.5f,  0.5f, 1.0f), cglVec3(0.0f, 1.0f,  0.0f), cglVec2(1.0f,  0.0f)),
		cglVert(cglVec4(-0.5f,  0.5f,  0.5f, 1.0f), cglVec3(0.0f, 1.0f,  0.0f), cglVec2(0.0f,  0.0f)),
		cglVert(cglVec4(-0.5f,  0.5f, -0.5f, 1.0f), cglVec3(0.0f, 1.0f,  0.0f), cglVec2(0.0f,  1.0f)),
};

HelloLightingShader shader;

void draw_cube()
{
	for (int i = 0; i < 36; i+= 3)
	{
		// submit primitive
		cglSubmitPrimitive(&shader, cube_vertices[i], cube_vertices[i + 1], cube_vertices[i + 2]);
	}

	// fence primitive tasks
	cglFencePrimitives();
}

int main()
{
	size_t w = 600;
	size_t h = 400;
	// initialize window
	cglInitWindow("Demo", w, h);

	// set viewport
	cglSetViewPort(w, h);

	// resize callback
	cglAddResizeEvent([](size_t w, size_t h, void* ud)
	{
		cglSetViewPort(w, h);
	}, nullptr);

	// setup shader properties
	cglVec3 cam_pos = cglVec3(2.0f, 2.0f, 2.0f);
	cglVec3 cube_pos = cglVec3Zero;
	float near = 0.05f;
	float far = 100.0f;

	float angle = 0.0f;

	cglMat4 model = cglTranslation(cube_pos) * cglRotation(cglVec3(0.0f, 1.0f, 0.0f), angle) * cglScale(cglVec3One);
	cglMat4 view = cglLookat(cam_pos, cube_pos, cglVec3Up);
	cglMat4 proj = cglPerspective(60.0f, (float)w / (float)h, near, far);

	shader.local_properties.set_mat4x4(mat_model, model);
	shader.local_properties.set_mat4x4(mat_view, view);
	shader.local_properties.set_mat4x4(mat_projection, proj);
	shader.local_properties.set_mat4x4(mat_vp, proj * view);
	shader.local_properties.set_mat4x4(mat_mvp, proj * view * model);

	shader.double_face = true;
	shader.ztest_func = cglCompareFunc::kLess;

	shader.local_properties.set_float4(albedo_prop, cglVec4(0.5f, 0.0f, 0.0f, 1.0f));
	shader.local_properties.set_float4(light_direction, cglVec4(0.0f, 1.0f, 1.5f, 1.0f));
	shader.local_properties.set_float4(light_diffuse, cglVec4(1.0f, 1.0f, 1.0f, 1.0f)); 
	shader.local_properties.set_float(light_intensity, 1.0f);

	// set background color
	cglSetClearColor(tinymath::kColorBlue);

	while (cglIsMainWindowOpen())
	{
		// clear window buffer
		cglClearMainWindow();

		// clear buffer
		cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);

		//angle += 1.0f;
		model = cglTranslation(cube_pos) * cglRotation(cglVec3(1.0f, 1.0f, 1.0f), angle) * cglScale(cglVec3One);
		shader.local_properties.set_mat4x4(mat_model, model);
		draw_cube();

		// fence pixel tasks
		cglFencePixels();

		cglSwapBuffer(true);
	}

	cglCloseMainWindow();

	return 0;
}
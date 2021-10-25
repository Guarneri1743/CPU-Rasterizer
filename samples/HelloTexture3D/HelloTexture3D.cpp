#include "CGL.h"
#include "HelloTexture3DShader.hpp"

std::vector<cglVert> cube_vertices = {
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

	// create a 3d texture
	auto tex = std::make_shared<Texture>(64, 64, 64, cglTextureFormat::kRGB);

	for (size_t r = 0; r < 64; ++r)
	{
		for (size_t c = 0; c < 64; ++c)
		{
			for (size_t l = 0; l < 64; ++l)
			{
				tex->write(r, c, l, { (float)r / (float)64, (float)c / (float)64, (float)l / (float)64 , 1.0f});
			}
		}
	}

	HelloTexture3DShader shader;
	uint32_t shader_id;
	cglCreateProgram(&shader, shader_id);

	std::vector<size_t> index_buffer = 
	{ 
		0, 1, 2, 3, 4, 5, 6, 7, 8, 
		9, 10, 11, 12, 13, 14, 15, 16, 17, 
		18, 19, 20, 21, 22, 23, 24, 25, 26, 
		27, 28, 29, 30, 31, 32, 33, 34, 35
	};

	for (int i = 0; i < cube_vertices.size(); i++)
	{
		cube_vertices[i].mask |= kNormalMask;
	}

	auto vid = cglBindVertexBuffer(cube_vertices);
	auto iid = cglBindIndexBuffer(index_buffer);

	// todo: strinify the key
	shader.local_properties.set_texture(123, tex);

	// setup shader properties
	cglVec3 cam_pos = cglVec3(1.5f, 1.5f, 1.5f);
	cglVec3 cube_pos = cglVec3Zero;
	float angle = 0.0f;

	float near = 0.05f;
	float far = 100.0f;

	cglMat4 model;
	cglMat4 view;
	cglMat4 proj;

	model = cglTranslation(cube_pos) * cglRotation(cglVec3Up, angle);
	view = cglLookat(cam_pos, cube_pos, cglVec3Up);
	proj = cglPerspective(60.0f, (float)w / (float)h, near, far);

	cglUniformMatrix4fv(shader_id, mat_model_prop, model);
	cglUniformMatrix4fv(shader_id, mat_view_prop, view);
	cglUniformMatrix4fv(shader_id, mat_projection_prop, proj);
	cglUniformMatrix4fv(shader_id, mat_vp_prop, proj * view);
	cglUniformMatrix4fv(shader_id, mat_mvp_prop, proj * view * model);

	cglEnable(cglPipelineFeature::kDepthTest);
	cglDepthFunc(cglCompareFunc::kLess);

	cglDisable(cglPipelineFeature::kFaceCulling);
	cglCullFace(cglFaceCulling::None);

	// set background color
	cglSetClearColor(tinymath::kColorBlue);

	while (cglIsMainWindowOpen())
	{
		// clear window buffer
		cglClearMainWindow();

		// clear buffer
		cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);

		angle += 2.0f;
		model = cglTranslation(cube_pos) * cglRotation(cglVec3Up, angle);

		cglUniformMatrix4fv(shader_id, mat_model_prop, model);
		cglUniformMatrix4fv(shader_id, mat_view_prop, view);
		cglUniformMatrix4fv(shader_id, mat_projection_prop, proj);
		cglUniformMatrix4fv(shader_id, mat_vp_prop, proj * view);
		cglUniformMatrix4fv(shader_id, mat_mvp_prop, proj * view * model);

		// submit primitive
		cglUseProgram(shader_id);
		cglUseVertexBuffer(vid);
		cglUseIndexBuffer(iid);

		// draw primitive
		cglDrawPrimitive();

		// fence primitive tasks
		cglFencePrimitives();

		// fence pixel tasks
		cglFencePixels(); 

		cglSwapBuffer(true);
	}

	cglCloseMainWindow();

	return 0;
}

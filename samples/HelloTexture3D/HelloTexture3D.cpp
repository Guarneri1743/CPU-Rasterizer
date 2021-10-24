#include "CGL.h"
#include "HelloTexture3DShader.hpp"

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

HelloTexture3DShader shader_;
cglMat4 model_;
cglMat4 view_;
cglMat4 proj_;

void draw_cube()
{
	for (int i = 0; i < 36; i += 3)
	{
		cube_vertices[i].mask |= kNormalMask;
		cube_vertices[i + 1].mask |= kNormalMask;
		cube_vertices[i + 2].mask |= kNormalMask;
		 
		// submit primitive
		cglSubmitPrimitive(&shader_, cube_vertices[i], cube_vertices[i + 1], cube_vertices[i + 2]);
	}
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
	cglAddResizeEvent([](size_t resized_w, size_t resized_h, void* ud)
	{
		UNUSED(ud);
		cglSetViewPort(resized_w, resized_h);
	}, nullptr);

	// setup shader_ properties
	cglVec3 cam_pos = cglVec3(1.5f, 1.5f, 1.5f);
	cglVec3 cube_pos = cglVec3Zero;
	float angle = 0.0f;

	float near = 0.05f;
	float far = 100.0f;

	model_ = cglTranslation(cube_pos) * cglRotation(cglVec3Up, angle);
	view_ = cglLookat(cam_pos, cube_pos, cglVec3Up);
	proj_ = cglPerspective(60.0f, (float)w / (float)h, near, far);

	shader_.local_properties.set_mat4x4(mat_model_prop, model_);
	shader_.local_properties.set_mat4x4(mat_view_prop, view_);
	shader_.local_properties.set_mat4x4(mat_projection_prop, proj_);
	shader_.local_properties.set_mat4x4(mat_vp_prop, proj_ * view_);
	shader_.local_properties.set_mat4x4(mat_mvp_prop, proj_ * view_ * model_);

	shader_.ztest_func = cglCompareFunc::kLess;
	shader_.double_face = true;

	// create a 3d texture
	auto tex = std::make_shared<Texture>(128, 128, 128, cglTextureFormat::kRGB);
	tex->filtering = cglFiltering::kPoint;
	tex->wrap_mode = cglWrapMode::kRepeat;

	for (size_t r = 0; r < tex->height; ++r)
	{
		for (size_t c = 0; c < tex->width; ++c)
		{
			for (size_t l = 0; l < tex->layer_count; ++l)
			{
				tex->write(r, c, l, { (float)r / (float)tex->width, (float)c / (float)tex->height, (float)l / (float)tex->layer_count, 1.0f });
			}
		}
	}

	property_name prop_id = 123;
	shader_.local_properties.set_texture(prop_id, tex);

	// set background color
	cglSetClearColor(tinymath::kColorBlue);

	while (cglIsMainWindowOpen())
	{
		// clear window buffer
		cglClearMainWindow();

		// clear buffer
		cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);

		angle += 2.0f;
		model_ = cglTranslation(cube_pos) * cglRotation(cglVec3Up, angle);
		shader_.local_properties.set_mat4x4(mat_model_prop, model_);
		shader_.local_properties.set_mat4x4(mat_view_prop, view_);
		shader_.local_properties.set_mat4x4(mat_projection_prop, proj_);
		shader_.local_properties.set_mat4x4(mat_vp_prop, proj_ * view_);
		shader_.local_properties.set_mat4x4(mat_mvp_prop, proj_ * view_ * model_);
		// submit primitive
		draw_cube();

		// fence primitive tasks
		cglFencePrimitives();

		// fence pixel tasks
		cglFencePixels(); 

		cglSwapBuffer(true);
	}

	cglCloseMainWindow();

	return 0;
}

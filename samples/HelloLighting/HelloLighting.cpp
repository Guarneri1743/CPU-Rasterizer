#include "CGL.h"
#include "HelloLightingShader.hpp"

HelloLightingShader shader;

cglVec3 spherical_coord(float theta, float phi)
{
	cglVec3 ret;
	ret.x = cglCos(theta) * cglCos(phi);
	ret.y = cglSin(theta);
	ret.z = cglCos(theta) * cglSin(phi);
	return cglNormalize(ret);
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

	// setup shader properties
	cglVec3 cam_pos = cglVec3(1.5f, 1.5f, 1.5f);
	cglVec3 cube_pos = cglVec3Zero;
	cglVec3 light_direction;	

	float l_theta = 0.0f;
	float l_phi = 0.0f;

	light_direction.x = 0.0f;
	light_direction.y = 0.5f;
	light_direction.z = 1.0f;

	float near = 0.05f;
	float far = 100.0f;

	cglMat4 model = cglTranslation(cube_pos);
	cglMat4 view = cglLookat(cam_pos, cube_pos, cglVec3Up);
	cglMat4 proj = cglPerspective(60.0f, (float)w / (float)h, near, far);

	shader.local_properties.set_mat4x4(mat_model_prop, model);
	shader.local_properties.set_mat4x4(mat_view_prop, view);
	shader.local_properties.set_mat4x4(mat_projection_prop, proj);
	shader.local_properties.set_mat4x4(mat_vp_prop, proj * view);
	shader.local_properties.set_mat4x4(mat_mvp_prop, proj * view * model);

	shader.local_properties.set_float4(cam_position_prop, cam_pos);

	shader.double_face = false;
	shader.ztest_func = cglCompareFunc::kLess;

	shader.local_properties.set_float4(albedo_prop, cglVec4(0.5f, 0.0f, 0.0f, 1.0f));
	shader.local_properties.set_float4(light_direction_prop, light_direction);
	shader.local_properties.set_float4(light_color_prop, cglVec4(1.0f, 1.0f, 1.0f, 1.0f));
	shader.local_properties.set_float(light_intensity_prop, 1.0f);

	std::vector<Vertex> vert;
	std::vector<size_t> ind;

	// construct a sphere
	float step = TWO_PI / 64.0f;
	for (float phi = 0.0f; phi < TWO_PI; phi += step)
	{
		for (float theta = -HALF_PI; theta < HALF_PI; theta += step)
		{
			float phi0 = phi;
			float phi1 = phi + step;

			float theta0 = theta;
			float theta1 = theta + step;

			auto pos0 = spherical_coord(theta0, phi0);

			auto pos1 = spherical_coord(theta1, phi0);

			auto pos2 = spherical_coord(theta1, phi1);

			auto pos3 = spherical_coord(theta0, phi1);

			auto v0 = Vertex(pos0, pos0, {});
			auto v1 = Vertex(pos1, pos1, {});
			auto v2 = Vertex(pos2, pos2, {});
			auto v3 = Vertex(pos3, pos3, {});

			ind.push_back(vert.size());
			vert.push_back(v0);
			ind.push_back(vert.size());
			vert.push_back(v1);
			ind.push_back(vert.size());
			vert.push_back(v2);
			ind.push_back(vert.size());
			vert.push_back(v0);
			ind.push_back(vert.size());
			vert.push_back(v2);
			ind.push_back(vert.size());
			vert.push_back(v3);
		}
	}

	// set background color
	cglSetClearColor(tinymath::kColorBlue);

	while (cglIsMainWindowOpen())
	{
		// clear window buffer
		cglClearMainWindow();

		// clear buffer
		cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);

		// rotate light
		l_theta += 0.01f;
		l_phi += 0.01f;
		light_direction.x = cglCos(l_theta) * cglCos(l_phi);
		light_direction.y = cglSin (l_theta);
		light_direction.z = cglCos(l_theta) * cglSin(l_phi);
		light_direction = cglNormalize(light_direction);

		shader.local_properties.set_float4(light_direction_prop, light_direction);

		for (int i = 0; i < ind.size(); i += 3)
		{
			size_t index = ind[i];
			auto& v1 = vert[index];
			auto& v2 = vert[index + 1];
			auto& v3 = vert[index + 2];
			// submit primitive
			cglSubmitPrimitive(&shader, v1, v2, v3);
		}

		// fence primitive tasks
		cglFencePrimitives();

		// fence pixel tasks
		cglFencePixels();

		cglSwapBuffer(true);
	}

	cglCloseMainWindow();

	return 0;
}

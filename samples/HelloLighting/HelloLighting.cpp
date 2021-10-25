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
	cglSetViewPort(0, 0, w, h);

	// resize callback
	cglAddResizeEvent([](size_t resized_w, size_t resized_h, void* ud)
	{
		UNUSED(ud);
		cglSetViewPort(0, 0, resized_w, resized_h);
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

	uint32_t shader_id;
	cglCreateProgram(&shader, shader_id);

	cglUniformMatrix4fv(shader_id, mat_model_prop, model);
	cglUniformMatrix4fv(shader_id, mat_view_prop, view);
	cglUniformMatrix4fv(shader_id, mat_projection_prop, proj);
	cglUniformMatrix4fv(shader_id, mat_vp_prop, proj * view);
	cglUniformMatrix4fv(shader_id, mat_mvp_prop, proj * view * model);
	cglUniform4fv(shader_id, cam_position_prop, cam_pos);
	cglUniform4fv(shader_id, albedo_prop, cglVec4(0.5f, 0.0f, 0.0f, 1.0f));
	cglUniform4fv(shader_id, light_direction_prop, light_direction);
	cglUniform4fv(shader_id, light_color_prop, cglVec4(1.0f, 1.0f, 1.0f, 1.0f));
	cglUniform1f(shader_id, light_intensity_prop, 1.0f);

	cglDisable(cglPipelineFeature::kBlending);
	cglEnable(cglPipelineFeature::kFaceCulling);
	cglCullFace(cglFaceCulling::Back);
	cglDepthFunc(cglCompareFunc::kLess);

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

			Vertex v0, v1, v2, v3;
			v0.position = pos0;
			v0.normal = pos0;
			v1.position = pos1;
			v1.normal = pos1;
			v2.position = pos2;
			v2.normal = pos2;
			v3.position = pos3;
			v3.normal = pos3;

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

	auto vid = cglBindVertexBuffer(vert);
	auto iid = cglBindIndexBuffer(ind);

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

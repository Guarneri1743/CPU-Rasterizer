#include "PreviewGUI.hpp"
#include <memory>
#include <algorithm>
#include "imgui/imgui.h"
#include "Window.hpp"
#include "RenderTexture.hpp"
#include "Camera.hpp"
#include "Transform.hpp"
#include "PrimitiveFactory.hpp"
#include "Serialization.hpp"
#include "Utility.hpp"

#undef near
#undef far

namespace CpuRasterizer
{
	PreviewGUI::PreviewGUI(std::string path, int w, int h) 
	{
		this->width = (float)w;
		this->height = (float)h;

		rt_id = cglCreateBuffer(w, h, cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kCoverage);

		float aspect = 600.0f / 600.0f;
		tinymath::FrustumParam frustum_param;
		frustum_param.projection_mode = tinymath::Projection::kPerspective;
		frustum_param.perspective_param.aspect = aspect;
		frustum_param.perspective_param.near = 0.5f;
		frustum_param.perspective_param.far = 50.0f;
		frustum_param.perspective_param.fov = 60.0f;
		previe_camera = Camera::create(tinymath::vec3f(5.0f, 5.0f, 5.0f), frustum_param);
		previe_camera->transform->set_world_position({1.2f, 1.2f, 1.2f });
		previe_camera->transform->lookat({0.0f, 0.0f, 0.0f});

		std::shared_ptr<Material> mat = std::make_shared<Material>();
		std::replace(path.begin(), path.end(), '\\', '/');
		std::string relative_path = path;
		relative_path = CpuRasterizer::replace(relative_path, ASSETS_PATH, "");
		Serializer::deserialize(relative_path, *mat);
		auto model = PrimitiveFactory::sphere(mat);
		model->transform->set_world_position({0.0f, 0.0f, 0.0f});
		model->transform->set_local_scale({1.0f, 1.0f, 1.0f});
		renderer = std::make_unique<Renderer>(model);
		glGenTextures(1, &PreviewFBO);
		glBindTexture(GL_TEXTURE_2D, PreviewFBO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void PreviewGUI::on_gui(int index)
	{
		UNUSED(index);

		cglSetActiveRenderTarget(rt_id);
		cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil | cglFrameContent::kCoverage);
		CpuRasterSharedData.cam_far = previe_camera->frustum_param.perspective_param.far;
		CpuRasterSharedData.cam_near = previe_camera->frustum_param.perspective_param.near;
		CpuRasterSharedData.view_matrix = previe_camera->view_matrix();
		CpuRasterSharedData.proj_matrix = previe_camera->projection_matrix();
		DirectionalLight main_light;
		main_light.intensity = 1.0f;
		main_light.diffuse = tinymath::Color(1.0f, 0.8f, 0.8f, 1.0f);
		main_light.ambient = tinymath::Color(0.1f, 0.05f, 0.2f, 1.0f);
		main_light.specular = tinymath::Color(1.0f, 1.0f, 1.0f, 1.0f);
		CpuRasterSharedData.main_light = main_light;
		CpuRasterSharedData.camera_pos = previe_camera->transform->world_position();
		renderer->render();
		cglFencePixels();

		glBindTexture(GL_TEXTURE_2D, PreviewFBO);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)width, (int)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, cglGetActiveColorBuffer());
		cglResetActiveRenderTarget();

		ImVec2 size = ImVec2(width, height);                     
		ImVec2 uv0 = ImVec2(0.0f, 0.0f);                       
		ImVec2 uv1 = ImVec2(1.0f, 1.0f);
		ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);       
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);      
		if (ImGui::ImageButton((ImTextureID)PreviewFBO, size, uv0, uv1, 4, bg_col, tint_col))
		{

		}
	}
}
#include "LightInspector.hpp"
#include "imgui/imgui.h"

namespace Guarneri
{
	void LightInspector::on_gui(DirectionalLight& light)
	{
		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::SliderFloat("Yaw", &light.yaw, -180.0f, 180.0f))
			{
				light.update_rotation();
			}

			if (ImGui::SliderFloat("Pitch", &light.pitch, -180.0f, 180.0f))
			{
				light.update_rotation();
			}

			ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 10.0f);
			if (ImGui::ColorEdit3("Ambient", main_light_ambient))
			{
				light.ambient = tinymath::Color(main_light_ambient[0], main_light_ambient[1], main_light_ambient[2], main_light_ambient[3]);
			}
			else
			{
				main_light_ambient[0] = light.ambient.r;
				main_light_ambient[1] = light.ambient.g;
				main_light_ambient[2] = light.ambient.b;
				main_light_ambient[3] = light.ambient.a;
			}
			if (ImGui::ColorEdit3("Diffuse", main_light_diffuse))
			{
				light.diffuse = tinymath::Color(main_light_diffuse[0], main_light_diffuse[1], main_light_diffuse[2], main_light_diffuse[3]);
			}
			else
			{
				main_light_diffuse[0] = light.diffuse.r;
				main_light_diffuse[1] = light.diffuse.g;
				main_light_diffuse[2] = light.diffuse.b;
				main_light_diffuse[3] = light.diffuse.a;
			}
			if (ImGui::ColorEdit3("Specular", main_light_specular))
			{
				light.specular = tinymath::Color(main_light_specular[0], main_light_specular[1], main_light_specular[2], main_light_specular[3]);
			}
			else
			{
				main_light_specular[0] = light.specular.r;
				main_light_specular[1] = light.specular.g;
				main_light_specular[2] = light.specular.b;
				main_light_specular[3] = light.specular.a;
			}
		}
	}
}
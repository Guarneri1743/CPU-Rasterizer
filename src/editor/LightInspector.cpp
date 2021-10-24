#include "LightInspector.hpp"
#include "imgui/imgui.h"

namespace CpuRasterizer
{
	void LightInspector::on_gui(DirectionalLight& light)
	{
		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::InputFloat("distance", &distance))
			{
				light.distance = distance;
			}
			else
			{
				distance = light.distance;
			}

			if (ImGui::InputFloat("near", &n))
			{
				light.frustum_param.ortho_param.near = n;
				light.update_projection();
			}
			else
			{
				n = light.frustum_param.ortho_param.near;
			}

			if (ImGui::InputFloat("far", &f))
			{
				light.frustum_param.ortho_param. far = f;
				light.update_projection();
			}
			else
			{
				f = light.frustum_param.ortho_param.far;
			}

			if (ImGui::InputFloat("light", &l))
			{
				light.frustum_param.ortho_param.left = l;
				light.update_projection();
			}
			else
			{
				l = light.frustum_param.ortho_param.left;
			}

			if (ImGui::InputFloat("right", &r))
			{
				light.frustum_param.ortho_param.right = r;
				light.update_projection();
			}
			else
			{
				r = light.frustum_param.ortho_param.right;
			}

			if (ImGui::InputFloat("bottom", &b))
			{
				light.frustum_param.ortho_param.bottom = b;
				light.update_projection();
			}
			else
			{
				b = light.frustum_param.ortho_param.bottom;
			}

			if (ImGui::InputFloat("top", &t))
			{
				light.frustum_param.ortho_param.top = t;
				light.update_projection();
			}
			else
			{
				t = light.frustum_param.ortho_param.top;
			}

			if (ImGui::SliderFloat("Yaw", &light.yaw, -180.0f, 180.0f))
			{
				light.update_rotation();
			}

			if (ImGui::SliderFloat("Pitch", &light.pitch, -180.0f, 180.0f))
			{
				light.update_rotation();
			}

			ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 10.0f);
			if (ImGui::ColorEdit3("Ambient", ambient))
			{
				light.ambient = tinymath::Color(ambient[0], ambient[1], ambient[2], ambient[3]);
			}
			else
			{
				ambient[0] = light.ambient.r;
				ambient[1] = light.ambient.g;
				ambient[2] = light.ambient.b;
				ambient[3] = light.ambient.a;
			}
			if (ImGui::ColorEdit3("Diffuse", diffuse))
			{
				light.diffuse = tinymath::Color(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
			}
			else
			{
				diffuse[0] = light.diffuse.r;
				diffuse[1] = light.diffuse.g;
				diffuse[2] = light.diffuse.b;
				diffuse[3] = light.diffuse.a;
			}
			if (ImGui::ColorEdit3("Specular", specular))
			{
				light.specular = tinymath::Color(specular[0], specular[1], specular[2], specular[3]);
			}
			else
			{
				specular[0] = light.specular.r;
				specular[1] = light.specular.g;
				specular[2] = light.specular.b;
				specular[3] = light.specular.a;
			}
		}
	}
}
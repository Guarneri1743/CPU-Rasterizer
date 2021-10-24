#include "MaterialInspector.hpp"
#include <filesystem>
#include "Utility.hpp"
#include "Serialization.hpp"
#include "imgui/imgui.h"
#include "tinymath/tinymath.h"
#include "Shader.hpp"

namespace CpuRasterizer
{
	void MaterialInspector::on_gui(Material& material)
	{
		if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// shader selections
			{
				const char* shaders[] = { "default", "pbr_shader" };
				static int selected_shader = 0;
				if (ImGui::Button("Shaders.."))
					ImGui::OpenPopup("shaders");
				ImGui::SameLine();
				ImGui::TextUnformatted(selected_shader == -1 ? "<None>" : shaders[selected_shader]);
				if (ImGui::BeginPopup("shaders"))
				{
					ImGui::Separator();
					for (int i = 0; i < IM_ARRAYSIZE(shaders); i++)
						if (ImGui::Selectable(shaders[i]))
						{
							selected_shader = i;
							auto shader = ShaderLab::get_shader(shaders[selected_shader]);
							material.target_shader = shader;
						}
					ImGui::EndPopup();
				}
			}

			if (ImGui::ColorEdit3("TintColor", tint_color))
			{
				material.local_properties.set_float4(tint_color_prop, tinymath::vec4f(tint_color[0], tint_color[1], tint_color[2], tint_color[3]));
			}
			else
			{
				tint_color[0] = material.local_properties.get_float4(tint_color_prop).x;
				tint_color[1] = material.local_properties.get_float4(tint_color_prop).y;
				tint_color[2] = material.local_properties.get_float4(tint_color_prop).z;
				tint_color[3] = material.local_properties.get_float4(tint_color_prop).w;
			}

			if (ImGui::SliderFloat("roughness_multiplier", &roughness_multiplier, 0.0f, 2.0f))
			{
				material.local_properties.set_float(roughness_multiplier_prop, roughness_multiplier);
			}
			else
			{
				roughness_multiplier = material.local_properties.get_float(roughness_multiplier_prop);
			}

			if (ImGui::SliderFloat("roughness_offset", &roughness_offset, -1.0f, 1.0f))
			{
				material.local_properties.set_float(roughness_offset_prop, roughness_offset);
			}
			else
			{
				roughness_offset = material.local_properties.get_float(roughness_offset_prop);
			}

			if (ImGui::SliderFloat("metallic_multiplier", &metallic_multiplier, 0.0f, 2.0f))
			{
				material.local_properties.set_float(metallic_multiplier_prop, metallic_multiplier);
			}
			else
			{
				metallic_multiplier = material.local_properties.get_float(metallic_multiplier_prop);
			}

			if (ImGui::SliderFloat("metallic_offset", &metallic_offset, -1.0f, 1.0f))
			{
				material.local_properties.set_float(metallic_offset_prop, metallic_offset);
			}
			else
			{
				metallic_offset = material.local_properties.get_float(metallic_offset_prop);
			}

			if (ImGui::Button("Save"))
			{
				std::filesystem::path path(ASSETS_PATH + material.meta_path);
				Serializer::serialize(material, material.meta_path);
			}
		}
	}
}
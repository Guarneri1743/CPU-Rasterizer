#include "MaterialInspector.hpp"
#include <filesystem>
#include "Utility.hpp"
#include "Serialization.hpp"
#include "imgui/imgui.h"
#include "tinymath/tinymath.h"

namespace Guarneri
{
	void MaterialInspector::on_gui(Material& material)
	{
		if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::ColorEdit3("TintColor", tint_color))
			{
				material.set_float4(tint_color_prop, tinymath::vec4f(tint_color[0], tint_color[1], tint_color[2], tint_color[3]));
			}
			else
			{
				tint_color[0] = material.get_float4(tint_color_prop).x;
				tint_color[1] = material.get_float4(tint_color_prop).y;
				tint_color[2] = material.get_float4(tint_color_prop).z;
				tint_color[3] = material.get_float4(tint_color_prop).w;
			}

			if (ImGui::SliderFloat("roughness_multiplier", &roughness_multiplier, 0.0f, 2.0f))
			{
				material.set_float(roughness_multiplier_prop, roughness_multiplier);
			}
			else
			{
				roughness_multiplier = material.get_float(roughness_multiplier_prop);
			}

			if (ImGui::SliderFloat("roughness_offset", &roughness_offset, -1.0f, 1.0f))
			{
				material.set_float(roughness_offset_prop, roughness_offset);
			}
			else
			{
				roughness_offset = material.get_float(roughness_offset_prop);
			}

			if (ImGui::SliderFloat("metallic_multiplier", &metallic_multiplier, 0.0f, 2.0f))
			{
				material.set_float(metallic_multiplier_prop, metallic_multiplier);
			}
			else
			{
				metallic_multiplier = material.get_float(metallic_multiplier_prop);
			}

			if (ImGui::SliderFloat("metallic_offset", &metallic_offset, -1.0f, 1.0f))
			{
				material.set_float(metallic_offset_prop, metallic_offset);
			}
			else
			{
				metallic_offset = material.get_float(metallic_offset_prop);
			}

			if (ImGui::Button("Save"))
			{
				std::filesystem::path path(ASSETS_PATH + material.meta_path);
				Serializer::serialize(material, material.meta_path);
			}
		}
	}
}
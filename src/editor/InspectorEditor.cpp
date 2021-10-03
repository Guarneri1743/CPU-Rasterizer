#include "InspectorEditor.hpp"
#include <algorithm>
#include <filesystem>

#include "imgui/imgui.h"
#include "Marcos.h"
#include "InputManager.hpp"
#include "Window.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "Scene.hpp"
#include "Utility.hpp"
#include "Logger.hpp"
#include "Serialization.hpp"

#undef near
#undef far

namespace Guarneri
{
	bool show = true;	
	int rt_size[2];
	int sub_samples = 4;

	InspectorEditor::InspectorEditor(float x, float y, float w, float h) : BaseEditor(x, y, w, h)
	{
		no_scrollbar = true;
		no_titlebar = true;
		no_menu = true;
		no_collapse = true;
		no_resize = true;
		no_close = true;
		no_move = true;
		title = "Setting";
	}

	void InspectorEditor::draw_inspector()
	{
		if (Scene::current() == nullptr) return;

		Scene& scene = *Scene::current();

		if (scene.selection != nullptr)
		{
			Model* model = scene.selection->get_model();
			if (model != nullptr)
			{
				if (model->transform != nullptr)
				{
					transform_inspector.on_gui(*model->transform);
				}

				if (model->material != nullptr)
				{
					material_inspector.on_gui(*model->material);
				}
			}
		}
	}

	void InspectorEditor::draw_settings()
	{
		{
			ImGui::Text("Window Size: %f, %f", tinymath::vec2f((float)Window::main()->get_width(), (float)Window::main()->get_height()));
			if (ImGui::InputInt2("RT Size", rt_size))
			{
				INST(GraphicsDevice).resize(rt_size[0], rt_size[1]);
			}
			else
			{
				rt_size[0] = INST(GraphicsDevice).get_width();
				rt_size[1] = INST(GraphicsDevice).get_height();
			}
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		}

		Scene& scene = *Scene::current();

		light_inspector.on_gui(scene.main_light);
	
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Position: %f, %f, %f", scene.main_cam->transform->world_position().x , scene.main_cam->transform->world_position().y, scene.main_cam->transform->world_position().z);
			ImGui::Text("Direction: %f, %f, %f", scene.main_cam->transform->world_euler_angles().x, scene.main_cam->transform->world_euler_angles().y, scene.main_cam->transform->world_euler_angles().z);

			bool cam_update = false;
			cam_update = ImGui::SliderFloat("Near", &scene.main_cam->near, 0.0f, 5.0f);
			cam_update = ImGui::SliderFloat("Far", &scene.main_cam->far, 5.0f, 10000.0f) || cam_update;
			cam_update = ImGui::SliderFloat("Fov", &scene.main_cam->fov, 0.0f, 180.0f) || cam_update;
			cam_update = ImGui::SliderFloat("Aspect", &scene.main_cam->aspect, 0.0f, 2.0f) || cam_update;
			if (cam_update) scene.main_cam->update_proj_mode();
			if (ImGui::Checkbox("MSAA", &INST(GlobalShaderParams).enable_msaa))
			{
				if (INST(GlobalShaderParams).enable_msaa)
				{
					INST(GraphicsDevice).set_subsample_count((uint8_t)sub_samples);
				}
				else
				{
					INST(GraphicsDevice).set_subsample_count((uint8_t)0);
				}
			}
			if (INST(GlobalShaderParams).enable_msaa)
			{
				if (ImGui::InputInt("Subsamples", &sub_samples))
				{
					INST(GraphicsDevice).set_subsample_count((uint8_t)sub_samples);
				}
				else
				{
					sub_samples = (int)INST(GraphicsDevice).get_subsample_count();
				}
			}
		}

		if (ImGui::CollapsingHeader("Shadow", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("ShadowOn", &INST(GlobalShaderParams).enable_shadow);
			if (INST(GlobalShaderParams).enable_shadow)
			{
				ImGui::Checkbox("PCF", &INST(GlobalShaderParams).pcf_on);
				ImGui::SliderFloat("Bias", &INST(GlobalShaderParams).shadow_bias, 0.0f, 0.01f);
			}
		}

		if (ImGui::CollapsingHeader("IBL", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("IBL_On", &INST(GlobalShaderParams).enable_ibl);
		}

		if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const char* debug_views[] = {
				"None",
				"WireFrame",
				"ShadowMap",
				"Depth",
				"Stencil",
				"UV",
				"Normal",
				"VertexColor",
				"Specular",
				"Tiles",
				"IrradianceMap",
				"Roughness",
				"Metallic",
				"AO",
				"IndirectDiffuse",
				"IndirectSpecular"
			};
			const RenderFlag view_flags[] =
			{
				RenderFlag::DISABLE,
				RenderFlag::WIREFRAME,
				RenderFlag::SHADOWMAP,
				RenderFlag::DEPTH,
				RenderFlag::STENCIL,
				RenderFlag::UV,
				RenderFlag::NORMAL,
				RenderFlag::VERTEX_COLOR,
				RenderFlag::SPECULAR,
				RenderFlag::FRAME_TILE,
				RenderFlag::IRRADIANCE_MAP,
				RenderFlag::ROUGHNESS,
				RenderFlag::METALLIC,
				RenderFlag::AO,
				RenderFlag::INDIRECT_DIFFUSE,
				RenderFlag::INDIRECT_SPECULAR
			};
			static int selected_view = 0;
			if (ImGui::Button("Views.."))
				ImGui::OpenPopup("debug_views");
			ImGui::SameLine();
			ImGui::TextUnformatted(selected_view == -1 ? "<None>" : debug_views[selected_view]);
			if (ImGui::BeginPopup("debug_views"))
			{
				ImGui::Separator();
				for (int i = 0; i < IM_ARRAYSIZE(debug_views); i++)
					if (ImGui::Selectable(debug_views[i]))
					{
						selected_view = i;
						INST(GlobalShaderParams).render_flag = RenderFlag::DISABLE;
						INST(GlobalShaderParams).render_flag = view_flags[i];
					}
				ImGui::EndPopup();
			}
		}

		if (ImGui::CollapsingHeader("Others"))
		{
			const char* work_flows[] = { "Metallic", "Specular" };
			static int selected_flow = (int)INST(GlobalShaderParams).workflow;
			if (ImGui::Button("Workflows.."))
				ImGui::OpenPopup("work_flows");
			ImGui::SameLine();
			ImGui::TextUnformatted(selected_flow == -1 ? "<None>" : work_flows[selected_flow]);
			if (ImGui::BeginPopup("work_flows"))
			{
				ImGui::Separator();
				for (int i = 0; i < IM_ARRAYSIZE(work_flows); i++)
					if (ImGui::Selectable(work_flows[i]))
					{
						selected_flow = i;
						INST(GlobalShaderParams).workflow = (PBRWorkFlow)selected_flow;
					}
				ImGui::EndPopup();
			}

			const char* color_spaces[] = { "Gamma", "Linear" };
			static int selected_color_space = (int)INST(GlobalShaderParams).color_space;
			if (ImGui::Button("ColorSpaces.."))
				ImGui::OpenPopup("color_spaces");
			ImGui::SameLine();
			ImGui::TextUnformatted(selected_color_space == -1 ? "<None>" : color_spaces[selected_color_space]);
			if (ImGui::BeginPopup("color_spaces"))
			{
				ImGui::Separator();
				for (int i = 0; i < IM_ARRAYSIZE(color_spaces); i++)
					if (ImGui::Selectable(color_spaces[i]))
					{
						selected_color_space = i;
						INST(GlobalShaderParams).color_space = (ColorSpace)selected_color_space;
					}
				ImGui::EndPopup();
			} 
		}
	}

	void InspectorEditor::on_gui()
	{
		rect = tinymath::Rect((float)Window::main()->get_width() - (float)kRightWidth, (float)kTopHeight, (float)kRightWidth, (float)Window::main()->get_height() - (float)kTopHeight - (float)kBottomHeight);

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("Inspector", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Inspector"))
			{
				draw_inspector();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Setting"))
			{
				draw_settings();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
}
#include "InspectorEditor.hpp"
#include <algorithm>
#include <filesystem>

#include "imgui/imgui.h"
#include "Define.hpp"
#include "InputManager.hpp"
#include "Window.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "Scene.hpp"
#include "Utility.hpp"
#include "Logger.hpp"
#include "Serialization.hpp"
#include "Time.hpp"

#undef near
#undef far

namespace CpuRasterizor
{
	bool show = true;	
	int rt_size[2];
	int shadowmap_size[2];
	int sub_samples = 4;

	InspectorEditor::InspectorEditor(int x, int y, int w, int h) : BaseEditor(x, y, w, h)
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
			if (ImGui::InputInt2("RT Size", rt_size))
			{
				CpuRasterApi.set_viewport(rt_size[0], rt_size[1]);
			}
			else
			{
				rt_size[0] = (int)CpuRasterApi.get_width();
				rt_size[1] = (int)CpuRasterApi.get_height();
			}
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		}

		Scene& scene = *Scene::current();

		light_inspector.on_gui(*scene.main_light);
	
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Position: %f, %f, %f", scene.main_cam->transform->world_position().x , scene.main_cam->transform->world_position().y, scene.main_cam->transform->world_position().z);
			ImGui::Text("Direction: %f, %f, %f", scene.main_cam->transform->world_euler_angles().x, scene.main_cam->transform->world_euler_angles().y, scene.main_cam->transform->world_euler_angles().z);

			bool cam_update = false;
			if (scene.main_cam->frustum_param.projection_mode == tinymath::Projection::kPerspective)
			{
				cam_update = ImGui::SliderFloat("Near", &scene.main_cam->frustum_param.perspective_param.near, 0.0f, 5.0f);
				cam_update = ImGui::SliderFloat("Far", &scene.main_cam->frustum_param.perspective_param.far, 5.0f, 10000.0f) || cam_update;
				cam_update = ImGui::SliderFloat("Fov", &scene.main_cam->frustum_param.perspective_param.fov, 0.0f, 180.0f) || cam_update;
				cam_update = ImGui::SliderFloat("Aspect", &scene.main_cam->frustum_param.perspective_param.aspect, 0.0f, 2.0f) || cam_update;
			}
			else
			{

			}
			
			if (cam_update) scene.main_cam->update_projection_matrix();
			if (ImGui::Checkbox("MSAA", &CpuRasterSharedData.enable_msaa))
			{
				if (CpuRasterSharedData.enable_msaa)
				{
					CpuRasterApi.set_subsample_count((uint8_t)sub_samples);
				}
				else
				{
					CpuRasterApi.set_subsample_count((uint8_t)0);
				}
			}

			if (CpuRasterSharedData.enable_msaa)
			{
				const char* frequencies[] = {
				"PixelFrequency",
				"SubsampleFrequency",
				};
				const MultiSampleFrequency frequency_flags[] =
				{
					MultiSampleFrequency::kPixelFrequency,
					MultiSampleFrequency::kSubsampleFrequency
				};
				static int selected_frequency = 0;
				if (ImGui::Button("MultiSampleFrequency.."))
					ImGui::OpenPopup("frequencies");
				ImGui::SameLine();
				ImGui::TextUnformatted(selected_frequency == -1 ? "<None>" : frequencies[selected_frequency]);
				if (ImGui::BeginPopup("frequencies"))
				{
					ImGui::Separator();
					for (int i = 0; i < IM_ARRAYSIZE(frequencies); i++)
						if (ImGui::Selectable(frequencies[i]))
						{
							selected_frequency = i;
							CpuRasterSharedData.multi_sample_frequency = frequency_flags[i];
						}
					ImGui::EndPopup();
				}

				if (ImGui::InputInt("Subsamples", &sub_samples))
				{
					CpuRasterApi.set_subsample_count((uint8_t)sub_samples);
				}
				else
				{
					sub_samples = (int)CpuRasterApi.get_subsample_count();
				}
			}
		}

		if (ImGui::CollapsingHeader("Shadow", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("ShadowOn", &CpuRasterSharedData.enable_shadow);
			if (CpuRasterSharedData.enable_shadow)
			{
				if (ImGui::InputInt2("ShadowMap Resolution", shadowmap_size))
				{
					Scene::current()->resize_shadowmap(shadowmap_size[0], shadowmap_size[1]);
				}
				else
				{
					size_t sw, sh;
					Scene::current()->get_shadowmap_size(sw, sh);
					shadowmap_size[0] = (int)sw;
					shadowmap_size[1] = (int)sh;
				}
				ImGui::Checkbox("PCF", &CpuRasterSharedData.pcf_on);
				ImGui::SliderFloat("Bias", &CpuRasterSharedData.shadow_bias, 0.0f, 0.01f);
			}
		}

		if (ImGui::CollapsingHeader("IBL", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("IBL_On", &CpuRasterSharedData.enable_ibl);

			std::string hdri_path[] = {
				"None",
				"valley",
				"ballroom",
				"cave_room",
				"fireplace",
				"green_house",
				"kloppenheim",
				"loft_hall",
				"man_outside",
				"memorial",
				"nave",
				"preller",
				"veranda"
			};

			static int selected_view = 0;
			if (ImGui::Button("HDRI.."))
				ImGui::OpenPopup("hdri");
			ImGui::SameLine();
			ImGui::TextUnformatted(selected_view == -1 ? "<None>" : hdri_path[selected_view].c_str());
			if (ImGui::BeginPopup("hdri"))
			{
				ImGui::Separator();
				for (int i = 0; i < IM_ARRAYSIZE(hdri_path); i++)
					if (ImGui::Selectable(hdri_path[i].c_str()))
					{
						selected_view = i;
						Scene::current()->set_cubemap("/hdri/" + hdri_path[selected_view] + ".hdri");
					}
				ImGui::EndPopup();
			}
		}

		if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Gizmos", &CpuRasterSharedData.enable_gizmos);

			ImGui::Checkbox("Mipmap", &CpuRasterSharedData.enable_mipmap);

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
				"Albedo",
				"Roughness",
				"Metallic",
				"AO",
				"IndirectDiffuse",
				"IndirectSpecular",
				"Mipmap"
			};
			const RenderFlag view_flags[] =
			{
				RenderFlag::kNone,
				RenderFlag::kWireFrame,
				RenderFlag::kShadowMap,
				RenderFlag::kDepth,
				RenderFlag::kStencil,
				RenderFlag::kUV,
				RenderFlag::kNormal,
				RenderFlag::kVertexColor,
				RenderFlag::kSpecular,
				RenderFlag::kFrameTile,
				RenderFlag::kIrradianceMap,
				RenderFlag::kAlbedo,
				RenderFlag::kRoughness,
				RenderFlag::kMetallic,
				RenderFlag::kAO,
				RenderFlag::kIndirectDiffuse,
				RenderFlag::kIndirectSpecular,
				RenderFlag::kMipmap
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
						CpuRasterSharedData.debug_flag = RenderFlag::kNone;
						CpuRasterSharedData.debug_flag = view_flags[i];
					}
				ImGui::EndPopup();
			}
		}

		if (ImGui::CollapsingHeader("Others"))
		{
			const char* work_flows[] = { "Metallic", "Specular" };
			static int selected_flow = (int)CpuRasterSharedData.workflow;
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
						CpuRasterSharedData.workflow = (PBRWorkFlow)selected_flow;
					}
				ImGui::EndPopup();
			}

			const char* color_spaces[] = { "Gamma", "Linear" };
			static int selected_color_space = (int)CpuRasterSharedData.color_space;
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
						CpuRasterSharedData.color_space = (ColorSpace)selected_color_space;
					}
				ImGui::EndPopup();
			} 
		}
	}

	void InspectorEditor::on_gui()
	{
		rect = tinymath::Rect(Window::main()->get_width() - kRightWidth, kTopHeight, kRightWidth, Window::main()->get_height() - kTopHeight - kBottomHeight);

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
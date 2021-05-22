#include "SettingEditor.hpp"
#include "imgui/imgui.h"
#include "Marcos.h"
#include "InputManager.hpp"
#include "Window.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "Scene.hpp"

#undef near
#undef far

namespace Guarneri
{
	bool show = true;
	float main_light_ambient[4];
	float main_light_diffuse[4];
	float main_light_specular[4];
	int rt_size[2];

	void DrawModelMenu()
	{
		ImGui::MenuItem("(load model)", NULL, false, false);

		if (ImGui::MenuItem("Open", "Ctrl+O")) 
		{
			
		}

		if (ImGui::BeginMenu("Open Recent"))
		{
			ImGui::MenuItem("fish_hat.c");
			ImGui::MenuItem("fish_hat.inl");
			ImGui::MenuItem("fish_hat.h");
			if (ImGui::BeginMenu("More.."))
			{
				ImGui::MenuItem("Hello");
				ImGui::MenuItem("Sailor");
				if (ImGui::BeginMenu("Recurse.."))
				{
					DrawModelMenu();
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Save", "Ctrl+S")) {}
		if (ImGui::MenuItem("Save As..")) {}
	}

	bool mani_editor_open = true;

	void SettingEditor::on_gui()
	{
		static bool no_titlebar = false;
		static bool no_scrollbar = false;
		static bool no_menu = false;
		static bool no_move = false;
		static bool no_resize = false;
		static bool no_collapse = false;
		static bool no_close = false;
		static bool no_nav = false;
		static bool no_background = false;
		static bool no_bring_to_front = false;

		ImGuiWindowFlags window_flags = 0;
		if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
		if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
		if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
		if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
		if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
		if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
		if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
		if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
		if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(400, 700));
			if (!ImGui::Begin("Settings", &mani_editor_open, window_flags))
			{
				ImGui::End();
				return;
			}

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Model"))
				{
					DrawModelMenu();
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			{
				ImGui::Text(("Window Size: " + Vector2(INST(GlobalShaderParams).window_width, INST(GlobalShaderParams).window_height).str()).c_str());
				if (ImGui::InputInt2("RT Size: ", rt_size))
				{
					INST(GlobalShaderParams).width = rt_size[0];
					INST(GlobalShaderParams).height = rt_size[1];
					INST(GraphicsDevice).resize(rt_size[0], rt_size[1]);
				}
				else
				{
					rt_size[0] = INST(GlobalShaderParams).width;
					rt_size[1] = INST(GlobalShaderParams).height;
				}
				ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
			}

			Scene& scene = *Scene::current();

			if(ImGui::CollapsingHeader("Camera"))
			{
				ImGui::Text(("Position: " + scene.main_cam->transform->world_position().str()).c_str());
				ImGui::Text(("Direction: " + scene.main_cam->transform->world_euler_angles().str()).c_str());

				bool cam_update = false;
				cam_update = ImGui::SliderFloat("Near", &scene.main_cam->near, 0.0f, 5.0f);
				cam_update = ImGui::SliderFloat("Far", &scene.main_cam->far, 5.0f, 10000.0f) || cam_update;
				cam_update = ImGui::SliderFloat("Fov", &scene.main_cam->fov, 0.0f, 180.0f) || cam_update;
				cam_update = ImGui::SliderFloat("Aspect", &scene.main_cam->aspect, 0.0f, 2.0f) || cam_update;
				if (cam_update) scene.main_cam->update_proj_mode();
				ImGui::Checkbox("MSAA", &scene.main_cam->enable_msaa);
			}

			if (ImGui::CollapsingHeader("Shadow"))
			{
				ImGui::Checkbox("Shadow", &INST(GlobalShaderParams).enable_shadow);
				if (INST(GlobalShaderParams).enable_shadow)
				{
					ImGui::Checkbox("PCF", &INST(GlobalShaderParams).pcf_on);
					ImGui::SliderFloat("Bias", &INST(GlobalShaderParams).shadow_bias, 0.0f, 0.01f);
				}
			}

			if (ImGui::CollapsingHeader("Light"))
			{
				ImGui::SliderFloat("Intensity", &scene.main_light.intensity, 0.0f, 10.0f);
				if (ImGui::ColorEdit3("Ambient", main_light_ambient))
				{
					scene.main_light.ambient = Color(main_light_ambient[0], main_light_ambient[1], main_light_ambient[2], main_light_ambient[3]);
				}
				else
				{
					main_light_ambient[0] = scene.main_light.ambient.r;
					main_light_ambient[1] = scene.main_light.ambient.g;
					main_light_ambient[2] = scene.main_light.ambient.b;
					main_light_ambient[3] = scene.main_light.ambient.a;
				}
				if (ImGui::ColorEdit3("Diffuse", main_light_diffuse))
				{
					scene.main_light.diffuse = Color(main_light_diffuse[0], main_light_diffuse[1], main_light_diffuse[2], main_light_diffuse[3]);
				}
				else
				{
					main_light_diffuse[0] = scene.main_light.diffuse.r;
					main_light_diffuse[1] = scene.main_light.diffuse.g;
					main_light_diffuse[2] = scene.main_light.diffuse.b;
					main_light_diffuse[3] = scene.main_light.diffuse.a;
				}
				if (ImGui::ColorEdit3("Specular", main_light_specular))
				{
					scene.main_light.specular = Color(main_light_specular[0], main_light_specular[1], main_light_specular[2], main_light_specular[3]);
				}
				else
				{
					main_light_specular[0] = scene.main_light.specular.r;
					main_light_specular[1] = scene.main_light.specular.g;
					main_light_specular[2] = scene.main_light.specular.b;
					main_light_specular[3] = scene.main_light.specular.a;
				}
			}

			if (ImGui::CollapsingHeader("Debug"))
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
					"Tiles"
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
					RenderFlag::FRAME_TILE
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

			ImGui::End();
		}
	}
}
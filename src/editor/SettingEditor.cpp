#include "SettingEditor.hpp"
#include "imgui/imgui.h"
#include "Marcos.h"
#include "InputManager.hpp"
#include "Window.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "Scene.hpp"
#include "Application.hpp"
#include <algorithm>

#undef near
#undef far

namespace Guarneri
{
	bool show = true;
	float main_light_ambient[4];
	float main_light_diffuse[4];
	float main_light_specular[4];
	int rt_size[2];
	float position[3];
	float angles[3];
	float scale[3];

	SettingEditor::SettingEditor() : BaseEditor()
	{
		no_collapse = true;
		no_resize = true;
		no_close = true;
		no_move = true;
		std::fill(std::begin(position), std::end(position), 0.0f);
		std::fill(std::begin(angles), std::end(angles), 0.0f);
		std::fill(std::begin(scale), std::end(scale), 0.0f);
	}

	void SettingEditor::on_gui()
	{
		ImGui::SetNextWindowPos(ImVec2((float)(Window::main()->get_width() - kSettingWidth), (float)kTopToolbarHeight));
		ImGui::SetNextWindowSize(ImVec2((float)kSettingWidth, (float)(Window::main()->get_height() - kTopToolbarHeight)));

		if (!ImGui::Begin("Settings", no_close ? nullptr : &show, get_window_flag()))
		{
			ImGui::End();
			return;
		}

		{
			ImGui::Text(("Window Size: " + Vector2((float)Window::main()->get_width(), (float)Window::main()->get_height()).str()).c_str());
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

		if (ImGui::CollapsingHeader("Inspector"))
		{
			if (scene.selection != nullptr)
			{
				if (ImGui::InputFloat3("position", position))
				{
					scene.selection->set_local_position({ position[0], position[1], position[2] });
				}
				else
				{
					position[0] = scene.selection->local_position.x;
					position[1] = scene.selection->local_position.y;
					position[2] = scene.selection->local_position.z;
				}

				if (ImGui::InputFloat3("angles", angles))
				{
					scene.selection->set_local_rotation({ DEGREE2RAD(angles[0]), DEGREE2RAD(angles[1]), DEGREE2RAD(angles[2]) });
				}
				else
				{
					angles[0] = RAD2DEGREE(scene.selection->local_rotation.x);
					angles[1] = RAD2DEGREE(scene.selection->local_rotation.y);
					angles[2] = RAD2DEGREE(scene.selection->local_rotation.z);
				}

				if (ImGui::InputFloat3("scale", scale))
				{
					scene.selection->set_local_scale({ scale[0], scale[1], scale[2] });
				}
				else
				{
					scale[0] = scene.selection->local_scale.x;
					scale[1] = scene.selection->local_scale.y;
					scale[2] = scene.selection->local_scale.z;
				}
			}
		}

		if (ImGui::CollapsingHeader("Camera"))
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
			ImGui::Checkbox("On", &INST(GlobalShaderParams).enable_shadow);
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
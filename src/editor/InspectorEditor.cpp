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
	float tint_color[4];
	float roughness_multiplier;
	float roughness_offset;
	float metallic_multiplier;
	float metallic_offset;
	float main_light_ambient[4];
	float main_light_diffuse[4];
	float main_light_specular[4];
	int rt_size[2];
	float position[3];
	float angles[3];
	float scale[3];
	float main_light_yaw;
	float main_light_pitch;
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
		std::fill(std::begin(position), std::end(position), 0.0f);
		std::fill(std::begin(angles), std::end(angles), 0.0f);
		std::fill(std::begin(scale), std::end(scale), 0.0f);
		title = "Setting";
	}

	void InspectorEditor::draw_inspector()
	{
		if (Scene::current() == nullptr) return;

		Scene& scene = *Scene::current();

		if (scene.selection != nullptr)
		{
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
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

			if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (scene.selection->get_model() != nullptr)
				{
					auto material = scene.selection->get_model()->material;

					if (material != nullptr)
					{
						if (ImGui::ColorEdit3("TintColor", tint_color))
						{
							material->set_float4(tint_color_prop, Vector4(tint_color[0], tint_color[1], tint_color[2], tint_color[3]));
						}
						else
						{
							tint_color[0] = material->get_float4(tint_color_prop).x;
							tint_color[1] = material->get_float4(tint_color_prop).y;
							tint_color[2] = material->get_float4(tint_color_prop).z;
							tint_color[3] = material->get_float4(tint_color_prop).w;
						}

						if (ImGui::SliderFloat("roughness_multiplier", &roughness_multiplier, 0.0f, 2.0f))
						{
							material->set_float(roughness_multiplier_prop, roughness_multiplier);
						}
						else
						{
							roughness_multiplier = material->get_float(roughness_multiplier_prop);
						}

						if (ImGui::SliderFloat("roughness_offset", &roughness_offset, -1.0f, 1.0f))
						{
							material->set_float(roughness_offset_prop, roughness_offset);
						}
						else
						{
							roughness_offset = material->get_float(roughness_offset_prop);
						}

						if (ImGui::SliderFloat("metallic_multiplier", &metallic_multiplier, 0.0f, 2.0f))
						{
							material->set_float(metallic_multiplier_prop, metallic_multiplier);
						}
						else
						{
							metallic_multiplier = material->get_float(metallic_multiplier_prop);
						}

						if (ImGui::SliderFloat("metallic_offset", &metallic_offset, -1.0f, 1.0f))
						{
							material->set_float(metallic_offset_prop, metallic_offset);
						}
						else
						{
							metallic_offset = material->get_float(metallic_offset_prop);
						}

						if (ImGui::Button("Save"))
						{
							std::filesystem::path path(ASSETS_PATH + material->meta_path);
							if (!std::filesystem::exists(path) || std::filesystem::is_directory(path))
							{
								material->meta_path = "/materials/" + scene.selection->get_model()->name +".material";
							}
							Serializer::serialize(*material, material->meta_path);
							Serializer::serialize(*scene.selection->get_model(), scene.selection->get_model()->meta_path);
						}
					}
				}
			}
		}
	}

	void InspectorEditor::draw_settings()
	{
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

		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text(("Position: " + scene.main_cam->transform->world_position().str()).c_str());
			ImGui::Text(("Direction: " + scene.main_cam->transform->world_euler_angles().str()).c_str());

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

		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::SliderFloat("Yaw", &scene.main_light.yaw, -180.0f, 180.0f))
			{
				scene.main_light.update_rotation();
			}

			if (ImGui::SliderFloat("Pitch", &scene.main_light.pitch, -180.0f, 180.0f))
			{
				scene.main_light.update_rotation();
			}

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
		rect = Rect((float)Window::main()->get_width() - (float)kRightWidth, (float)kTopHeight, (float)kRightWidth, (float)Window::main()->get_height() - (float)kTopHeight - (float)kBottomHeight);

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
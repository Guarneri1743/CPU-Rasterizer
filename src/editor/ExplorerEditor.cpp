#include "ExplorerEditor.hpp"
#include "Window.hpp"
#include <filesystem>
#include "Utility.hpp"
#include "Scene.hpp"
#include "Model.hpp"
#include <iostream>
#include <string>

namespace Guarneri
{
	ExplorerEditor::ExplorerEditor(float x, float y, float w, float h) : BaseEditor(x, y, w, h)
	{
		no_titlebar = true;
		no_menu = true;
		no_collapse = true;
		no_resize = true;
		no_close = true;
		no_move = true;
		no_scrollbar_with_mouse = true;
		no_scrollbar = true;
		title = "Explorer";
	}

	void ExplorerEditor::on_gui()
	{
		rect = Rect(0.0f, (float)Window::main()->get_height() - (float)kBottomHeight, (float)Window::main()->get_width(), (float)kBottomHeight);
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("ExplorerBar", tab_bar_flags))
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
			if (ImGui::BeginTabItem("Explorer"))
			{
				// explorer
				{
					ImGui::BeginChild("Explorer", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.25f, rect.h()), true, window_flags);
					draw_directories(ASSETS_PATH.c_str(), selected_in_explorer);
					ImGui::EndChild();
				}

				ImGui::SameLine();

				// contents
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);
					ImGui::BeginChild("Content", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.75f, rect.h()), true, window_flags);
					if (std::filesystem::is_directory(selected_in_explorer))
					{
						draw_files(selected_in_explorer, selected_in_contents);
					}
					ImGui::EndChild();
					ImGui::PopStyleVar();
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Console"))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);
				ImGui::BeginChild("Console", ImVec2(ImGui::GetWindowContentRegionWidth(), rect.h()), true, window_flags);
				draw_console();
				ImGui::EndChild();
				ImGui::PopStyleVar();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	void ExplorerEditor::draw_files(std::filesystem::path dir, std::filesystem::path& selected)
	{
		for (auto& path : std::filesystem::directory_iterator(dir))
		{
			std::string full_name = path.path().generic_string();
			size_t last_slash = full_name.find_last_of("/\\");
			std::string nice_name = full_name;
			nice_name = nice_name.replace(nice_name.begin(), nice_name.begin() + last_slash + 1, "");

			ImGui::PushID(full_name.c_str());
			if (!path.is_directory())
			{
				if (ImGui::Selectable(nice_name.c_str(), selected == path.path(), ImGuiSelectableFlags_AllowItemOverlap))
				{
					selected = path.path();
				}
				
				if (path.path().has_extension())
				{
					std::string asset_path = ASSETS_PATH;
					size_t length = asset_path.length();
					std::string relative_path = path.path().generic_string();
					relative_path = relative_path.replace(relative_path.begin(), relative_path.begin() + length, "");
					std::string ext = path.path().extension().string();
					if (ext == ".scene")
					{
						ImGui::SameLine();
						if (ImGui::SmallButton("Open"))
						{
							Scene::open_scene(relative_path.c_str());
						}
					}
					else if (ext == ".model")
					{
						ImGui::SameLine();
						if (ImGui::SmallButton("Add"))
						{
							auto model = Model::load_asset(relative_path.c_str());
							Scene::current()->add(model);
						}
					}
				}
			}

			// todo
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("file", NULL, 0);
				ImGui::Text(nice_name.c_str());
				ImGui::EndDragDropSource();
			}
			ImGui::PopID();
		}
	}

	void ExplorerEditor::draw_console()
	{
	}

	std::size_t count_dirs(std::filesystem::path path)
	{
		using std::filesystem::directory_iterator;
		using fp = bool (*)(const std::filesystem::path&);
		return std::count_if(directory_iterator(path), directory_iterator{}, (fp)std::filesystem::is_directory);
	}

	void ExplorerEditor::draw_directories(std::filesystem::path dir, std::filesystem::path& selected)
	{
		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth;

		for (auto& path : std::filesystem::directory_iterator(dir))
		{
			std::string full_name = path.path().generic_string();
			size_t last_slash = full_name.find_last_of("/\\");
			std::string nice_name = full_name;
			nice_name = nice_name.replace(nice_name.begin(), nice_name.begin() + last_slash + 1, "");

			if (path.path() == selected)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}
			else
			{
				flags &= ~ImGuiTreeNodeFlags_Selected;
			}

			ImGuiTreeNodeFlags node_flags = flags;
			if (path.is_directory())
			{
				if (count_dirs(path) > 0)
				{
					if (ImGui::TreeNodeEx(nice_name.c_str(), node_flags))
					{
						draw_directories(path.path(), selected);
						ImGui::TreePop();
					}
				}
				else
				{
					node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
					ImGui::TreeNodeEx(nice_name.c_str(), node_flags);
				}
			}

			if (ImGui::IsItemClicked(0))
			{
				selected = path.path();
			}

			// todo
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("file", NULL, 0);
				ImGui::Text(nice_name.c_str());
				ImGui::EndDragDropSource();
			}
		}
	}
}
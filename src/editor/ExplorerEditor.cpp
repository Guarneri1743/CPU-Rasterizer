#include "ExplorerEditor.hpp"
#include "Window.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include "Utility.hpp"
#include "Scene.hpp"
#include "Model.hpp"
#include "Logger.hpp"
#include "Serialization.hpp"
#include "EditorSharedData.hpp"

namespace CpuRasterizor
{
	ExplorerEditor::ExplorerEditor(int x, int y, int w, int h) : BaseEditor(x, y, w, h)
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
		rect = tinymath::Rect(0, Window::main()->get_height() - kBottomHeight, Window::main()->get_width(), kBottomHeight);
		EditorSharedData::explorer_selection_dirty = false;
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("ExplorerBar", tab_bar_flags))
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
			if (ImGui::BeginTabItem("Explorer"))
			{
				// explorer
				{
					ImGui::BeginChild("Explorer", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.25f, (float)rect.h()), true, window_flags);
					draw_directories(ASSETS_PATH.c_str());
					ImGui::EndChild();
				}

				ImGui::SameLine();

				// contents
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);
					ImGui::BeginChild("Content", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.75f, (float)rect.h()), true, window_flags);
					if (std::filesystem::is_directory(EditorSharedData::explorer_selection))
					{
						draw_files(EditorSharedData::explorer_selection);
					}
					ImGui::EndChild();
					ImGui::PopStyleVar();
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Console"))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);
				ImGui::BeginChild("Console", ImVec2(ImGui::GetWindowContentRegionWidth(), (float)rect.h()), true, window_flags);
				draw_console();
				ImGui::EndChild();
				ImGui::PopStyleVar();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	void ExplorerEditor::draw_files(const std::filesystem::path& dir)
	{
		for (auto& path : std::filesystem::directory_iterator(dir))
		{
			std::string full_name = path.path().generic_string();
			size_t last_slash = full_name.find_last_of("/\\");
			std::string nice_name = full_name;
			nice_name = nice_name.replace(nice_name.begin(), nice_name.begin() + last_slash + 1, "");

			ImGui::PushID(full_name.c_str());
			bool is_dir = path.is_directory();
			std::string label = get_prefix(path) + nice_name;
			if (!is_dir)
			{
				if (ImGui::Selectable(label.c_str(), EditorSharedData::content_selection == path.path(), ImGuiSelectableFlags_AllowItemOverlap))
				{
					EditorSharedData::content_selection = path.path().string();
				}

				if (ImGui::BeginPopupContextItem("RightClickPopup"))
				{
					if (path.path().has_extension())
					{
						std::string asset_path = ASSETS_PATH;
						size_t length = asset_path.length();
						std::string relative_path = path.path().generic_string();
						relative_path = relative_path.replace(relative_path.begin(), relative_path.begin() + length, "");
						std::string ext = path.path().extension().string();
						if (ext == ".scene")
						{
							if (ImGui::Selectable("Open")) { Scene::open_scene(relative_path.c_str()); }
						}
						else if (ext == ".model")
						{
							if (ImGui::Selectable("Add")) 
							{
								Model* deserialized_model = new Model();
								Serializer::deserialize(relative_path.c_str(), *deserialized_model);
								std::shared_ptr<Model> model = std::shared_ptr<Model>(deserialized_model);
								Scene::current()->add(model);
							}
						}
					}

					ImGui::EndPopup();
				}
			}
			else
			{
				if (ImGui::Selectable(label.c_str(), EditorSharedData::content_selection == path.path(), ImGuiSelectableFlags_AllowItemOverlap))
				{
					
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

	// todo: get the console an independent editor
	void ExplorerEditor::draw_console()
	{
		auto logs = Logger::get_console_logs();
		for (size_t idx = 0; idx < logs.size(); idx++)
		{
			if (ImGui::Selectable(logs[idx].c_str(), EditorSharedData::log_selection == idx))
			{
				EditorSharedData::log_selection = idx;
			}
		}
		draw_space(3);
	}

	std::size_t count_dirs(std::filesystem::path path)
	{
		using std::filesystem::directory_iterator;
		using fp = bool (*)(const std::filesystem::path&);
		return std::count_if(directory_iterator(path), directory_iterator{}, (fp)std::filesystem::is_directory);
	}

	std::string ExplorerEditor::get_prefix(const std::filesystem::path& path)
	{
		std::string prefix = "";
		if (std::filesystem::is_directory(path))
		{
			 prefix = "[folder]";
		}
		else
		{
			// todo: support icon
			if (path.has_extension())
			{
				std::string ext = path.extension().string();
				if (ext == ".scene")
				{
					prefix = "[scene]";
				}
				else if (ext == ".model")
				{
					prefix = "[model]";
				}
				else if (ext == ".material")
				{
					prefix = "[mat]";
				}
				else if (ext == ".texture")
				{
					prefix = "[tex]";
				}
				else if (ext == ".hdri")
				{
					prefix = "[hdri]";
				}
				else
				{
					prefix = "[other]";
				}
			}
		}

		return prefix;
	}

	void ExplorerEditor::draw_directories(const std::filesystem::path& dir)
	{
		for (auto& path : std::filesystem::directory_iterator(dir)) 
		{
			ImGuiTreeNodeFlags flags =
				ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_OpenOnDoubleClick |
				ImGuiTreeNodeFlags_SpanAvailWidth;

			std::string full_name = path.path().generic_string();
			size_t last_slash = full_name.find_last_of("/\\");
			std::string nice_name = full_name;
			nice_name = nice_name.replace(nice_name.begin(), nice_name.begin() + last_slash + 1, "");

			bool is_selected = path.path() == EditorSharedData::explorer_selection;
			if (is_selected)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}
			else
			{
				flags &= ~ImGuiTreeNodeFlags_Selected;
			}

			if (path.is_directory())
			{
				if (count_dirs(path) > 0)
				{
					bool clicked = ImGui::TreeNodeEx(nice_name.c_str(), flags);

					if (!EditorSharedData::explorer_selection_dirty && ImGui::IsItemClicked(0))
					{
						EditorSharedData::explorer_selection = path.path().string();
						EditorSharedData::explorer_selection_dirty = true;
					}

					if(clicked)
					{
						draw_directories(path.path());
						ImGui::TreePop();
					}
				}
				else
				{
					flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
					ImGui::TreeNodeEx(nice_name.c_str(), flags);

					if (!EditorSharedData::explorer_selection_dirty && ImGui::IsItemClicked(0))
					{
						EditorSharedData::explorer_selection = path.path().string();
						EditorSharedData::explorer_selection_dirty = true;
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
		}
	}
}
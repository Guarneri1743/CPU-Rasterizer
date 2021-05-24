#include "MainEditor.hpp"
#include "imgui/imgui.h"
#include "Marcos.h"
#include "InputManager.hpp"
#include "Window.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "Scene.hpp"
#include "Application.hpp"
#include "Utility.hpp"
#include <filesystem>

namespace Guarneri
{
	MainEditor::MainEditor() : BaseEditor()
	{
		no_titlebar = true;
		no_collapse = true;
		no_resize = true;
		no_close = true;
		no_move = true;
	}

	void MainEditor::DrawSceneMenu()
	{
		ImGui::MenuItem("(load scene)", NULL, false, false);

		if (ImGui::MenuItem("Open", "Ctrl+O"))
		{
			file_dialog_directory = ASSETS_PATH + "/scenes";
			menu_type = MenuType::kScene;
			show_file_dialog = true;
		}

		if (ImGui::MenuItem("Save", "Ctrl+S"))
		{
			Scene::serialize(*Scene::current(), Scene::current()->get_asset_path());
		}

		if (ImGui::MenuItem("Save As.."))
		{
			//todo
		}
	}

	void MainEditor::DrawModelMenu()
	{
		ImGui::MenuItem("(load model)", NULL, false, false);

		if (ImGui::MenuItem("Add", "Ctrl+O"))
		{
			file_dialog_directory = ASSETS_PATH + "/models"; 
			menu_type = MenuType::kModel;
			show_file_dialog = true;
		}

		if (ImGui::MenuItem("Save", "Ctrl+S"))
		{
			//todo
		}

		if (ImGui::MenuItem("Save As.."))
		{
			//todo
		}
	}

	void MainEditor::on_gui()
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)Window::main()->get_width(), kTopToolbarHeight));
		ImGuiWindowFlags flags = get_window_flag();
		flags |= ImGuiWindowFlags_NoBackground;
		if (!ImGui::Begin("Toolbar", no_close ? nullptr : &show, flags))
		{
			ImGui::End();
			return;
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Scene"))
			{
				DrawSceneMenu();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Model"))
			{
				DrawModelMenu();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (show_file_dialog)
		{
			std::string filename;
			if (DrawFileDialog(FileOp::Open, filename))
			{
				switch (menu_type)
				{
				case MenuType::kScene:
					Application::load_scene(filename.c_str());
					break;
				case MenuType::kModel:
					auto model = Model::load_asset(filename);
					Scene::current()->add(model);
					break;
				}
			}
		}

		ImGui::End();
	}
}
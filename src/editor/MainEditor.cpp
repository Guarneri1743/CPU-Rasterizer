#include "MainEditor.hpp"
#include "imgui/imgui.h"
#include "Marcos.h"
#include "InputManager.hpp"
#include "Window.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "Scene.hpp"
#include "Utility.hpp"
#include <filesystem>

namespace Guarneri
{
	MainEditor::MainEditor(float x, float y, float w, float h) : BaseEditor(x, y, w, h)
	{
		no_titlebar = true;
		no_collapse = true;
		no_resize = true;
		no_close = true;
		no_move = true;
		no_bring_to_front = true;
		title = "Main";
	}

	//todo
	void MainEditor::draw_file_menu()
	{
		//ImGui::MenuItem("()", NULL, false, false);

		if (ImGui::MenuItem("Import", "Ctrl+O"))
		{
		}

		if (ImGui::MenuItem("Save", "Ctrl+S"))
		{
			Scene::serialize(*Scene::current(), Scene::current()->get_asset_path());
		}

		if (ImGui::MenuItem("Save As.."))
		{
		}
	}

	void MainEditor::on_gui()
	{
		this->rect = Rect(0.0f, 0.0f, (float)Window::main()->get_width(), (float)Window::main()->get_height());
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				draw_file_menu();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}
}
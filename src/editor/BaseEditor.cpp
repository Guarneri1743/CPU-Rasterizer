#include "BaseEditor.hpp"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "Marcos.h"
#include "InputManager.hpp"
#include "Window.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "Application.hpp"
#include "Scene.hpp"
#include <filesystem>
#include <string>
#include "Utility.hpp"

#undef near
#undef far

namespace Guarneri
{
	bool BaseEditor::imgui_initialized = false;

	BaseEditor::BaseEditor() : show(true), show_file_dialog(false)
	{
		initialize_imgui();
	}

	BaseEditor::~BaseEditor()
	{}

	void BaseEditor::open()
	{
		show = true;
	}

	void BaseEditor::close()
	{
		show = false;
	}

	void BaseEditor::render()
	{
		if (!show) { return; }
		on_gui();
	}

	void BaseEditor::pre_render()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void BaseEditor::post_render()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	ImGuiWindowFlags BaseEditor::get_window_flag()
	{
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
		return window_flags;
	}

	void BaseEditor::initialize_imgui()
	{
		if (imgui_initialized) return;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::StyleColorsLight();
		ImGui_ImplGlfw_InitForOpenGL(Window::main()->get(), true);
		ImGui_ImplOpenGL3_Init(kGlslVersion);

		imgui_initialized = true;
	}

	bool BaseEditor::DrawFileDialog(FileOp op, std::string& filename)
	{
		UNUSED(op);
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();
		ImGui::SetNextWindowPos(ImVec2((float)(Window::main()->get_width() / 4), 128.0f));
		ImGui::SetNextWindowSize(ImVec2(512, 512));
		if (!ImGui::Begin("FileDialog", &show_file_dialog, 0))
		{
			ImGui::End();
			return false;
		}

		bool selected = false;
		for (auto& file : std::filesystem::directory_iterator(file_dialog_directory))
		{
			ImGui::Separator();
			if (ImGui::Selectable(file.path().filename().string().c_str()))
			{
				std::string file_path = file.path().generic_string();
				std::string asset_path = ASSETS_PATH;
				size_t length = asset_path.length();
				filename = file_path.replace(file_path.begin(), file_path.begin() + length, "");
				selected = true;
			}
		}

		ImGui::End();
		return selected;
	}
}
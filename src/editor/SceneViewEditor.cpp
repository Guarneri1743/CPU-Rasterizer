#include "SceneViewEditor.hpp"
#include "imgui/imgui.h"
#include "Window.hpp"

namespace Guarneri
{
	SceneViewEditor::SceneViewEditor(float x, float y, float w, float h) : BaseEditor(x, y, w, h)
	{
		no_scrollbar_with_mouse = true;
		no_scrollbar = true;
		no_titlebar = true;
		no_menu = true;
		no_collapse = true;
		no_resize = true;
		no_close = true;
		no_move = true;
		no_background = true;
		title = "SceneView";
	}

	void SceneViewEditor::on_gui()
	{
		rect = Rect((float)kLeftWidth, (float)kTopHeight, (float)Window::main()->get_width() - (float)kRightWidth - (float)kLeftWidth, (float)Window::main()->get_height() - (float)kTopHeight - (float)kBottomHeight);

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("SceneView", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("SceneView"))
			{
				ImTextureID fbo = reinterpret_cast<ImTextureID>(Window::main()->get_fbo());
				ImGui::Image(fbo, ImVec2(this->rect.w(), this->rect.h()), ImVec2(0, 1), ImVec2(1, 0));
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
}
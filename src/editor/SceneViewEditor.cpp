#include "SceneViewEditor.hpp"
#include "imgui/imgui.h"
#include "Window.hpp"

namespace CpuRasterizor
{
	SceneViewEditor::SceneViewEditor(int x, int y, int w, int h) : BaseEditor(x, y, w, h)
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
		rect = tinymath::Rect(kLeftWidth, kTopHeight, (int)Window::main()->get_width() - kRightWidth - kLeftWidth, (int)Window::main()->get_height() - kTopHeight - kBottomHeight);

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("SceneView", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("SceneView"))
			{
#pragma warning(push)
#pragma warning(disable : 4312)
				ImGui::Image((ImTextureID)Window::main()->get_fbo(), ImVec2((float)this->rect.w(), (float)this->rect.h()), ImVec2(0, 1), ImVec2(1, 0));
#pragma warning(pop)
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
}
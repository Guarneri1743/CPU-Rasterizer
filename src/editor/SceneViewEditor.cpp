#include "SceneViewEditor.hpp"
#include "imgui/imgui.h"
#include "Window.hpp"

namespace Guarneri
{
	SceneViewEditor::SceneViewEditor(float x, float y, float w, float h) : BaseEditor(x, y, w, h)
	{
		no_collapse = true;
		no_resize = true;
		no_close = true;
		no_move = true;
		title = "SceneView";
	}

	void SceneViewEditor::on_gui()
	{
		rect = Rect((float)kLeftWidth, (float)kTopHeight, (float)Window::main()->get_width() - (float)kRightWidth - (float)kLeftWidth, (float)Window::main()->get_height() - (float)kTopHeight - (float)kBottomHeight);
		
		ImTextureID fbo = reinterpret_cast<ImTextureID>(Window::main()->get_fbo());
		ImGui::Image(fbo, ImVec2(this->rect.w(), this->rect.h()), ImVec2(0, 1), ImVec2(1, 0));
	}
}
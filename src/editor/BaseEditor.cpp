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
#include "Scene.hpp"
#include <filesystem>
#include <string>
#include "Utility.hpp"

#undef near
#undef far

namespace Guarneri
{
	bool BaseEditor::imgui_initialized = false;

	BaseEditor::BaseEditor(int x, int y, int w, int h) : rect(x, y, w, h), show(true), title("Editor")
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

		ImGui::SetNextWindowPos(ImVec2((float)rect.x(), (float)rect.y()));
		ImGui::SetNextWindowSize(ImVec2((float)rect.w(), (float)rect.h()));

		if (!ImGui::Begin(title, no_close ? nullptr : &show, get_window_flag()))
		{
			ImGui::End();
			return;
		}
		
		on_gui();

		ImGui::End();
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

	void BaseEditor::add_left(BaseEditor* editor)
	{
		if (editor != nullptr)
		{
			if (rect.x() > 0)
			{
				if (left.size() > 0)
				{
					left.back()->add_right(editor);
					editor->rect = tinymath::Rect(left.back()->rect.x() + left.back()->rect.w(), rect.y(), rect.x() - left.back()->rect.x(), rect.h());
				}
				else
				{
					editor->rect = tinymath::Rect(0, rect.y(), rect.x(), rect.h());
					left.push_back(editor);
				}
			}
		}
	}

	void BaseEditor::add_right(BaseEditor* editor)
	{
		if (editor != nullptr)
		{
			if (rect.x() < Window::main()->get_width())
			{
				if (right.size() > 0)
				{
					right.front()->add_left(editor);
					editor->rect = tinymath::Rect(rect.x() + rect.w(), rect.y(), right.front()->rect.x() - rect.x() - rect.w(), rect.h());
				}
				else
				{
					editor->rect = tinymath::Rect(rect.x() + rect.w(), rect.y(), Window::main()->get_width() - rect.x(), rect.h());
					right.push_back(editor);
				}
			}
		}
	}

	void BaseEditor::add_top(BaseEditor* editor)
	{
		if (editor != nullptr)
		{
			if (rect.y() > 0)
			{
				editor->rect = tinymath::Rect(rect.x(), 0, rect.w(), rect.y());
				top.push_back(editor);
			}
		}
	}

	void BaseEditor::add_bottom(BaseEditor* editor)
	{
		if (editor != nullptr)
		{
			if (rect.y() < Window::main()->get_height())
			{
				editor->rect = tinymath::Rect(rect.x(), rect.y(), rect.w(), Window::main()->get_height() - rect.y());
				bottom.push_back(editor);
			}
		}
	}

	ImGuiWindowFlags BaseEditor::get_window_flag()
	{
		ImGuiWindowFlags window_flags = 0;
		if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
		if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
		if (no_scrollbar_with_mouse) window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
		if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
		if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
		if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
		if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
		if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
		if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
		if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		return window_flags;
	}

	void BaseEditor::draw_space(const int& count)
	{
		if (count <= 0) return;
		int total = count;
		while (total--)
		{
			ImGui::Spacing();
		}
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

	void BaseEditor::on_pos_size_change(tinymath::Rect prev, tinymath::Rect cur)
	{
		int l = cur.x() - prev.x();
		int r = cur.x() + cur.w() - prev.x() - prev.w();
		int t = cur.y() - prev.y();
		int b = cur.y() + cur.h() - prev.y() - prev.h();

		for (auto& editor : left)
		{
			if (editor != nullptr)
			{
				editor->rect.center.x -= l;
				editor->rect.extents.x -= l / 2;
			}
		}

		for (auto& editor : right)
		{
			if (editor != nullptr)
			{
				editor->rect.center.x += r;
				editor->rect.extents.x += r / 2;
			}
		}

		for (auto& editor : top)
		{
			if (editor != nullptr)
			{
				editor->rect.center.y -= t;
				editor->rect.extents.y -= t / 2;
			}
		}

		for (auto& editor : bottom)
		{
			if (editor != nullptr)
			{
				editor->rect.center.y -= b;
				editor->rect.extents.y -= b / 2;
			}
		}
	}

	void BaseEditor::set_rect(int x, int y, int w, int h)
	{
		tinymath::Rect new_rect = tinymath::Rect(x, y, w, h);
		on_pos_size_change(rect, new_rect);
		rect = new_rect;
	}
}
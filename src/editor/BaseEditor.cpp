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

#undef near
#undef far

namespace Guarneri
{
	BaseEditor::BaseEditor() : show(true)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::StyleColorsLight();
		ImGui_ImplGlfw_InitForOpenGL(Window::main()->get(), true);
		ImGui_ImplOpenGL3_Init(kGlslVersion);
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
		if (!show)
		{
			return;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		on_gui();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
#include "HierarchyEditor.hpp"
#include "imgui/imgui.h"
#include "Marcos.h"
#include "InputManager.hpp"
#include "Window.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "Scene.hpp"
#include "Application.hpp"

namespace Guarneri
{
	HierarchyEditor::HierarchyEditor() : BaseEditor()
	{
		no_collapse = true;
		no_resize = true;
		no_close = true;
		no_move = true;
	}

	void DrawTransform(Transform* transform)
	{
		if (ImGui::CollapsingHeader(transform->name.c_str()))
		{
			for (size_t idx = 0; idx < transform->child_count(); idx++)
			{
				auto child = transform->access_child(idx);
				DrawTransform(child);
			}
		}
	}

	void HierarchyEditor::on_gui()
	{
		ImGui::SetNextWindowPos(ImVec2(0, kTopToolbarHeight));
		ImGui::SetNextWindowSize(ImVec2(kHierarchyWidth, Window::main()->get_height() - kTopToolbarHeight));
		if (!ImGui::Begin("Hierarchy", &show, get_window_flag()))
		{
			ImGui::End();
			return;
		}

		auto scene = Scene::current();

		for (auto& obj : scene->objects)
		{
			if (obj == nullptr || obj->target == nullptr || obj->target->name == "") continue;
			DrawTransform(obj->target->transform.get());
		}

		for (auto& obj : scene->transparent_objects)
		{
			if (obj == nullptr || obj->target == nullptr || obj->target->name == "") continue;
			DrawTransform(obj->target->transform.get());
		}

		ImGui::End();
	}
}

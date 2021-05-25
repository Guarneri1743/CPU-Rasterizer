#include "HierarchyEditor.hpp"
#include "imgui/imgui.h"
#include "Marcos.h"
#include "InputManager.hpp"
#include "Window.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "Scene.hpp"
#include "Application.hpp"
#include <iostream>

namespace Guarneri
{
	HierarchyEditor::HierarchyEditor(float x, float y, float w, float h) : BaseEditor(x, y, w, h)
	{
		no_collapse = true;
		no_resize = true;
		no_close = true;
		no_move = true;
		title = "Hierarchy";
	}

	void DrawTransform(Transform* transform)
	{
		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth;

		if (transform == Scene::current()->selection)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (ImGui::TreeNodeEx(transform->name.c_str(), flags))
		{
			for (int i = 0; i < transform->child_count(); i++)
			{
				auto child = transform->access_child(i);
				DrawTransform(child);
			}
			ImGui::TreePop();
		}

		if (ImGui::IsItemClicked())
		{
			Scene::current()->selection = transform;
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("transform", NULL, 0);
			ImGui::Text(transform->name.c_str());
			ImGui::EndDragDropSource();
		}
	}

	void HierarchyEditor::on_gui()
	{
		rect = Rect(0.0f, (float)kTopHeight, (float)kLeftWidth, (float)Window::main()->get_height() - (float)kTopHeight - (float)kBottomHeight);
		
		auto scene = Scene::current();

		for (size_t idx = 0; idx < scene->objects.size(); idx++)
		{
			auto obj = scene->objects[idx];
			if (obj == nullptr || obj->target == nullptr || obj->target->name == "") continue;
			DrawTransform(obj->target->transform.get());
		}

		for (size_t idx = 0; idx < scene->transparent_objects.size(); idx++)
		{
			auto obj = scene->transparent_objects[idx];
			if (obj == nullptr || obj->target == nullptr || obj->target->name == "") continue;
			DrawTransform(obj->target->transform.get());
		}
	}
}

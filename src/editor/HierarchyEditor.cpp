#include "HierarchyEditor.hpp"
#include <iostream>
#include "imgui/imgui.h"
#include "Define.hpp"
#include "InputManager.hpp"
#include "Window.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "Scene.hpp"
#include "Model.hpp"
#include "Renderer.hpp"
#include "tinymath/primitives/Rect.h"


namespace CpuRasterizor
{
	HierarchyEditor::HierarchyEditor(int x, int y, int w, int h) : BaseEditor(x, y, w, h)
	{
		no_scrollbar_with_mouse = true;
		no_scrollbar = true;
		no_titlebar = true;
		no_menu = true;
		no_collapse = true;
		no_resize = true;
		no_close = true;
		no_move = true;
		title = "Hierarchy";
	}

	void HierarchyEditor::draw_transform(Transform* transform)
	{
		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth;

		if (transform == Scene::current()->selection)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (transform->child_count() > 0)
		{
			if (ImGui::TreeNodeEx(transform->name.c_str(), flags))
			{
				for (int i = 0; i < transform->child_count(); i++)
				{
					auto child = transform->access_child(i);
					draw_transform(child);
				}
				ImGui::TreePop();
			}
		}
		else
		{
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGui::TreeNodeEx(transform->name.c_str(), flags);
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

	void HierarchyEditor::draw_scene_hierarchy()
	{
		if (Scene::current() == nullptr) return;

		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth;

		auto scene = Scene::current();

		if (ImGui::TreeNodeEx(scene->name.c_str(), flags))
		{
			for (size_t idx = 0; idx < scene->objects.size(); idx++)
			{
				auto obj = scene->objects[idx];
				if (obj == nullptr || obj->target == nullptr || obj->target->name == "") continue;
				draw_transform(obj->target->transform.get());
			}

			for (size_t idx = 0; idx < scene->transparent_objects.size(); idx++)
			{
				auto obj = scene->transparent_objects[idx];
				if (obj == nullptr || obj->target == nullptr || obj->target->name == "") continue;
				draw_transform(obj->target->transform.get());
			}

			ImGui::TreePop();
		}
	}

	void HierarchyEditor::on_gui()
	{
		rect = tinymath::Rect(0, kTopHeight, kLeftWidth, Window::main()->get_height() - kTopHeight - kBottomHeight);

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("Hierarchy", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Hierarchy"))
			{
				draw_scene_hierarchy();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
}

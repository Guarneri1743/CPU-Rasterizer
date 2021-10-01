#pragma once
#include "BaseEditor.hpp"
#include "Rect.hpp"
#include "Transform.hpp"

namespace Guarneri
{
	class HierarchyEditor : public BaseEditor
	{
	public:
		HierarchyEditor(float x, float y, float w, float h);
		virtual void on_gui();
		void draw_transform(Transform* transform);
		void draw_scene_hierarchy();
	};
}
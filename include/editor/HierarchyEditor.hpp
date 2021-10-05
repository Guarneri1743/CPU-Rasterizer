#pragma once
#include "BaseEditor.hpp"
#include "Transform.hpp"

namespace CpuRasterizor
{
	class HierarchyEditor : public BaseEditor
	{
	public:
		HierarchyEditor(int x, int y, int w, int h);
		virtual void on_gui();
		void draw_transform(Transform* transform);
		void draw_scene_hierarchy();
	};
}
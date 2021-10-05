#pragma once
#include "BaseEditor.hpp"

namespace CpuRasterizor
{
	class SceneViewEditor : public BaseEditor
	{
	public:
		SceneViewEditor(int x, int y, int w, int h);
		virtual void on_gui();
	};
}
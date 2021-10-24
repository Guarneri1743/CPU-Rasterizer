#pragma once
#include "BaseEditor.hpp"

namespace CpuRasterizer
{
	class MainEditor : public BaseEditor
	{
	public:
		MainEditor(int x, int y, int w, int h);
		virtual void on_gui();

	private:
		void draw_file_menu();
	};
}
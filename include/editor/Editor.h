#ifndef _EDITOR_
#define _EDITOR_
#include "imgui/imgui.h"
#include <Windows.h>

namespace Guarneri
{
	class Editor
	{
	public:
		static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		Editor();
		~Editor();
		void initialize(HWND window, WNDCLASS window_class);
		void dispose();
		void draw_editor_gui();
		void render();

	private:
		bool disposed = false;
	};
}

#endif
#ifndef _GDI_WINDOW_
#define _GDI_WINDOW_
#include <Windows.h>
#include <tchar.h>

namespace Guarneri
{
	static LRESULT event_callback(HWND, UINT, WPARAM, LPARAM);
	static bool closed;

	class GDIWindow
	{
	public:
		void* framebuffer;
		int width;
		int height;
		float aspect;
		LPCSTR title;
		LPCSTR name;
		bool initialized;
		int buffer_size;
		HWND window_handle;
		WNDCLASS win_class;
		HDC window_device_context;
		HBITMAP bitmap_handle;
		HBITMAP original_handle;
		int text_start = 16;

	public:
		void initialize(int w, int h, LPCSTR title_str, LRESULT(*event_callback)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam));
		void set_title(LPCSTR _title);
		void draw_text(const int& w, const int& h, LPCSTR text);
		bool is_valid();
		void flush();
		void get_mouse_position(float& x, float& y, int& xi, int& yi);
		RECT get_rect();
		void dispose();
		void send_message();
	};
}
#endif
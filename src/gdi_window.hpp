#pragma once
#include <common.hpp>
#include <windows.h>
#include <tchar.h>

#ifdef _MSC_VER
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#endif

namespace guarneri {
	class gdi_window;
	static LRESULT event_callback(HWND, UINT, WPARAM, LPARAM);
	static int keys[512];
	static bool closed;
	#define IS_ON(key_code) keys[key_code] == 1

	class gdi_window {
	public:
		gdi_window(int w, int h, LPCSTR title, LPCSTR name) {
			this->width = w;
			this->height = h;
			this->title = title;
			this->name = name;
			buffer_size = width * height * sizeof(color_rgba);
			window_handle = nullptr;
			window_device_context = nullptr;
			framebuffer = nullptr;
			bitmap_handle = nullptr;
			initialized = false;
			closed = false;
			std::fill(std::begin(keys), std::end(keys), 0);
			initialize();
		}

		~gdi_window() {
			if (original_handle) {
				SelectObject(window_device_context, original_handle);
				original_handle = nullptr;
			}
			if (window_device_context) {
				DeleteDC(window_device_context);
				window_device_context = nullptr;
			}
			if (bitmap_handle) {
				DeleteObject(bitmap_handle);
				bitmap_handle = nullptr;
			}
			if (window_handle) {
				CloseWindow(window_handle);
				window_handle = nullptr;
			}
		}

	public:
		void* framebuffer;

	private:
		LPCSTR title;
		LPCSTR name;
		bool initialized;
		int width;
		int height;
		int buffer_size;
		HWND window_handle;
		HDC window_device_context;
		HBITMAP bitmap_handle;
		HBITMAP original_handle;

	public:
		void initialize() {
			if (initialized) {
				return;
			}

			WNDCLASS win_class;
			win_class.style = CS_BYTEALIGNCLIENT;
			win_class.lpfnWndProc = (WNDPROC)event_callback;
			win_class.cbClsExtra = 0;
			win_class.cbWndExtra = 0;
			win_class.hInstance = GetModuleHandle(nullptr);
			win_class.hIcon = nullptr;
			win_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
			win_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
			win_class.lpszMenuName = title;
			win_class.lpszClassName = name;

			BITMAPINFO bitmap_info;
			BITMAPINFOHEADER bitmap_header;
			bitmap_header.biSize = sizeof(BITMAPINFOHEADER);
			bitmap_header.biWidth = width;
			bitmap_header.biHeight = -height;
			bitmap_header.biPlanes = 1;
			bitmap_header.biBitCount  = 32;
			bitmap_header.biCompression = BI_RGB;
			bitmap_header.biSizeImage = buffer_size;
			bitmap_header.biXPelsPerMeter = 0;
			bitmap_header.biYPelsPerMeter = 0;
			bitmap_header.biClrUsed = 0;
			bitmap_header.biClrImportant = 0;
			bitmap_info.bmiHeader = bitmap_header;

			RegisterClass(&win_class);
			window_handle = CreateWindow(name, title, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, 0, 0, nullptr, nullptr, win_class.hInstance, nullptr);
			HDC hDC = GetDC(window_handle);
			window_device_context = CreateCompatibleDC(hDC);
			ReleaseDC(window_handle, hDC);

			LPVOID buffer;
			bitmap_handle = CreateDIBSection(window_device_context, &bitmap_info, DIB_RGB_COLORS, &buffer, 0, 0);
			if (bitmap_handle != nullptr) {
				original_handle = (HBITMAP)SelectObject(window_device_context, bitmap_handle);
			}
			framebuffer = (void*)buffer;
			memset(framebuffer, 0, buffer_size);
			RECT rect = { 0, 0, width, height };
			AdjustWindowRect(&rect, GetWindowLong(window_handle, GWL_STYLE), 0);
			int real_width = rect.right - rect.left;
			int real_height = rect.bottom - rect.top;
			int window_x = (GetSystemMetrics(SM_CXSCREEN) - real_width) / 2;
			int window_y = (GetSystemMetrics(SM_CYSCREEN) - real_height) / 2;
			SetWindowPos(window_handle, nullptr, window_x, window_y, real_width, real_height, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
			SetForegroundWindow(window_handle);
			ShowWindow(window_handle, SW_NORMAL);

			// window initialized
			initialized = true;
		}

		bool is_valid(){
			return keys[VK_ESCAPE] == 0 && !closed;
		}

		void flush() {
			HDC hDC = GetDC(window_handle);
			BitBlt(hDC, 0, 0, width, height, window_device_context, 0, 0, SRCCOPY);
			ReleaseDC(window_handle, hDC);
			send_message();
		}

		void send_message() {
			MSG msg;
			while (1) {
				if (!PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
					break;
				if (!GetMessage(&msg, nullptr, 0, 0))
					break;
				DispatchMessage(&msg);
			}
		}

		static LRESULT event_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			switch (msg) {
			case WM_CLOSE:
				closed = true;
				break;
			case WM_KEYDOWN:
				keys[wParam & 511] = 1;
				break;
			case WM_KEYUP:
				keys[wParam & 511] = 0;
				break;
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	};
}
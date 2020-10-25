#pragma once
#include <windows.h>
#include <tchar.h>

namespace guarneri {
	int screen_w, screen_h, screen_exit = 0;
	int screen_mx = 0, screen_my = 0, screen_mb = 0;
	int screen_keys[512];	// 当前键盘按下状态
	static HWND screen_handle = NULL;		// 主窗口 HWND
	static HDC screen_dc = NULL;			// 配套的 HDC
	static HBITMAP screen_hb = NULL;		// DIB
	static HBITMAP screen_ob = NULL;		// 老的 BITMAP
	unsigned char* screen_fb = NULL;		// frame buffer
	long screen_pitch = 0;

	int screen_init(int w, int h, const TCHAR* title);	// 屏幕初始化
	int screen_close(void);								// 关闭屏幕
	void screen_dispatch(void);							// 处理消息
	void screen_update(void);							// 显示 FrameBuffer

	// win32 event handler
	static LRESULT screen_events(HWND, UINT, WPARAM, LPARAM);

#ifdef _MSC_VER
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#endif

	// 初始化窗口并设置标题
	int screen_init(int w, int h, const TCHAR* title) {
		WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0,
			NULL, NULL, NULL, NULL, _T("SCREEN3.1415926") };
		BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
			w * h * 4, 0, 0, 0, 0 } };
		RECT rect = { 0, 0, w, h };
		int wx, wy, sx, sy;
		LPVOID ptr;
		HDC hDC;

		screen_close();

		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.hInstance = GetModuleHandle(NULL);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		if (!RegisterClass(&wc)) return -1;

		screen_handle = CreateWindow(_T("SCREEN3.1415926"), title,
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
		if (screen_handle == NULL) return -2;

		screen_exit = 0;
		hDC = GetDC(screen_handle);
		screen_dc = CreateCompatibleDC(hDC);
		ReleaseDC(screen_handle, hDC);

		screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
		if (screen_hb == NULL) return -3;

		screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);
		screen_fb = (unsigned char*)ptr;
		screen_w = w;
		screen_h = h;
		screen_pitch = w * 4;

		AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);
		wx = rect.right - rect.left;
		wy = rect.bottom - rect.top;
		sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
		sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
		if (sy < 0) sy = 0;
		SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
		SetForegroundWindow(screen_handle);

		ShowWindow(screen_handle, SW_NORMAL);
		screen_dispatch();

		memset(screen_keys, 0, sizeof(int) * 512);
		memset(screen_fb, 0, w * h * 4);

		return 0;
	}

	int screen_close(void) {
		if (screen_dc) {
			if (screen_ob) {
				SelectObject(screen_dc, screen_ob);
				screen_ob = NULL;
			}
			DeleteDC(screen_dc);
			screen_dc = NULL;
		}
		if (screen_hb) {
			DeleteObject(screen_hb);
			screen_hb = NULL;
		}
		if (screen_handle) {
			CloseWindow(screen_handle);
			screen_handle = NULL;
		}
		return 0;
	}

	static LRESULT screen_events(HWND hWnd, UINT msg,
		WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_CLOSE: screen_exit = 1; break;
		case WM_KEYDOWN: screen_keys[wParam & 511] = 1; break;
		case WM_KEYUP: screen_keys[wParam & 511] = 0; break;
		default: return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return 0;
	}

	void screen_dispatch(void) {
		MSG msg;
		while (1) {
			if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
			if (!GetMessage(&msg, NULL, 0, 0)) break;
			DispatchMessage(&msg);
		}
	}

	void screen_update(void) {
		HDC hDC = GetDC(screen_handle);
		BitBlt(hDC, 0, 0, screen_w, screen_h, screen_dc, 0, 0, SRCCOPY);
		ReleaseDC(screen_handle, hDC);
		screen_dispatch();
	}
}
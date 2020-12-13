#ifndef _GDI_WINDOW_
#define _GDI_WINDOW_

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


	void GDIWindow::initialize(int w, int h, LPCSTR title_str, LRESULT(*event_callback)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam))
	{
		if (initialized)
		{
			return;
		}
		this->width = w;
		this->height = h;
		this->aspect = (float)w / (float)h;
		this->title = title_str;
		this->name = title_str;
		buffer_size = width * height * 4;
		window_handle = nullptr;
		window_device_context = nullptr;
		framebuffer = nullptr;
		bitmap_handle = nullptr;
		closed = false;

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
		bitmap_header.biHeight = height;
		bitmap_header.biPlanes = 1;
		bitmap_header.biBitCount = 32;
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
		if (bitmap_handle != nullptr)
		{
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

	void  GDIWindow::set_title(LPCSTR _title)
	{
		SetWindowText(window_handle, _title);
	}

	void GDIWindow::draw_text(const int& w, const int& h, LPCSTR text)
	{
		RECT rect;
		rect.left = 1;
		rect.right = 1 + w;
		rect.bottom = text_start - h;
		rect.top = text_start;
		DrawText(window_device_context, text, -1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		text_start += h - 4;
	}

	bool GDIWindow::is_valid()
	{
		return !closed;
	}

	void GDIWindow::flush()
	{
		text_start = 16;
		HDC hDC = GetDC(window_handle);
		BitBlt(hDC, 0, 0, width, height, window_device_context, 0, 0, SRCCOPY);
		ReleaseDC(window_handle, hDC);
		send_message();
	}

	void GDIWindow::get_mouse_position(float& x, float& y, int& xi, int& yi)
	{
		POINT pt;
		if (GetCursorPos(&pt))
		{
			ScreenToClient(window_handle, &pt);
			xi = (int)pt.x;
			yi = (int)pt.y;
			x = (float)pt.x / (float)this->width;
			y = (float)pt.y / (float)this->height;
		}
	}

	RECT GDIWindow::get_rect()
	{
		RECT rect;
		if (GetWindowRect(window_handle, &rect))
		{
			return rect;
		}
		return rect;
	}

	void GDIWindow::dispose()
	{
		if (original_handle)
		{
			SelectObject(window_device_context, original_handle);
			original_handle = nullptr;
		}
		if (window_device_context)
		{
			DeleteDC(window_device_context);
			window_device_context = nullptr;
		}
		if (bitmap_handle)
		{
			DeleteObject(bitmap_handle);
			bitmap_handle = nullptr;
		}
		if (window_handle)
		{
			CloseWindow(window_handle);
			window_handle = nullptr;
		}
		closed = true;
	}

	void GDIWindow::send_message()
	{
		MSG msg;
		while (1)
		{
			if (!PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
				break;
			if (!GetMessage(&msg, nullptr, 0, 0))
				break;
			DispatchMessage(&msg);
		}
	}
}
#endif
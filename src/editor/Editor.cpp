#include "Editor.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "Marcos.h"
#include "InputManager.hpp"
#include "GDIWindow.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"


namespace Guarneri
{
   

    Editor::Editor()
    {}

    Editor::~Editor()
    {
        dispose();
    }

    void Editor::initialize(HWND handle, WNDCLASS wc)
    {

    }

    void Editor::dispose()
    {
        if (disposed)
        {
            return;
        }
       
        disposed = true;
    }

    void Editor::draw_editor_gui()
    {
        
    }

    void Editor::render()
    {
       
    }

    LRESULT Editor::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CLOSE:
            INST(GDIWindow).dispose();
            break;
        case WM_MOUSEMOVE:
            break;
        case WM_LBUTTONDOWN:
            INST(InputManager).on_vk_down(VK_LBUTTON);
            break;
        case WM_LBUTTONUP:
            INST(InputManager).on_vk_up(VK_LBUTTON);
            break;
        case WM_RBUTTONDOWN:
            INST(InputManager).on_vk_down(VK_RBUTTON);
            break;
        case WM_RBUTTONUP:
            INST(InputManager).on_vk_up(VK_RBUTTON);
            break;
        case WM_MBUTTONDOWN:
            INST(InputManager).on_vk_down(VK_MBUTTON);
            break;
        case WM_MBUTTONUP:
            INST(InputManager).on_vk_up(VK_MBUTTON);
            break;
        case WM_MOUSEWHEEL:
            INST(InputManager).on_wheel_rolling(GET_WHEEL_DELTA_WPARAM(wParam));
            break;
        case WM_KEYDOWN:
            INST(InputManager).on_vk_down(wParam);
            break;
        case WM_KEYUP:
            INST(InputManager).on_vk_up(wParam);
            break;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}
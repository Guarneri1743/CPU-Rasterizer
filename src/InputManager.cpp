#include "InputManager.hpp"
#include "Marcos.h"
#include "Singleton.hpp"
#include "GDIWindow.hpp"

namespace Guarneri
{
	LRESULT InputManager::event_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
			closed = true;
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

	void InputManager::update()
	{
		auto prev = mouse_position;
		float x, y;
		INST(GDIWindow).get_mouse_position(x, y, mouse_x, mouse_y);
		mouse_position = Vector2(x, y);
		if (Vector2::magnitude(prev - mouse_position) > EPSILON)
		{
			for (auto& kv : on_mouse_move_events)
			{
				auto evt = kv.first;
				auto user_data = kv.second;
				evt(prev, mouse_position, user_data);
			}
		}
		for (auto& kv : on_update_evts)
		{
			auto evt = kv.first;
			auto user_data = kv.second;
			evt(user_data);
		}
	}

	void InputManager::on_wheel_rolling(short delta)
	{
		for (auto& kv : on_wheel_rolling_events)
		{
			auto evt = kv.first;
			auto user_data = kv.second;
			evt(delta > 0 ? MouseWheel::UP : MouseWheel::DOWN, user_data);
		}
	}

	void InputManager::on_vk_down(WPARAM code)
	{
		if (vk2key.count(code) > 0)
		{
			auto key = vk2key.find(code)->second;
			active_keys.insert(key);
			for (auto& kv : on_key_down_events)
			{
				auto evt = kv.first;
				auto user_data = kv.second;
				evt(key, user_data);
			}
		}
		if (vk2mouse.count(code) > 0)
		{
			auto key = vk2mouse.find(code)->second;
			active_mouse_btns.insert(key);
			for (auto& kv : on_mouse_down_events)
			{
				auto evt = kv.first;
				auto user_data = kv.second;
				evt(key, user_data);
			}
		}
	}

	void InputManager::on_vk_up(WPARAM code)
	{
		if (vk2key.count(code) > 0)
		{
			auto key = vk2key.find(code)->second;
			active_keys.erase(key);
			for (auto& kv : on_key_up_events)
			{
				auto evt = kv.first;
				auto user_data = kv.second;
				evt(key, user_data);
			}
		}
		if (vk2mouse.count(code) > 0)
		{
			auto key = vk2mouse.find(code)->second;
			active_mouse_btns.erase(key);
			for (auto& kv : on_mouse_up_events)
			{
				auto evt = kv.first;
				auto user_data = kv.second;
				evt(key, user_data);
			}
		}
	}

	void InputManager::add_on_update_evt(void (*on_update)(void* ud), void* user_data)
	{
		if (on_update_evts.count(on_update) > 0)
		{
			return;
		}
		on_update_evts[on_update] = user_data;
	}

	void InputManager::remove_on_update_evt(void (*on_update)(void* ud))
	{
		on_update_evts.erase(on_update);
	}

	void InputManager::add_on_key_down_evt(void (*on_key_down)(KeyCode code, void* ud), void* user_data)
	{
		if (on_key_down_events.count(on_key_down) > 0)
		{
			return;
		}
		on_key_down_events.insert(std::pair(on_key_down, user_data));
	}

	void InputManager::remove_on_key_down_evt(void (*on_key_down)(KeyCode code, void* ud))
	{
		on_key_down_events.erase(on_key_down);
	}

	void InputManager::add_on_key_up_evt(void (*on_key_up)(KeyCode code, void* ud), void* user_data)
	{
		if (on_key_up_events.count(on_key_up) > 0)
		{
			return;
		}
		on_key_up_events.insert(std::pair(on_key_up, user_data));
	}

	void InputManager::remove_on_key_up_evt(void (*on_key_up)(KeyCode code, void* ud))
	{
		on_key_up_events.erase(on_key_up);
	}

	void InputManager::add_on_mouse_down_evt(void (*on_mouse_down)(MouseButton code, void* ud), void* user_data)
	{
		if (on_mouse_down_events.count(on_mouse_down) > 0)
		{
			return;
		}
		on_mouse_down_events.insert(std::pair(on_mouse_down, user_data));
	}

	void InputManager::remove_on_mouse_down_evt(void (*on_mouse_down)(MouseButton code, void* ud))
	{
		on_mouse_down_events.erase(on_mouse_down);
	}

	void InputManager::add_on_mouse_up_evt(void (*on_mouse_up)(MouseButton code, void* ud), void* user_data)
	{
		if (on_mouse_up_events.count(on_mouse_up) > 0)
		{
			return;
		}
		on_mouse_up_events.insert(std::pair(on_mouse_up, user_data));
	}

	void InputManager::remove_on_mouse_up_evt(void (*on_mouse_up)(MouseButton code, void* ud))
	{
		on_mouse_up_events.erase(on_mouse_up);
	}

	void InputManager::add_on_mouse_move_evt(void (*on_mouse_move)(Vector2 prev, Vector2 pos, void* ud), void* user_data)
	{
		if (on_mouse_move_events.count(on_mouse_move) > 0)
		{
			return;
		}
		on_mouse_move_events.insert(std::pair(on_mouse_move, user_data));
	}

	void InputManager::remove_on_mouse_move_evt(void (*on_mouse_move)(Vector2 prev, Vector2 pos, void* ud))
	{
		on_mouse_move_events.erase(on_mouse_move);
	}

	void InputManager::add_on_mouse_wheel_rolling_evt(void (*on_mouse_wheel_rolling)(MouseWheel rolling, void* ud), void* user_data)
	{
		if (on_wheel_rolling_events.count(on_mouse_wheel_rolling) > 0)
		{
			return;
		}
		on_wheel_rolling_events.insert(std::pair(on_mouse_wheel_rolling, user_data));
	}

	void InputManager::remove_on_mouse_wheel_rolling_evt(void (*on_mouse_wheel_rolling)(MouseWheel rolling, void* ud))
	{
		on_wheel_rolling_events.erase(on_mouse_wheel_rolling);
	}

	bool InputManager::is_key_down(KeyCode code)
	{
		return active_keys.count(code) > 0;
	}

	bool InputManager::is_mouse_down(MouseButton btn)
	{
		return active_mouse_btns.count(btn) > 0;
	}
}
#ifndef _INPUT_MGR_
#define _INPUT_MGR_
#include <unordered_map>
#include <unordered_set>

#include <windows.h>
#include <tchar.h>

#include "Marcos.h"
#include "Singleton.hpp"
#include "GDIWindow.hpp"
#include "Vector2.hpp"

namespace Guarneri
{
	enum class MouseButton
	{
		LEFT,
		RIGHT,
		MIDDLE
	};

	enum class MouseWheel
	{
		UP,
		DOWN
	};

	enum class KeyCode
	{
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		ESC,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		SPACE,
		CTRL_L,
		ALT_L,
		CTRL_R,
		ALT_R,
		SHIFT_L,
		SHIFT_R,
		ENTER,
		WIN
	};

	class InputManager
	{
		const std::unordered_map<WPARAM, KeyCode> vk2key =
		{
			{ 'A', KeyCode::A},
			{ 'B', KeyCode::B},
			{ 'C', KeyCode::C},
			{ 'D', KeyCode::D},
			{ 'E', KeyCode::E},
			{ 'F', KeyCode::F},
			{ 'G', KeyCode::G},
			{ 'H', KeyCode::H},
			{ 'I', KeyCode::I},
			{ 'J', KeyCode::J},
			{ 'K', KeyCode::K},
			{ 'L', KeyCode::L},
			{ 'M', KeyCode::M},
			{ 'N', KeyCode::N},
			{ 'O', KeyCode::O},
			{ 'P', KeyCode::P},
			{ 'Q', KeyCode::Q},
			{ 'R', KeyCode::R},
			{ 'S', KeyCode::S},
			{ 'T', KeyCode::T},
			{ 'U', KeyCode::U},
			{ 'V', KeyCode::V},
			{ 'W', KeyCode::W},
			{ 'X', KeyCode::X},
			{ 'Y', KeyCode::Y},
			{ 'Z', KeyCode::Z},
			{ VK_F1, KeyCode::F1},
			{ VK_F2, KeyCode::F2},
			{ VK_F3, KeyCode::F3},
			{ VK_F4, KeyCode::F4},
			{ VK_F5, KeyCode::F5},
			{ VK_F6, KeyCode::F6},
			{ VK_F7, KeyCode::F7},
			{ VK_F8, KeyCode::F8},
			{ VK_F9, KeyCode::F9},
			{ VK_F10, KeyCode::F10},
			{ VK_F11, KeyCode::F11},
			{ VK_F12, KeyCode::F12},
			{ VK_ESCAPE, KeyCode::ESC},
			{ VK_UP, KeyCode::UP},
			{ VK_DOWN, KeyCode::DOWN},
			{ VK_LEFT, KeyCode::LEFT},
			{ VK_RIGHT, KeyCode::RIGHT},
			{ VK_SPACE, KeyCode::SPACE},
			{ VK_LCONTROL, KeyCode::CTRL_L},
			{ VK_RCONTROL, KeyCode::CTRL_R},
			{ VK_LSHIFT, KeyCode::SHIFT_L},
			{ VK_RSHIFT, KeyCode::SHIFT_R},
			{ VK_RETURN, KeyCode::ENTER}
		};

		const std::unordered_map<WPARAM, MouseButton> vk2mouse =
		{
			{VK_LBUTTON, MouseButton::LEFT},
			{VK_RBUTTON, MouseButton::RIGHT},
			{VK_MBUTTON, MouseButton::MIDDLE}
		};

		std::unordered_set<KeyCode> active_keys;
		std::unordered_set<MouseButton> active_mouse_btns;

	public:
		Vector2 mouse_position;
		int mouse_x;
		int mouse_y;

	private:
		std::unordered_map<void (*)(Vector2 prev, Vector2 pos, void* ud), void*> on_mouse_move_events;
		std::unordered_map<void (*)(KeyCode code, void* ud), void*> on_key_down_events;
		std::unordered_map<void (*)(KeyCode code, void* ud), void*> on_key_up_events;
		std::unordered_map<void (*)(MouseButton code, void* ud), void*> on_mouse_down_events;
		std::unordered_map<void (*)(MouseButton code, void* ud), void*> on_mouse_up_events;
		std::unordered_map<void (*)(MouseWheel rolling, void* ud), void*> on_wheel_rolling_events;
		std::unordered_map<void (*)(void* user_data), void*> on_update_evts;

	public:
		static LRESULT event_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void update();
		void on_wheel_rolling(short delta);
		void on_vk_down(WPARAM code);
		void on_vk_up(WPARAM code);
		void add_on_update_evt(void (*on_update)(void* ud), void* user_data);
		void remove_on_update_evt(void (*on_update)(void* ud));
		void add_on_key_down_evt(void (*on_key_down)(KeyCode code, void* ud), void* user_data);
		void remove_on_key_down_evt(void (*on_key_down)(KeyCode code, void* ud));
		void add_on_key_up_evt(void (*on_key_up)(KeyCode code, void* ud), void* user_data);
		void remove_on_key_up_evt(void (*on_key_up)(KeyCode code, void* ud));
		void add_on_mouse_down_evt(void (*on_mouse_down)(MouseButton code, void* ud), void* user_data);
		void remove_on_mouse_down_evt(void (*on_mouse_down)(MouseButton code, void* ud));
		void add_on_mouse_up_evt(void (*on_mouse_up)(MouseButton code, void* ud), void* user_data);
		void remove_on_mouse_up_evt(void (*on_mouse_up)(MouseButton code, void* ud));
		void add_on_mouse_move_evt(void (*on_mouse_move)(Vector2 prev, Vector2 pos, void* ud), void* user_data);
		void remove_on_mouse_move_evt(void (*on_mouse_move)(Vector2 prev, Vector2 pos, void* ud));
		void add_on_mouse_wheel_rolling_evt(void (*on_mouse_wheel_rolling)(MouseWheel rolling, void* ud), void* user_data);
		void remove_on_mouse_wheel_rolling_evt(void (*on_mouse_wheel_rolling)(MouseWheel rolling, void* ud));
		bool is_key_down(KeyCode code);
		bool is_mouse_down(MouseButton btn);
	};

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
#endif
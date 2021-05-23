#include "InputManager.hpp"
#include "Marcos.h"
#include "Singleton.hpp"
#include "Window.hpp"
#include "GLFW/glfw3.h"

namespace Guarneri
{
	std::unordered_map<int, KeyCode> key_mapping =
	{
		{ GLFW_KEY_A, KeyCode::A},
		{ GLFW_KEY_B, KeyCode::B},
		{ GLFW_KEY_C, KeyCode::C},
		{ GLFW_KEY_D, KeyCode::D},
		{ GLFW_KEY_E, KeyCode::E},
		{ GLFW_KEY_F, KeyCode::F},
		{ GLFW_KEY_G, KeyCode::G},
		{ GLFW_KEY_H, KeyCode::H},
		{ GLFW_KEY_I, KeyCode::I},
		{ GLFW_KEY_J, KeyCode::J},
		{ GLFW_KEY_K, KeyCode::K},
		{ GLFW_KEY_L, KeyCode::L},
		{ GLFW_KEY_M, KeyCode::M},
		{ GLFW_KEY_N, KeyCode::N},
		{ GLFW_KEY_O, KeyCode::O},
		{ GLFW_KEY_P, KeyCode::P},
		{ GLFW_KEY_Q, KeyCode::Q},
		{ GLFW_KEY_R, KeyCode::R},
		{ GLFW_KEY_S, KeyCode::S},
		{ GLFW_KEY_T, KeyCode::T},
		{ GLFW_KEY_U, KeyCode::U},
		{ GLFW_KEY_V, KeyCode::V},
		{ GLFW_KEY_W, KeyCode::W},
		{ GLFW_KEY_X, KeyCode::X},
		{ GLFW_KEY_Y, KeyCode::Y},
		{ GLFW_KEY_Z, KeyCode::Z},
		{ GLFW_KEY_F1, KeyCode::F1},
		{ GLFW_KEY_F2, KeyCode::F2},
		{ GLFW_KEY_F3, KeyCode::F3},
		{ GLFW_KEY_F4, KeyCode::F4},
		{ GLFW_KEY_F5, KeyCode::F5},
		{ GLFW_KEY_F6, KeyCode::F6},
		{ GLFW_KEY_F7, KeyCode::F7},
		{ GLFW_KEY_F8, KeyCode::F8},
		{ GLFW_KEY_F9, KeyCode::F9},
		{ GLFW_KEY_F10, KeyCode::F10},
		{ GLFW_KEY_F11, KeyCode::F11},
		{ GLFW_KEY_F12, KeyCode::F12},
		{ GLFW_KEY_ESCAPE, KeyCode::ESC},
		{ GLFW_KEY_UP, KeyCode::UP},
		{ GLFW_KEY_DOWN, KeyCode::DOWN},
		{ GLFW_KEY_LEFT, KeyCode::LEFT},
		{ GLFW_KEY_RIGHT, KeyCode::RIGHT},
		{ GLFW_KEY_SPACE, KeyCode::SPACE},
		{ GLFW_KEY_LEFT_CONTROL, KeyCode::CTRL_L},
		{ GLFW_KEY_RIGHT_CONTROL, KeyCode::CTRL_R},
		{ GLFW_KEY_LEFT_SHIFT, KeyCode::SHIFT_L},
		{ GLFW_KEY_RIGHT_SHIFT, KeyCode::SHIFT_R},
		{ GLFW_KEY_ENTER, KeyCode::ENTER},
		{ GLFW_KEY_DELETE, KeyCode::DELETION}
	};

	std::unordered_map<WPARAM, MouseButton> mouse_mapping =
	{
		{ GLFW_MOUSE_BUTTON_LEFT, MouseButton::LEFT},
		{ GLFW_MOUSE_BUTTON_RIGHT, MouseButton::RIGHT},
		{ GLFW_MOUSE_BUTTON_MIDDLE, MouseButton::MIDDLE}
	};

	void InputManager::on_mouse_move(double x, double y)
	{
		auto prev = mouse_position;
		mouse_position = Vector2((float)x, (float)y);
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

	void InputManager::on_wheel_rolling(double delta)
	{
		for (auto& kv : on_wheel_rolling_events)
		{
			auto evt = kv.first;
			auto user_data = kv.second;
			evt(delta > 0 ? MouseWheel::UP : MouseWheel::DOWN, user_data);
		}
	}

	void InputManager::on_key_down(int code)
	{
		if (key_mapping.count(code) > 0)
		{
			auto key = key_mapping.find(code)->second;
			active_keys.insert(key);
			for (auto& kv : on_key_down_events)
			{
				auto evt = kv.first;
				auto user_data = kv.second;
				evt(key, user_data);
			}
		}
		if (mouse_mapping.count(code) > 0)
		{
			auto key = mouse_mapping.find(code)->second;
			active_mouse_btns.insert(key);
			for (auto& kv : on_mouse_down_events)
			{
				auto evt = kv.first;
				auto user_data = kv.second;
				evt(key, user_data);
			}
		}
	}

	void InputManager::on_key_up(int code)
	{
		if (key_mapping.count(code) > 0)
		{
			auto key = key_mapping.find(code)->second;
			active_keys.erase(key);
			for (auto& kv : on_key_up_events)
			{
				auto evt = kv.first;
				auto user_data = kv.second;
				evt(key, user_data);
			}
		}
		if (mouse_mapping.count(code) > 0)
		{
			auto key = mouse_mapping.find(code)->second;
			active_mouse_btns.erase(key);
			for (auto& kv : on_mouse_up_events)
			{
				auto evt = kv.first;
				auto user_data = kv.second;
				evt(key, user_data);
			}
		}
	}

	void InputManager::clear_evts()
	{
		on_mouse_move_events.clear();
		on_key_down_events.clear();
		on_key_up_events.clear();
		on_mouse_down_events.clear();
		on_mouse_up_events.clear();
		on_wheel_rolling_events.clear();
		on_update_evts.clear();
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
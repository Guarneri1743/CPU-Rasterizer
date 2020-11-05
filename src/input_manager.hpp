#pragma once
#include <guarneri.hpp>

namespace guarneri {
	enum class key_code {
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

	enum class mouse_button {
		left,
		right,
		middle
	};

	enum class mouse_wheel_rolling {
		up,
		down
	};

	class input_manager {
		const std::unordered_map<WPARAM, key_code> vk2key =
		{
			{ 'A', key_code::A},
			{ 'B', key_code::B},
			{ 'C', key_code::C},
			{ 'D', key_code::D},
			{ 'E', key_code::E},
			{ 'F', key_code::F},
			{ 'G', key_code::G},
			{ 'H', key_code::H},
			{ 'I', key_code::I},
			{ 'J', key_code::J},
			{ 'K', key_code::K},
			{ 'L', key_code::L},
			{ 'M', key_code::M},
			{ 'N', key_code::N},
			{ 'O', key_code::O},
			{ 'P', key_code::P},
			{ 'Q', key_code::Q},
			{ 'R', key_code::R},
			{ 'S', key_code::S},
			{ 'T', key_code::T},
			{ 'U', key_code::U},
			{ 'V', key_code::V},
			{ 'W', key_code::W},
			{ 'X', key_code::X},
			{ 'Y', key_code::Y},
			{ 'Z', key_code::Z},
			{ VK_F1, key_code::F1},
			{ VK_F2, key_code::F2},
			{ VK_F3, key_code::F3},
			{ VK_F4, key_code::F4},
			{ VK_F5, key_code::F5},
			{ VK_F6, key_code::F6},
			{ VK_F7, key_code::F7},
			{ VK_F8, key_code::F8},
			{ VK_F9, key_code::F9},
			{ VK_F10, key_code::F10},
			{ VK_F11, key_code::F11},
			{ VK_F12, key_code::F12},
			{ VK_ESCAPE, key_code::ESC},
			{ VK_UP, key_code::UP},
			{ VK_DOWN, key_code::DOWN},
			{ VK_LEFT, key_code::LEFT},
			{ VK_RIGHT, key_code::RIGHT},
			{ VK_SPACE, key_code::SPACE},
			{ VK_LCONTROL, key_code::CTRL_L},
			{ VK_RCONTROL, key_code::CTRL_R},
			{ VK_LSHIFT, key_code::SHIFT_L},
			{ VK_RSHIFT, key_code::SHIFT_R},
			{ VK_RETURN, key_code::ENTER}
		};

		const std::unordered_map<WPARAM, mouse_button> vk2mouse =
		{
			{VK_LBUTTON, mouse_button::left},
			{VK_RBUTTON, mouse_button::right},
			{VK_MBUTTON, mouse_button::middle}
		};

		std::unordered_set<key_code> active_keys;
		std::unordered_set<mouse_button> active_mouse_btns;

	public:
		float2 mouse_position;
		int mouse_x;
		int mouse_y;

	private:
		std::unordered_map<void (*)(float2 prev, float2 pos, void* ud), void*> on_mouse_move_events;
		std::unordered_map<void (*)(key_code code, void* ud), void*> on_key_down_events;
		std::unordered_map<void (*)(key_code code, void* ud), void*> on_key_up_events;
		std::unordered_map<void (*)(mouse_button code, void* ud), void*> on_mouse_down_events;
		std::unordered_map<void (*)(mouse_button code, void* ud), void*> on_mouse_up_events;
		std::unordered_map<void (*)(mouse_wheel_rolling rolling, void* ud), void*> on_wheel_rolling_events;

	public:
		static LRESULT event_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			switch (msg) {
			case WM_CLOSE:
				closed = true;
				break;
			case WM_MOUSEMOVE:
				break;
			case WM_LBUTTONDOWN:
				input_mgr().on_vk_down(VK_LBUTTON);
				break;
			case WM_LBUTTONUP:
				input_mgr().on_vk_up(VK_LBUTTON);
				break;
			case WM_RBUTTONDOWN:
				input_mgr().on_vk_down(VK_RBUTTON);
				break;
			case WM_RBUTTONUP:
				input_mgr().on_vk_up(VK_RBUTTON);
				break;
			case WM_MBUTTONDOWN:
				input_mgr().on_vk_down(VK_MBUTTON);
				break;
			case WM_MBUTTONUP:
				input_mgr().on_vk_up(VK_MBUTTON);
				break;
			case WM_MOUSEWHEEL:
				input_mgr().on_wheel_rolling(GET_WHEEL_DELTA_WPARAM(wParam));
				break;
			case WM_KEYDOWN:
				input_mgr().on_vk_down(wParam);
				break;
			case WM_KEYUP:
				input_mgr().on_vk_up(wParam);
				break;
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		void update() {
			auto prev = mouse_position;
			float x, y;
			window().get_mouse_position(x, y, mouse_x, mouse_y);
			mouse_position = float2(x, y);
			if (float2::magnitude(prev - mouse_position) > EPSILON) {
				for (auto& kv : on_mouse_move_events) {
					auto evt = kv.first;
					auto user_data = kv.second;
					evt(prev, mouse_position, user_data);
				}
			}
		}

		void on_wheel_rolling(short delta) {
			for (auto& kv : on_wheel_rolling_events) {
				auto evt = kv.first;
				auto user_data = kv.second;
				evt(delta > 0 ? mouse_wheel_rolling::up : mouse_wheel_rolling::down, user_data);
			}
		}

		void on_vk_down(WPARAM code) {
			if (vk2key.count(code) > 0) {
				auto key = vk2key.find(code)->second;
				active_keys.insert(key);
				for (auto& kv : on_key_down_events) {
					auto evt = kv.first;
					auto user_data = kv.second;
					evt(key, user_data);
				}
			}
			if (vk2mouse.count(code) > 0) {
				auto key = vk2mouse.find(code)->second;
				active_mouse_btns.insert(key);
				for (auto& kv : on_mouse_down_events) {
					auto evt = kv.first;
					auto user_data = kv.second;
					evt(key, user_data);
				}
			}
		}

		void on_vk_up(WPARAM code) {
			if (vk2key.count(code) > 0) {
				auto key = vk2key.find(code)->second;
				active_keys.erase(key);
				for (auto& kv : on_key_up_events) {
					auto evt = kv.first;
					auto user_data = kv.second;
					evt(key, user_data);
				}
			}
			if (vk2mouse.count(code) > 0) {
				auto key = vk2mouse.find(code)->second;
				active_mouse_btns.erase(key);
				for (auto& kv : on_mouse_up_events) {
					auto evt = kv.first;
					auto user_data = kv.second;
					evt(key, user_data);
				}
			}
		}

		void add_on_key_down_evt(void (*on_key_down)(key_code code, void* ud), void* user_data) {
			if (on_key_down_events.count(on_key_down) > 0) {
				return;
			}
			on_key_down_events.insert(std::pair(on_key_down, user_data));
		}

		void remove_on_key_down_evt(void (*on_key_down)(key_code code, void* ud), void* user_data) {
			on_key_down_events.erase(on_key_down);
		}

		void add_on_key_up_evt(void (*on_key_up)(key_code code, void* ud), void* user_data) {
			if (on_key_up_events.count(on_key_up) > 0) {
				return;
			}
			on_key_up_events.insert(std::pair(on_key_up, user_data));
		}

		void remove_on_key_up_evt(void (*on_key_up)(key_code code, void* ud), void* user_data) {
			on_key_up_events.erase(on_key_up);
		}

		void add_on_mouse_down_evt(void (*on_mouse_down)(mouse_button code, void* ud), void* user_data) {
			if (on_mouse_down_events.count(on_mouse_down) > 0) {
				return;
			}
			on_mouse_down_events.insert(std::pair(on_mouse_down, user_data));
		}

		void remove_on_mouse_down_evt(void (*on_mouse_down)(mouse_button code, void* ud), void* user_data) {
			on_mouse_down_events.erase(on_mouse_down);
		}

		void add_on_mouse_up_evt(void (*on_mouse_up)(mouse_button code, void* ud), void* user_data) {
			if (on_mouse_up_events.count(on_mouse_up) > 0) {
				return;
			}
			on_mouse_up_events.insert(std::pair(on_mouse_up, user_data));
		}

		void remove_on_mouse_up_evt(void (*on_mouse_up)(mouse_button code, void* ud), void* user_data) {
			on_mouse_up_events.erase(on_mouse_up);
		}

		void add_on_mouse_move_evt(void (*on_mouse_move)(float2 prev, float2 pos, void* ud), void* user_data) {
			if (on_mouse_move_events.count(on_mouse_move) > 0) {
				return;
			}
			on_mouse_move_events.insert(std::pair(on_mouse_move, user_data));
		}

		void remove_on_mouse_move_evt(void (*on_mouse_move)(float2 prev, float2 pos, void* ud), void* user_data) {
			on_mouse_move_events.erase(on_mouse_move);
		}

		void add_on_mouse_wheel_rolling_evt(void (*on_mouse_wheel_rolling)(mouse_wheel_rolling rolling, void* ud), void* user_data) {
			if (on_wheel_rolling_events.count(on_mouse_wheel_rolling) > 0) {
				return;
			}
			on_wheel_rolling_events.insert(std::pair(on_mouse_wheel_rolling, user_data));
		}

		void remove_on_mouse_wheel_rolling_evt(void (*on_mouse_wheel_rolling)(mouse_wheel_rolling rolling, void* ud), void* user_data) {
			on_wheel_rolling_events.erase(on_mouse_wheel_rolling);
		}

		bool is_key_down(key_code code) {
			return active_keys.count(code) > 0;
		}

		bool is_mouse_down(mouse_button btn) {
			return active_mouse_btns.count(btn) > 0;
		}
	};
}
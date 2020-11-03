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

	public:
		static LRESULT event_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			switch (msg) {
			case WM_CLOSE:
				closed = true;
				break;
			case WM_KEYDOWN:
				keys[wParam & 511] = 1;
				input_mgr().on_vk_down(wParam);
				break;
			case WM_KEYUP:
				keys[wParam & 511] = 0;
				input_mgr().on_vk_up(wParam);
				break;
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		void update() {
			tagPOINT pt;
			if (GetCursorPos(&pt)) {
				mouse_position.x = (float)pt.x / (float)window().width;
				mouse_position.y = (float)pt.y / (float)window().height;
			}
		}

		void on_vk_down(WPARAM code) {
			if (vk2key.count(code) > 0) {
				active_keys.insert(vk2key.find(code)->second);
			}
			else if (vk2mouse.count(code) > 0) {
				active_mouse_btns.insert(vk2mouse.find(code)->second);
			}
		}

		void on_vk_up(WPARAM code) {
			if (vk2key.count(code) > 0) {
				active_keys.erase(vk2key.find(code)->second);
			}
			else if (vk2mouse.count(code) > 0) {
				active_mouse_btns.erase(vk2mouse.find(code)->second);
			}
		}

		bool is_key_down(key_code code) {
			return active_keys.count(code) > 0;
		}

		bool is_mouse_down(mouse_button btn) {
			return active_mouse_btns.count(btn) > 0;
		}
	};
}
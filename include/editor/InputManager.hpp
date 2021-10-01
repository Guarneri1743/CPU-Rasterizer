#ifndef _INPUT_MGR_
#define _INPUT_MGR_
#include <unordered_map>
#include <unordered_set>
#include "tinymath.h"

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
		WIN,
		DELETION
	};

	class InputManager
	{
		std::unordered_set<KeyCode> active_keys;
		std::unordered_set<MouseButton> active_mouse_btns;

	public:
		tinymath::vec2f mouse_position;
		int mouse_x;
		int mouse_y;

	private:
		std::unordered_map<void (*)(tinymath::vec2f prev, tinymath::vec2f pos, void* ud), void*> on_mouse_move_events;
		std::unordered_map<void (*)(KeyCode code, void* ud), void*> on_key_down_events;
		std::unordered_map<void (*)(KeyCode code, void* ud), void*> on_key_up_events;
		std::unordered_map<void (*)(MouseButton code, void* ud), void*> on_mouse_down_events;
		std::unordered_map<void (*)(MouseButton code, void* ud), void*> on_mouse_up_events;
		std::unordered_map<void (*)(MouseWheel rolling, void* ud), void*> on_wheel_rolling_events;
		std::unordered_map<void (*)(void* user_data), void*> on_update_evts;

	public:
		void on_mouse_move(double x, double y);
		void on_wheel_rolling(double delta);
		void on_key_down(int code);
		void on_key_up(int code);
		void clear_evts();
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
		void add_on_mouse_move_evt(void (*on_mouse_move)(tinymath::vec2f prev, tinymath::vec2f pos, void* ud), void* user_data);
		void remove_on_mouse_move_evt(void (*on_mouse_move)(tinymath::vec2f prev, tinymath::vec2f pos, void* ud));
		void add_on_mouse_wheel_rolling_evt(void (*on_mouse_wheel_rolling)(MouseWheel rolling, void* ud), void* user_data);
		void remove_on_mouse_wheel_rolling_evt(void (*on_mouse_wheel_rolling)(MouseWheel rolling, void* ud));
		bool is_key_down(KeyCode code);
		bool is_mouse_down(MouseButton btn);
	};
}
#endif
#pragma once
#include "imgui/imgui.h"
#include <string>
#include <vector>
#include "tinymath/primitives/Rect.h"

namespace CpuRasterizer
{
	constexpr int kTopHeight = 19;
	constexpr int kLeftWidth = 256;
	constexpr int kRightWidth = 256;
	constexpr int kBottomHeight = 256;

	class BaseEditor
	{
	public:
		enum class FileOp
		{
			Open,
			Save,
			SaveAs
		};

		BaseEditor(int x, int y, int w, int h);
		virtual ~BaseEditor();
		void open();
		void close();
		void render();
		virtual void on_gui() = 0;
		static void pre_render();
		static void post_render();
		void add_left(BaseEditor* editor);
		void add_right(BaseEditor* editor);
		void add_top(BaseEditor* editor);
		void add_bottom(BaseEditor* editor);
		void on_pos_size_change(tinymath::Rect prev, tinymath::Rect cur);
		void set_rect(int x, int y, int w, int h);

	protected:
		bool show;
		ImGuiWindowFlags get_window_flag();
		void draw_space(int count);
		
		static void initialize_imgui();
		static bool imgui_initialized;

		std::vector<BaseEditor*> left;
		std::vector<BaseEditor*> right;
		std::vector<BaseEditor*> top;
		std::vector<BaseEditor*> bottom;

		tinymath::Rect rect;
		const char* title;

		bool no_titlebar = false;
		bool no_scrollbar = false;
		bool no_scrollbar_with_mouse = false;
		bool no_menu = false;
		bool no_move = false;
		bool no_resize = false;
		bool no_collapse = false;
		bool no_close = false;
		bool no_nav = false;
		bool no_background = false;
		bool no_bring_to_front = false;
	};
}
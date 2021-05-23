#ifndef _BASE_EDITOR_
#define _BASE_EDITOR_
#include "imgui/imgui.h"
#include <string>

namespace Guarneri
{
	class BaseEditor
	{
	public:
		enum class FileOp
		{
			Open,
			Save,
			SaveAs
		};

		BaseEditor();
		virtual ~BaseEditor();
		void open();
		void close();
		void render();
		virtual void on_gui() = 0;
		static void pre_render();
		static void post_render();

	protected:
		bool DrawFileDialog(FileOp op, std::string& filename);
		bool show_file_dialog;
		std::string file_dialog_directory;
		bool show;
		ImGuiWindowFlags get_window_flag();
		
		static void initialize_imgui();
		static bool imgui_initialized;

		bool no_titlebar = false;
		bool no_scrollbar = false;
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

#endif
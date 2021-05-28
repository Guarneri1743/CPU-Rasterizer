#ifndef _EXPLORER_EDITOR_
#define _EXPLORER_EDITOR_

#include "BaseEditor.hpp"
#include <filesystem>

namespace Guarneri
{
	class ExplorerEditor : public BaseEditor
	{
	public:
		ExplorerEditor(float x, float y, float w, float h);
		virtual void on_gui();

	private:
		void draw_directories(std::filesystem::path dir, std::filesystem::path& selected);
		void draw_files(std::filesystem::path dir, std::filesystem::path& selected);
		void draw_console();
		std::filesystem::path selected_in_explorer;
		std::filesystem::path selected_in_contents;
	};
}

#endif
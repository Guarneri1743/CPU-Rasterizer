#pragma once
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
		std::string get_prefix(const std::filesystem::path& path); // todo: support icon
		void draw_directories(const std::filesystem::path& dir);
		void draw_files(const std::filesystem::path& dir);
		void draw_console();
	};
}
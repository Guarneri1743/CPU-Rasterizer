#pragma once
#include <string>

namespace EditorSharedData
{
	static bool explorer_selection_dirty = false;
	static std::string explorer_selection = "";
	static std::string content_selection = "";
	static size_t log_selection = 0;
};
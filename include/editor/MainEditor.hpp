#ifndef _TOP_TOOL_BAR_EDITOR_
#define _TOP_TOOL_BAR_EDITOR_

#include "BaseEditor.hpp"

namespace Guarneri
{
	class MainEditor : public BaseEditor
	{
	public:
		MainEditor();
		virtual void on_gui();

	private:
		enum class MenuType
		{
			kNone = 0,
			kScene = 1,
			kModel = 2
		};
		MenuType menu_type = MenuType::kNone;
		void DrawSceneMenu();
		void DrawModelMenu();
	};
}

#endif
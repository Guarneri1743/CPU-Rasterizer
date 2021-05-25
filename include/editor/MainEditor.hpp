#ifndef _MAIN_EDITOR_
#define _MAIN_EDITOR_

#include "BaseEditor.hpp"

namespace Guarneri
{
	class MainEditor : public BaseEditor
	{
	public:
		MainEditor(float x, float y, float w, float h);
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
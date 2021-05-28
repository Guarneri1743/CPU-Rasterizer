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
		void draw_file_menu();
	};
}

#endif
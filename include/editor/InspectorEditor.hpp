#ifndef _SETTING_EDITOR_
#define _SETTING_EDITOR_

#include "BaseEditor.hpp"

namespace Guarneri
{
	class InspectorEditor : public BaseEditor
	{
	public:
		InspectorEditor(float x, float y, float w, float h);
		virtual void on_gui();
		void draw_inspector();
		void draw_settings();
	};
}

#endif
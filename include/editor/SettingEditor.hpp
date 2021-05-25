#ifndef _EDITOR_
#define _EDITOR_

#include "BaseEditor.hpp"

namespace Guarneri
{
	class SettingEditor : public BaseEditor
	{
	public:
		SettingEditor(float x, float y, float w, float h);
		virtual void on_gui();
	};
}

#endif
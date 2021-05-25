#ifndef _EDITOR_
#define _EDITOR_

#include "BaseEditor.hpp"

namespace Guarneri
{
	class SettingEditor : public BaseEditor
	{
	public:
		SettingEditor();
		virtual void on_gui();
	};
}

#endif
#ifndef _HIERARCHY_EDITOR_
#define _HIERARCHY_EDITOR_

#include "BaseEditor.hpp"

namespace Guarneri
{
	class HierarchyEditor : public BaseEditor
	{
	public:
		HierarchyEditor();
		virtual void on_gui();
	};
}

#endif
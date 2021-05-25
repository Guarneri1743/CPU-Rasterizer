#ifndef _HIERARCHY_EDITOR_
#define _HIERARCHY_EDITOR_

#include "BaseEditor.hpp"
#include "Rect.hpp"

namespace Guarneri
{
	class HierarchyEditor : public BaseEditor
	{
	public:
		HierarchyEditor(float x, float y, float w, float h);
		virtual void on_gui();
	};
}

#endif
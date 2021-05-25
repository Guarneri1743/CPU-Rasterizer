#ifndef _SCENE_VIEW_EDITOR_
#define _SCENE_VIEW_EDITOR_

#include "BaseEditor.hpp"

namespace Guarneri
{
	class SceneViewEditor : public BaseEditor
	{
	public:
		SceneViewEditor(float x, float y, float w, float h);
		virtual void on_gui();
	};
}

#endif
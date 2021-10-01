#pragma once
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
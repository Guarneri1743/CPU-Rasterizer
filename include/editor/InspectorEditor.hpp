#pragma once
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
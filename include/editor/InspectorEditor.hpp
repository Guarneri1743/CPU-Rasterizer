#pragma once
#include "BaseEditor.hpp"
#include "MaterialInspector.hpp"
#include "TransformInspector.hpp"
#include "LightInspector.hpp"

namespace Guarneri
{
	class InspectorEditor : public BaseEditor
	{
	public:
		InspectorEditor(float x, float y, float w, float h);
		virtual void on_gui();
		void draw_inspector();
		void draw_settings();

	private:
		MaterialInspector material_inspector;
		TransformInspector transform_inspector;
		LightInspector light_inspector;
	};
}
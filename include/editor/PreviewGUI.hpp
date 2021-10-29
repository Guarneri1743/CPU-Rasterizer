#pragma once
#include <string>
#include "CGL.h"
#include "Camera.hpp"
#include "Renderer.hpp"

namespace CpuRasterizer
{
	class PreviewGUI 
	{
	public:
		PreviewGUI(std::string material, int w, int h);
		virtual void on_gui(int index);

	private:
		uint32_t PreviewFBO;
		std::unique_ptr<Camera> previe_camera;
		std::unique_ptr<Renderer> renderer;
		cglResID rt_id;
		float width;
		float height;
	};
}
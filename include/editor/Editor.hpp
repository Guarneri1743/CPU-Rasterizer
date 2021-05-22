#ifndef _EDITOR_
#define _EDITOR_

#include "imgui/imgui.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace Guarneri
{
	class Editor
	{
	public:
		Editor(GLFWwindow* window, char* glsl_version);
		~Editor();
		void dispose();
		void render();

	private:
		bool disposed = false;
	};
}

#endif
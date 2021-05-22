#ifndef _WINDOW_
#define _WINDOW_

#include <functional>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace Guarneri
{
	constexpr char* kGlslVersion = "#version 130";

	class Window
	{
	public:
		Window(const char* title, int width, int height);
		~Window();

		GLFWwindow* get() const { return window; }

		void add_key_evt(std::function<void(int key, int scancode, int action, int mods)> callback);
		void add_cursor_pos_evt(std::function<void(double xpos, double ypos)> callback);
		void add_mouse_evt(std::function<void(int button, int action, int mods)> callback);
		void add_on_scroll_change_evt(std::function<void(double xoffset, double yoffset)> callback);
		void clear();
		void blit2screen(uint8_t* framebuffer);
		void flush();
		void close() { closed = true; }
		void get_cursor_pos(double& x, double& y) { x = cursor_x; y = cursor_y; }
		bool is_open();
		int get_width() { return width; }
		int get_height() { return height; }

	private:
		static void glfw_error_callback(int error, const char* description);
		static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void glfw_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
		static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

		uint32_t compile_blit_shader(std::string vert_path, std::string frag_path);

		GLFWwindow* window{ nullptr };
		double cursor_x;
		double cursor_y;
		int width;
		int height;
		bool valid;
		bool closed;

		uint32_t shader_id;
		uint32_t texture;
		uint32_t VBO;
		uint32_t VAO;
		uint32_t EBO;
	};
}

#endif
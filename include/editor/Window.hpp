#ifndef _WINDOW_
#define _WINDOW_

#include <functional>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace Guarneri
{
	constexpr char* kGlslVersion = "#version 130";
	constexpr int kHierarchyWidth = 320;
	constexpr int kSettingWidth = 320;
	constexpr int kTopToolbarHeight = 19;
	constexpr int kDefaultWindowWidth = 1920;
	constexpr int kDefaultWindowHeight = 1080;

	class Window
	{
	public:
		Window(const char* title, int width, int height);
		~Window();

		static void initialize_main_window(const char* title);
		static Window* main() { return main_window; };
		GLFWwindow* get() const { return window; }

		void clear();
		void blit2screen(uint8_t* framebuffer, uint32_t w, uint32_t h);
		void flush();
		void close() { closed = true; }
		void get_cursor_pos(double& x, double& y) { x = cursor_x; y = cursor_y; }
		bool is_open();
		int get_width() { return width; }
		int get_height() { return height; }
		int get_scene_view_width() { return width - kHierarchyWidth - kSettingWidth;}
		int get_scene_view_height() { return height - kTopToolbarHeight; }
		void add_on_resize_evt(void (*on_resize)(int w, int h, void* ud), void* user_data);


	private:
		static void glfw_error_callback(int error, const char* description);
		static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void glfw_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
		static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void glfw_resize_callback(GLFWwindow* window, int width, int height);

		uint32_t compile_blit_shader(std::string vert_path, std::string frag_path);

		std::unordered_map<void (*)(int w, int h, void* user_data), void*> on_resize_evts;

		GLFWwindow* window;
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

		static Window* main_window;
	};
}

#endif
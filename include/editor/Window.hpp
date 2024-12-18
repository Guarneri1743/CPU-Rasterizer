#pragma once
#include <functional>
#include <string>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace CpuRasterizer
{
	constexpr char* kGlslVersion = "#version 130";

	class Window
	{
	public:
		Window(const char* title, size_t width, size_t height);
		~Window();

		static void initialize_main_window(const char* title, size_t w, size_t h);
		static void initialize_main_window(const char* title);
		static Window* main() { return main_window; };
		GLFWwindow* get() const { return window; }

		void clear();
		void blit2screen(uint8_t* framebuffer, size_t w, size_t h, bool draw_directly);
		void swap_buffer();
		void close() { closed = true; }
		void get_cursor_pos(double& x, double& y) { x = cursor_x; y = cursor_y; }
		bool is_open();
		void add_on_resize_evt(void (*on_resize)(size_t w, size_t h, void* ud), void* user_data);
		uint32_t get_fbo() { return FBO; }

		size_t get_width() { return width; }
		size_t get_height() { return height; }

	private:
		static void glfw_error_callback(int error, const char* description);
		static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void glfw_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
		static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void glfw_resize_callback(GLFWwindow* window, int width, int height);

		uint32_t compile_blit_shader(std::string vert_path, std::string frag_path);

		std::unordered_map<void (*)(size_t w, size_t h, void* user_data), void*> on_resize_evts;

		GLFWwindow* window;
		double cursor_x;
		double cursor_y;
		bool valid;
		bool closed;

		size_t width;
		size_t height;

		uint32_t shader_id;
		uint32_t FBO;
		uint32_t VBO;
		uint32_t VAO;
		uint32_t EBO;

		static Window* main_window;
	};
}
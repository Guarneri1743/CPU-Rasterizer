#include "Window.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Utility.hpp"
#include "InputManager.hpp"
#include "Singleton.hpp"
#include "Logger.hpp"

namespace CpuRasterizor
{
	constexpr int kDefaultWindowWidth = 1920;
	constexpr int kDefaultWindowHeight = 1080;
	Window* Window::main_window;

	Window::Window(const char* title, int w, int h) :
		width(w),
		height(h),
		valid(false),
		closed(false),
		EBO(0),
		VAO(0),
		VBO(0),
		FBO(0),
		shader_id(0),
		cursor_x(0.0f),
		cursor_y(0.0f),
		window(nullptr)
	{
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
		{
			FATAL("create window failed.");
			return;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		//glfwWindowHint(GLFW_DECORATED, 0);
		glfwWindowHint(GLFW_RESIZABLE, 1);

		window = glfwCreateWindow(width, height, title, nullptr, nullptr);

		if (window == nullptr)
		{
			glfwTerminate();
			FATAL("create window failed.");
			return;
		}

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		if (gl3wInit() != 0)
		{
			FATAL("create window failed.");
			return;
		}

		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, glfw_key_callback);
		glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);
		glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
		glfwSetScrollCallback(window, glfw_scroll_callback);
		glfwSetFramebufferSizeCallback(window, glfw_resize_callback);

		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);

		float vertices[] = {
			// positions          // colors           // texture coords
			 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
			 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
			-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
			-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
		};

		unsigned int indices[] = {
		   0, 1, 3, // first triangle
		   1, 2, 3  // second triangle
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glGenTextures(1, &FBO);
		glBindTexture(GL_TEXTURE_2D, FBO);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		shader_id = compile_blit_shader("/shader/blit.vert", "/shader/blit.frag");

		glUseProgram(shader_id);
		glUniform1i(glGetUniformLocation(shader_id, "framebuffer"), 0);

		valid = true;
		LOG("window initialized.");
	}

	Window::~Window()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		valid = false;
	}

	void Window::initialize_main_window(const char* title, int w, int h)
	{
		if (main_window == nullptr)
		{
			main_window = new Window(title, w, h);
		}
	}

	void Window::initialize_main_window(const char* title)
	{
		initialize_main_window(title, kDefaultWindowWidth, kDefaultWindowHeight);
	}

	static void check_shader_error(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
				ERROR("shader program compile error: {}", infoLog);
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
				ERROR("shader program link error: {}", infoLog);
			}
		}
	}

	uint32_t Window::compile_blit_shader(std::string vert_path, std::string frag_path)
	{
		LOG("try compile blit shader, vertex: {}  fragment: {}", vert_path, frag_path);
		std::string vertexCode;
		std::string fragmentCode;

		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			vShaderFile.open(ASSETS_PATH + vert_path);
			fShaderFile.open(ASSETS_PATH + frag_path);

			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure& e)
		{
			ERROR("load shader failed. {}", e.what());
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vertex, fragment;

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);
		check_shader_error(vertex, "VERTEX");

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);
		check_shader_error(fragment, "FRAGMENT");

		uint32_t ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);

		glLinkProgram(ID);
		check_shader_error(ID, "PROGRAM");

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		return ID;
	}

	void Window::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void Window::blit2screen(uint8_t* framebuffer, size_t w, size_t h, bool draw_directly)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)w, (int)h, 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer);
		glBindTexture(GL_TEXTURE_2D, FBO);

		if (draw_directly)
		{
			glViewport(0, 0, (GLsizei)w, (GLsizei)h);
			glUseProgram(shader_id);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	void Window::flush()
	{
		glfwSwapBuffers(get());
		glfwPollEvents();
	}

	bool Window::is_open()
	{
		return !closed && valid && !glfwWindowShouldClose(get());
	}

	void Window::add_on_resize_evt(void(*on_resize)(int w, int h, void* ud), void* user_data)
	{
		if (on_resize_evts.count(on_resize) > 0)
		{
			return;
		}

		on_resize_evts.insert(std::pair(on_resize, user_data));
	}

	void Window::glfw_error_callback(const int error, const char* const description)
	{
		ERROR("glfw error: {}, desc: {}", error, description);
	}

	void Window::glfw_key_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods)
	{
		UNUSED(scancode);
		UNUSED(mods);
		UNUSED(window);
		if (action == GLFW_PRESS)
		{
			InputMgr.on_key_down(key);
		}
		else if (action == GLFW_RELEASE)
		{
			InputMgr.on_key_up(key);
		}
		else if (action == GLFW_REPEAT)
		{
			//todo
		}
	}

	void Window::glfw_cursor_pos_callback(GLFWwindow* window, const double x, const double y)
	{
		UNUSED(window);
		InputMgr.on_mouse_move(x, y);
	}

	void Window::glfw_mouse_button_callback(GLFWwindow* window, const int button, const int action, const int mods)
	{
		UNUSED(window);
		UNUSED(mods);
		if (action == GLFW_PRESS)
		{
			InputMgr.on_key_down(button);
		}
		else if (action == GLFW_RELEASE)
		{
			InputMgr.on_key_up(button);
		}
		else if (action == GLFW_REPEAT)
		{
			//todo
		}
	}

	void Window::glfw_scroll_callback(GLFWwindow* window, const double delta_x, const double delta_y)
	{
		UNUSED(window);
		UNUSED(delta_x);
		InputMgr.on_wheel_rolling(delta_y);
	}

	void Window::glfw_resize_callback(GLFWwindow* window, int w, int h)
	{
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		win->width = w;
		win->height = h;
		for (auto& kv : win->on_resize_evts)
		{
			auto evt = kv.first;
			auto user_data = kv.second;
			evt(w, h, user_data);
		}
	}
}
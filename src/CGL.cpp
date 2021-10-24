#include "CGL.h"
#include "tinymath.h"
#include "Define.hpp"
#include "GraphicsDevice.hpp"
#include "Shader.hpp"
#include "Window.hpp"

using namespace CpuRasterizer;


void cglInitWindow(const char* title, size_t w, size_t h)
{
	Window::initialize_main_window(title, w, h);
}

void cglGetMainWindowSize(size_t& w, size_t& h)
{
	w = Window::main()->get_width();
	h = Window::main()->get_height();
}

void cglAddResizeEvent(void(*on_resize)(size_t w, size_t h, void* ud), void* user_data)
{
	Window::main()->add_on_resize_evt(on_resize, user_data);
}

int cglIsMainWindowOpen()
{
	return Window::main()->is_open() ? 1 : 0;
}

void cglSwapBuffer(int draw_immediately)
{
	Window::main()->blit2screen(reinterpret_cast<uint8_t*>(CpuRasterDevice.get_target_color_buffer()), CpuRasterDevice.get_width(), CpuRasterDevice.get_height(), draw_immediately == 1 ? true : false);
	Window::main()->swap_buffer();
}

void cglCloseMainWindow()
{
	Window::main()->close();
}

void cglClearMainWindow()
{
	Window::main()->clear();
}

int cglGenId(uint32_t& id)
{
	int ret = CpuRasterDevice.try_alloc_id(id) ? 1 : 0;
	return ret;
}

void cglGetRTSize(size_t& w, size_t& h)
{
	CpuRasterDevice.get_active_rendertexture()->get_size(w, h);
}

void cglSetViewPort(size_t w, size_t h)
{
	CpuRasterDevice.set_viewport(w, h);
}

void cglSetClearColor(cglColor clear_color)
{
	CpuRasterDevice.set_clear_color(clear_color);
}

void cglClearBuffer(cglFrameContent content)
{
	CpuRasterDevice.clear_buffer(content);
}

void cglSubmitPrimitive(cglShader* shader, cglVert v1, cglVert v2, cglVert v3)
{
	CpuRasterDevice.submit_primitive(shader, v1, v2, v3);
}

void cglDrawSegment(cglVec3 start, cglVec3 end, cglMat4 mvp, cglColor col)
{
	CpuRasterDevice.draw_segment(start, end, col, mvp);
}

void cglFencePrimitives()
{
	CpuRasterDevice.fence_primitives();
}

void cglFencePixels()
{
	CpuRasterDevice.fence_pixels();
}

void cglSetSubSampleCount(uint8_t count)
{
	CpuRasterDevice.set_subsample_count(count);
}

uint8_t cglGetSubSampleCount()
{
	return CpuRasterDevice.get_subsample_count();
}

int cglSetMSAAEnabled(int enabled)
{
	return CpuRasterSharedData.enable_msaa = enabled == 1 ? true : false;
}

int cglIsMSAAEnabled()
{
	return CpuRasterSharedData.enable_msaa ? 1 : 0;
}


void cglSetMultisampleFrequency(cglMultisampleFrequency frequency)
{
	CpuRasterSharedData.multi_sample_frequency = frequency;
}
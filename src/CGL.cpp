#include "CGL.h"
#include "tinymath.h"
#include "Define.hpp"
#include "GraphicsDevice.hpp"
#include "Shader.hpp"
#include "Window.hpp"

using namespace CpuRasterizer;

constexpr int kMaxMipmap = 8;
constexpr int kMaxShaderNum = 128;
constexpr int kMaxTextureNum = 128;
static Shader* shader_array[kMaxShaderNum];
static std::shared_ptr<Texture> texture_array[kMaxTextureNum];
static uint32_t shader_stack_count = 0;
static uint32_t texture_stack_count = 0;
static uint32_t current_shader = 0;
static uint32_t current_texture = 0;

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

void cglEnable(cglRasterFlag flag)
{
	CpuRasterSharedData.raster_flag |= flag;
}

void cglDisable(cglRasterFlag flag)
{
	CpuRasterSharedData.raster_flag &= ~flag;
}

void cglCullFace(cglFaceCulling face)
{
	CpuRasterSharedData.face_culling = face;
}

void cglFrontFace(cglVertexOrder order)
{
	CpuRasterSharedData.vert_order = order;
}

CGL_EXTERN void cglDepthFunc(cglCompareFunc func)
{
	CpuRasterSharedData.ztest_func = func;
}

int cglGenId(uint32_t& id)
{
	int ret = CpuRasterDevice.try_alloc_id(id) ? 1 : 0;
	return ret;
}

bool cglGenTexture(uint32_t& id)
{
	if (texture_stack_count >= kMaxTextureNum)
	{
		return false;
	}
	
	id = texture_stack_count++;
	return true;
}

bool cglActivateTexture(uint32_t id)
{
	if (id < kMaxTextureNum)
	{
		current_texture = id;
		if (shader_stack_count > 0 && current_shader < shader_stack_count)
		{
			shader_array[current_shader]->local_properties.set_texture(id, texture_array[current_texture]);
			return true;
		}
	}

	return false;
}

void cglTexImage2D(uint32_t id, size_t width, size_t height, cglTextureFormat fmt, cglPointer data)
{
	if (texture_stack_count > 0 && id < texture_stack_count)
	{
		texture_array[id] = std::make_shared<Texture>(data, width, height, fmt);
	}
}

void cglTexImage3D(uint32_t id, size_t width, size_t height, size_t layer_count, cglTextureFormat fmt, cglPointer data)
{
	if (texture_stack_count > 0 && id < texture_stack_count)
	{
		texture_array[id] = std::make_shared<Texture>(data, width, height, layer_count, fmt);
	}
}

void cglGenerateMipmap()
{
	if (texture_stack_count > 0 && current_texture < texture_stack_count)
	{
		auto tex = texture_array[current_texture];
		if (tex != nullptr)
		{
			tex->generate_mipmap(kMaxMipmap);
		}
	}
}

bool cglCreateProgram(Shader* shader, uint32_t& id)
{
	if (shader_stack_count >= kMaxShaderNum)
		return false;
	shader_array[shader_stack_count] = shader;
	id = shader_stack_count++;
	return true;
}

bool cglUseProgram(uint32_t& id)
{
	if (shader_stack_count < kMaxShaderNum)
	{
		current_shader = id;
		return true;
	}

	return false;
}

void cglUniform1i(uint32_t id, property_name prop_id, int v)
{
	if (shader_stack_count > 0 && id < shader_stack_count)
	{
		shader_array[id]->local_properties.set_int(prop_id, v);
	}
}

void cglUniform1f(uint32_t id, property_name prop_id, float v)
{
	if (shader_stack_count > 0 && id < shader_stack_count)
	{
		shader_array[id]->local_properties.set_float(prop_id, v);
	}
}

void cglUniform4fv(uint32_t id, property_name prop_id, cglVec4 v)
{
	if (shader_stack_count > 0 && id < shader_stack_count)
	{
		shader_array[id]->local_properties.set_float4(prop_id, v);
	}
}

void cglUniformMatrix4fv(uint32_t id, property_name prop_id, cglMat4 mat)
{
	if (shader_stack_count > 0 && id < shader_stack_count)
	{
		shader_array[id]->local_properties.set_mat4x4(prop_id, mat);
	}
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

void cglSubmitPrimitive(cglVert v1, cglVert v2, cglVert v3)
{
	if (shader_stack_count > 0 && current_shader < shader_stack_count)
	{
		CpuRasterDevice.submit_primitive(shader_array[current_shader], v1, v2, v3);
	}
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
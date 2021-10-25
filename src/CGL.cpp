#include "CGL.h"
#include <unordered_map>
#include "Singleton.hpp"
#include "tinymath.h"
#include "Define.hpp"
#include "GraphicsDevice.hpp"
#include "ShaderProgram.hpp"
#include "RenderTexture.hpp"
#include "Window.hpp"
#include "IdAllocator.hpp"

using namespace CpuRasterizer;

IdAllocator id_allocator(1, kMaxID);
constexpr int kMaxMipmap = 8;
constexpr int kMaxShaderNum = 128;
constexpr int kMaxTextureNum = 128;

static std::unordered_map<uint32_t, ShaderProgram*> id2shader;
static std::unordered_map<uint32_t, TextureType> id2texturetype;

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

void cglEnable(cglPipelineFeature flag)
{
	CpuRasterDevice.enable_flag(flag);
}

void cglDisable(cglPipelineFeature flag)
{
	CpuRasterDevice.disable_flag(flag);
}

void cglCullFace(cglFaceCulling face)
{
	CpuRasterDevice.set_cull_face(face);
}

void cglFrontFace(cglVertexOrder order)
{
	CpuRasterDevice.set_front_face(order);
}

void cglDepthFunc(cglCompareFunc func)
{
	CpuRasterDevice.set_depth_func(func);
}

void cglSetBlendFunc(cglBlendFunc func)
{
	CpuRasterDevice.set_blend_func(func);
}

void cglSetStencilFunc(cglCompareFunc func)
{
	CpuRasterDevice.set_stencil_func(func);
}

void cglStencilMask(cglStencilValue ref_val, cglStencilValue write_mask, cglStencilValue read_mask)
{
	CpuRasterDevice.set_stencil_mask(ref_val, write_mask, read_mask);
}

void cglSetStencilOp(cglStencilOp pass_op, cglStencilOp fail_op, cglStencilOp zfail_op)
{
	CpuRasterDevice.set_stencil_op(pass_op, fail_op, zfail_op);
}

void cglSetBlendFactor(cglBlendFactor src_factor, cglBlendFactor dst_factor)
{
	CpuRasterDevice.set_blend_factor(src_factor, dst_factor);
}

void cglSetColorMask(cglColorMask mask)
{
	CpuRasterDevice.set_color_mask(mask);
}

int cglGenId(uint32_t& id)
{
	int ret = CpuRasterDevice.try_alloc_id(id) ? 1 : 0;
	return ret;
}

uint32_t cglCreateBuffer(size_t width, size_t height, cglFrameContent content)
{
	return CpuRasterDevice.create_buffer(width, height, content);
}

bool cglGenTexture(uint32_t& id)
{
	return id_allocator.alloc(id);
}

CGL_EXTERN void cglBindTexture(cglTextureType type, uint32_t id)
{
	// to be implement
	UNUSED(type);
	UNUSED(id);
}

bool cglActivateTexture(uint32_t id)
{
	// to be implement	
	UNUSED(id);
	return false;
}

void cglGenerateMipmap()
{
	// to be implement	
}

bool cglCreateProgram(ShaderProgram* shader, uint32_t& id)
{
	if (id_allocator.alloc(id))
	{
		id2shader[id] = shader;
		return true;
	}

	return false;
}

bool cglUseProgram(uint32_t id)
{
	if (id2shader.count(id) > 0)
	{
		current_shader = id;
		return true;
	}

	return false;
}

void cglUniform1i(uint32_t id, property_name prop_id, int v)
{
	if (id2shader.count(id) > 0)
	{
		id2shader[id]->local_properties.set_int(prop_id, v);
	}
}

void cglUniform1f(uint32_t id, property_name prop_id, float v)
{
	if (id2shader.count(id) > 0)
	{
		id2shader[id]->local_properties.set_float(prop_id, v);
	}
}

void cglUniform4fv(uint32_t id, property_name prop_id, cglVec4 v)
{
	if (id2shader.count(id) > 0)
	{
		id2shader[id]->local_properties.set_float4(prop_id, v);
	}
}

void cglUniformMatrix4fv(uint32_t id, property_name prop_id, cglMat4 mat)
{
	if (id2shader.count(id) > 0)
	{
		id2shader[id]->local_properties.set_mat4x4(prop_id, mat);
	}
}

void cglDrawCoordinates(const cglVec3& pos, const cglVec3& forward, const cglVec3& up, const cglVec3& right, const cglMat4& m, const cglMat4& v, const cglMat4& p)
{
	CpuRasterDevice.draw_coordinates(pos, forward, up, right, m, v, p);
}

void cglGetRTSize(size_t& w, size_t& h)
{
	CpuRasterDevice.get_active_rendertexture()->get_size(w, h);
}

void cglSetViewPort(size_t x, size_t y, size_t w, size_t h)
{
	CpuRasterDevice.set_viewport(x, y, w, h);
}

void cglGetViewport(size_t& x, size_t& y, size_t& width, size_t& height)
{
	x = 0;
	y = 0;
	width = CpuRasterDevice.get_width();
	height = CpuRasterDevice.get_height();
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
	if (id2shader.count(current_shader) > 0)
	{
		CpuRasterDevice.submit_primitive(id2shader[current_shader], v1, v2, v3);
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

void cglSetActiveRenderTarget(uint32_t id)
{
	CpuRasterDevice.set_active_rendertexture(id);
}

void cglResetActiveRenderTarget()
{
	CpuRasterDevice.reset_active_rendertexture();
}

void* cglGetTargetColorBuffer()
{
	return CpuRasterDevice.get_target_color_buffer();
}

void cglSetSubSampleCount(uint8_t count)
{
	CpuRasterDevice.set_subsample_count(count);
}

uint8_t cglGetSubSampleCount()
{
	return CpuRasterDevice.get_subsample_count();
}

void cglSetMultisampleFrequency(cglMultisampleFrequency frequency)
{
	CpuRasterDevice.set_multisample_frequency(frequency);
}
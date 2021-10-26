#include "CGL.h"
#include <unordered_map>
#include "Singleton.hpp"
#include "tinymath.h"
#include "Define.hpp"
#include "GraphicsDevice.hpp"
#include "ShaderProgram.hpp"
#include "RenderTexture.hpp"
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

cglResID cglCreateBuffer(size_t width, size_t height, cglFrameContent content)
{
	return CpuRasterDevice.create_buffer(width, height, content);
}

size_t cglBindVertexBuffer(const std::vector<cglVert>& buffer)
{
	return CpuRasterDevice.bind_vertex_buffer(buffer);
}

size_t cglBindIndexBuffer(const std::vector<size_t>& buffer)
{
	return CpuRasterDevice.bind_index_buffer(buffer);
}

void cglUseVertexBuffer(cglResID id)
{
	CpuRasterDevice.use_vertex_buffer(id);
}

void cglUseIndexBuffer(cglResID id)
{
	CpuRasterDevice.use_index_buffer(id);
}

void cglFreeVertexBuffer(cglResID id)
{
	CpuRasterDevice.delete_vertex_buffer(id);
}

void cglFreeIndexBuffer(cglResID id)
{
	CpuRasterDevice.delete_index_buffer(id);
}

void cglBindTexture(cglTextureType type, cglResID id)
{
	// to be implement
	UNUSED(type);
	UNUSED(id);
}

bool cglActivateTexture(cglResID id)
{
	// to be implement	
	UNUSED(id);
	return false;
}

void cglGenerateMipmap()
{
	// to be implement	
}

cglResID cglCreateProgram(cglShaderProgram shader)
{
	return CpuRasterDevice.create_shader_program(shader);
}

void cglUseProgram(cglResID id)
{
	CpuRasterDevice.use_program(id);
}

void cglUniform1i(cglResID id, cglPropertyName prop_id, int v)
{
	CpuRasterDevice.set_uniform_int(id, prop_id, v);
}

void cglUniform1f(cglResID id, cglPropertyName prop_id, float v)
{
	CpuRasterDevice.set_uniform_float(id, prop_id, v);
}

void cglUniform4fv(cglResID id, cglPropertyName prop_id, cglVec4 v)
{
	CpuRasterDevice.set_uniform_float4(id, prop_id, v);
}

void cglUniformMatrix4fv(cglResID id, cglPropertyName prop_id, cglMat4 mat)
{
	CpuRasterDevice.set_uniform_mat4x4(id, prop_id, mat);
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

void cglDrawPrimitive()
{
	CpuRasterDevice.draw_primitive();
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

void cglSetActiveRenderTarget(cglResID id)
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
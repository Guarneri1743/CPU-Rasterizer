# Tile Based CPU Rasterizer

### Usage

1. download [zip](https://github.com/Guarneri1743/CPU-Rasterizer/archive/main.zip) or clone [CPU-Rasterizer](https://github.com/Guarneri1743/CPU-Rasterizer)
2. run 'setup.bat' or type 'premake vs2017/vs2019'

##### Hello Triangle

Hello Triangle Sample:

	#include "GraphicsApi.h"
	#include "HelloTriangleShader.hpp"
	
	using namespace CpuRasterizor;
	
	int main()
	{
		// initialize window
		crWindowInit("Demo", 600, 400);
	
		// set viewport
		crApi.set_viewport(crMainWindow.get_width(), crMainWindow.get_height());
	
		// resize callback
		crMainWindow.add_on_resize_evt([](int w, int h, void* ud)
		{
			crApi.set_viewport(w, h);
		}, nullptr);
	
		HelloTriangleShader shader;
	
		// a triangle 
		crVert v1(crVec4(-0.5f, -0.5f, 0.0f, 1.0f), crVec3Zero, crVec2Zero);
		crVert v2(crVec4(0.5f, -0.5f, 0.0f, 1.0f), crVec3Zero, crVec2Zero);
		crVert v3(crVec4(0.0f, 0.5f, 0.0f, 1.0f), crVec3Zero, crVec2Zero);
	
		// setup shader properties
		shader.local_properties.set_mat4x4(mat_model, crMat4Identity);
		shader.local_properties.set_mat4x4(mat_view, crMat4Identity);
		shader.local_properties.set_mat4x4(mat_projection, crMat4Identity);
		shader.local_properties.set_mat4x4(mat_vp, crMat4Identity);
		shader.local_properties.set_mat4x4(mat_mvp, crMat4Identity);
	
		shader.double_face = true;
		shader.ztest_func = CompareFunc::kAlways;
		shader.transparent = false;
	
		// set background color
		crApi.set_clear_color(tinymath::kColorBlue);
	
		while (crMainWindow.is_open())
		{
			// clear window buffer
			crMainWindow.clear();
	
			// clear buffer
			crApi.clear_buffer(FrameContent::kColor | FrameContent::kDepth | FrameContent::kStencil);
	
			// submit a triangle to draw
			crApi.submit_draw_command(&shader, v1, v2, v3);
	
			// sync draw commands submitted
			crApi.fence_draw_commands();
	
			// render all content to render texture
			crApi.present();
	
			// blit render texture to screen
			crMainWindow.blit2screen(reinterpret_cast<uint8_t*>(crApi.get_target_color_buffer()), crApi.get_width(), crApi.get_height(), true);
	
			// swap front/back buffer
			crMainWindow.flush();
	
			Sleep(0);
		}
	
		crMainWindow.close();
	
		return 0;
	}



Hello Triangle Shader:

	#pragma once
	#include "Shader.hpp"
	
	using namespace CpuRasterizor;
	
	class HelloTriangleShader : public Shader
	{
	public:
		HelloTriangleShader() : Shader("sample_shader") {}
	
		v2f vertex_shader(const a2v& input) const
		{
			v2f o;
			auto opos = tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
			auto m = model();
			auto vp = vp_matrix();
			auto wpos = m * opos;
			auto cpos = vp * wpos;
			o.position = cpos;
			return o;
		}
	
		tinymath::Color fragment_shader(const v2f& input) const
		{
			UNUSED(input);
	
			// draw a white triangle
			return tinymath::kColorWhite;
		}
	};

Result:

<img src="/gallery/HelloTriangle.png" width = "256" height = "256" alt="overview" align=bottom /> 


Texture Mapping

	#include "GraphicsApi.h"
	#include "HelloTextureShader.hpp"
	
	using namespace CpuRasterizor;
	
	int main()
	{
		// initialize window
		crWindowInit("Demo", 600, 400);
	
		// set viewport
		crApi.set_viewport(Window::main()->get_width(), Window::main()->get_height());
	
		// resize callback
		crMainWindow.add_on_resize_evt([](int w, int h, void* ud)
		{
			crApi.set_viewport(w, h);
		}, nullptr);
	
		HelloTextureShader shader;
	
		// a triangle 
		crVert v1(crVec4(-0.5f, -0.5f, 0.0f, 1.0f), crVec3Zero, crVec2(0.0f, 0.0f));
		crVert v2(crVec4(0.5f, -0.5f, 0.0f, 1.0f), crVec3Zero, crVec2(1.0f, 0.0f));
		crVert v3(crVec4(0.0f, 0.5f, 0.0f, 1.0f), crVec3Zero, crVec2(0.5f, 1.0f));
	
		// setup shader properties
		shader.local_properties.set_mat4x4(mat_model, crMat4Identity);
		shader.local_properties.set_mat4x4(mat_view, crMat4Identity);
		shader.local_properties.set_mat4x4(mat_projection, crMat4Identity);
		shader.local_properties.set_mat4x4(mat_vp, crMat4Identity);
		shader.local_properties.set_mat4x4(mat_mvp, crMat4Identity);
	
		shader.double_face = true;
		shader.ztest_func = CompareFunc::kAlways;
		shader.transparent = false;
	
		auto tex = std::make_shared<Texture>(128, 128, TextureFormat::kRGB);
	
		// generate a checkerboard texture
		for (int r = 0; r< tex->width; r++)
		{
			for (int c = 0; c < tex->height; c++)
			{
				auto val = (((r & 0x8) == 0) ^ ((c & 0x8) == 0));
				tex->write((size_t)r, (size_t)c, { (float)val, (float)val, (float)val, 1.0f });
			}
		}
	
		property_name prop_id = 123;
		shader.local_properties.set_texture(prop_id, tex);
	
		// set background color
		crApi.set_clear_color(tinymath::kColorBlue);
	
		while (crMainWindow.is_open())
		{
			// clear window buffer
			crMainWindow.clear();
	
			// clear buffer
			crApi.clear_buffer(FrameContent::kColor | FrameContent::kDepth | FrameContent::kStencil);
	
			// submit a triangle to draw
			crApi.submit_draw_command(&shader, v1, v2, v3); 
	
			// sync draw commands submitted
			crApi.fence_draw_commands(); 
	
			// render all content to render texture
			crApi.present(); 
	
			// blit render texture to screen
			crMainWindow.blit2screen(reinterpret_cast<uint8_t*>(crApi.get_target_color_buffer()), crApi.get_width(), crApi.get_height(), true);
	
			// swap front/back buffer
			crMainWindow.flush();
	
			Sleep(0);
		}
	
		crMainWindow.close();
	
		return 0;
	}



Texture Mapping Shader:

	#pragma once
	#include "Shader.hpp"
	
	using namespace CpuRasterizor;
	
	class HelloTextureShader : public Shader
	{
	public:
		HelloTextureShader() : Shader("sample_shader") {}
	
		v2f vertex_shader(const a2v& input) const
		{
			v2f o;
			auto opos = tinymath::vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
			auto m = model();
			auto vp = vp_matrix();
			auto wpos = m * opos;
			auto cpos = vp * wpos;
			o.position = cpos;
			o.uv = input.uv;
			return o;
		}
	
		tinymath::Color fragment_shader(const v2f& input) const
		{
			UNUSED(input);
	
			// sample texture
			tinymath::Color c;
			if (local_properties.has_texture(123))
			{
				local_properties.get_texture(123)->sample(input.uv.x, input.uv.y, c);
			}
	
			return c;
		}
	};

Result:

<img src="/gallery/HelloTexture1.png" width = "256" height = "256" alt="overview" align=bottom /> 

And you can visualize the uv by:

	tinymath::Color fragment_shader(const v2f& input) const
	{
		return input.uv;
	}

Result:

<img src="/gallery/HelloTexture2.png" width = "256" height = "256" alt="overview" align=bottom /> 


### Viewer

<img src="/gallery/IBL.png" width = "512" height = "400" alt="overview" align=bottom /> 

<img src="/gallery/IBL2.png" width = "512" height = "400" alt="overview" align=bottom /> 

<img src="/gallery/IBL3.png" width = "512" height = "400" alt="overview" align=bottom /> 

### Gallery

| Physical based rendering | Physical based rendering | Image based lighting |
| --- | --- | --- |
|  <img src="/gallery/Roughness.png" width = "180" height = "140" alt="Roughness" align=bottom/> | <img src="/gallery/Metallic.png" width = "180" height = "140" alt="Metallic" align=bottom/> | <img src="/gallery/IBL_Comparison.png" width = "180" height = "140" alt="MSAA" align=bottom/>  |


| MSAA | Mipmap | Clipping | DebugView |
| ---  | --- | ---  |
| <img src="/gallery/msaa_on_4x.png" width = "180" height = "180" alt="MSAA" align=bottom/> | <img src="/gallery/mipmap.png" width = "180" height = "180" alt="Mipmap" align=bottom/> | <img src="/gallery/Clipping.png" width = "180" height = "180" alt="Clipping" align=bottom />  | <img src="/gallery/DebugView.png" width = "180" height = "180" alt="DebugView" align=bottom/> |

| Stencil test | Depth test | Blending |
| --- | --- | --- |
| <img src="/gallery/stencil_test.jpg" width = "180" height = "180" alt="Per Sample Operation" align=bottom/> | <img src="/gallery/z_buffer.jpg" width = "220" height = "180" alt="Per Sample Operation" align=bottom/> | <img src="/gallery/transparent_cube.jpg" width = "180" height = "180" alt="Blending" align=bottom/> |


| Point sampler | Bilinear sampler |
| --- | --- |
| <img src="/gallery/point_filtering.jpg" width = "180" height = "180" alt="Texture Filtering" align=bottom/> | <img src="/gallery/bilinear_filtering.jpg" width = "180" height = "180" alt="Texture Filtering" align=bottom/>  |


--- 

#### Features

- tile based

- fully parallel of triangle/pixel processing
			
- vertex/fragment shader

- cvv clipping

- frustum culling
			
- back-face culling

- msaa (pixel frequency and subsample frequency)

- perspective correct interpolation
			
- early-z
			
- alpha test
			
- stencil test
			
- depth test
			
- alpha blending

- segment drawer

- texture sampler

- cubemap sampler

---
		
#### Application

- input manager
			
- model importer

- resource serialization & deserialization

- hierarchical transform

- roaming camera

- material

- texture

- renderer
			
- cook-torrance brdf

- ibl baker

- ibl

- metallic workflow

- gamma/linear workflow

---

#### Third Parties
- [tinymath](https://github.com/Guarneri1743/tinymath) 
- assimp 
- stb_image 
- [rapidjson](https://github.com/Tencent/rapidjson) 
- imgui

---

#### Future Works

- low-level:
	
	- simd
				
	- gl/dx like api
		 
- features:

	- specular workflow
	
	- skeleton-animation
	
	- skinning
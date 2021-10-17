# CPU Rasterizer

A tile based cpu rasterizer

### Platform

- [x] Windows
- [ ] Linux
- [ ] MacOS

### Usage

1. download [zip](https://github.com/Guarneri1743/CPU-Rasterizer/archive/main.zip) or clone [CPU-Rasterizer](https://github.com/Guarneri1743/CPU-Rasterizer)
2. run 'setup.bat' or type 'premake vs2017/vs2019'

##### Hello Triangle

	#include "CGL.h"
	#include "HelloTriangleShader.hpp"
	
	int main()
	{
		// initialize window
		cglInitWindow("Demo", 600, 400);
	
		// set viewport
		size_t w, h;
		cglGetMainWindowSize(w, h);
		cglSetViewPort(w, h);
	
		// resize callback
		cglAddResizeEvent([](size_t w, size_t h, void* ud)
		{
			cglSetViewPort(w, h);
		}, nullptr);
	
		HelloTriangleShader shader;
	
		// a triangle 
		cglVert v1(cglVec4(-0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2Zero);
		cglVert v2(cglVec4(0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2Zero);
		cglVert v3(cglVec4(0.0f, 0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2Zero);
	
		// setup shader properties
		shader.local_properties.set_mat4x4(mat_model, cglMat4Identity);
		shader.local_properties.set_mat4x4(mat_view, cglMat4Identity);
		shader.local_properties.set_mat4x4(mat_projection, cglMat4Identity);
		shader.local_properties.set_mat4x4(mat_vp, cglMat4Identity);
		shader.local_properties.set_mat4x4(mat_mvp, cglMat4Identity);
	
		shader.double_face = true;
		shader.ztest_func = cglCompareFunc::kAlways;
		shader.transparent = false;
	
		// set background color
		cglSetClearColor(tinymath::kColorBlue);
	
		while (cglIsMainWindowOpen())
		{
			// clear window buffer
			cglClearMainWindow();
	
			// clear buffer
			cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);
	
			// submit primitive
			cglSubmitPrimitive(&shader, v1, v2, v3);
	
			// fence primitive tasks
			cglFencePrimitives();
	
			// fence pixel tasks
			cglFencePixels();
	
			cglSwapBuffer(true);
		}
	
		cglCloseMainWindow();
	
		return 0;
	}



Shader:

	#pragma once
	#include "Shader.hpp"
	
	using namespace CpuRasterizor;
	using namespace tinymath;
	
	class HelloTriangleShader : public Shader
	{
	public:
		HelloTriangleShader() : Shader("sample_shader") {}
	
		v2f vertex_shader(const a2v& input) const
		{
			v2f o;
			auto opos = vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
			auto m = model();
			auto vp = vp_matrix();
			auto wpos = m * opos;
			auto cpos = vp * wpos;
			o.position = cpos;
			return o;
		}
	
		Color fragment_shader(const v2f& input) const
		{
			UNUSED(input);
	
			// draw a white triangle
			return kColorWhite;
		}
	};

Result:

<img src="/gallery/HelloTriangle.png" width = "256" height = "200" alt="overview" align=bottom /> 


##### Texture Mapping

	#include "CGL.h"
	#include "HelloTextureShader.hpp"
	
	int main()
	{
		// initialize window
		cglInitWindow("Demo", 600, 400);
	
		// set viewport
		size_t w, h;
		cglGetMainWindowSize(w, h);
		cglSetViewPort(w, h);
	
		// resize callback
		cglAddResizeEvent([](size_t w, size_t h, void* ud)
		{
			cglSetViewPort(w, h);
		}, nullptr);
	
		HelloTextureShader shader;
	
		// a triangle 
		cglVert v1(cglVec4(-0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2(0.0f, 0.0f));
		cglVert v2(cglVec4(0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2(1.0f, 0.0f));
		cglVert v3(cglVec4(0.0f, 0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2(0.5f, 1.0f));
	
		// setup shader properties
		shader.local_properties.set_mat4x4(mat_model, cglMat4Identity);
		shader.local_properties.set_mat4x4(mat_view, cglMat4Identity);
		shader.local_properties.set_mat4x4(mat_projection, cglMat4Identity);
		shader.local_properties.set_mat4x4(mat_vp, cglMat4Identity);
		shader.local_properties.set_mat4x4(mat_mvp, cglMat4Identity);
	
		shader.double_face = true;
		shader.ztest_func = cglCompareFunc::kAlways;
		shader.transparent = false;
	
		auto tex = std::make_shared<Texture>(128, 128, cglTextureFormat::kRGB);
	
		// generate a checkerboard texture
		for (int r = 0; r< tex->width; r++)
		{
			for (int c = 0; c < tex->height; c++)
			{
				auto val = ((r & 0x8) == 0) ^ ((c & 0x8) == 0);
				tex->write((size_t)r, (size_t)c, { (float)val, (float)val, (float)val, 1.0f });
			}
		}
	
		property_name prop_id = 123;
		shader.local_properties.set_texture(prop_id, tex);
	
		// set background color
		cglSetClearColor(tinymath::kColorBlue);
	
		while (cglIsMainWindowOpen())
		{
			// clear window buffer
			cglClearMainWindow();
	
			// clear buffer
			cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);
	
			// submit primitive
			cglSubmitPrimitive(&shader, v1, v2, v3); 
	
			// fence primitive tasks
			cglFencePrimitives(); 
	
			// fence pixel tasks
			cglFencePixels(); 
	
			cglSwapBuffer(true);
		}
	
		cglCloseMainWindow();
	
		return 0;
	}




Shader:

	#pragma once
	#include "Shader.hpp"
	
	using namespace CpuRasterizor;
	using namespace tinymath;
	
	class HelloTextureShader : public Shader
	{
	public:
		HelloTextureShader() : Shader("sample_shader") {}
	
		v2f vertex_shader(const a2v& input) const
		{
			v2f o;
			auto opos = vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
			auto m = model();
			auto vp = vp_matrix();
			auto wpos = m * opos;
			auto cpos = vp * wpos;
			o.position = cpos;
			o.uv = input.uv;
			return o;
		}
	
		Color fragment_shader(const v2f& input) const
		{
			UNUSED(input);
	
			// sample texture
			Color c;
			if (local_properties.has_texture(123))
			{
				local_properties.get_texture(123)->sample(input.uv.x, input.uv.y, c);
			}
	
			return c;
		}
	};


Result:

<img src="/gallery/HelloTexture1.png" width = "256" height = "200" alt="overview" align=bottom /> 

UV can be visualized by:

	tinymath::Color fragment_shader(const v2f& input) const
	{
		return input.uv;
	}

Result:

<img src="/gallery/HelloTexture2.png" width = "256" height = "200" alt="overview" align=bottom /> 


##### Simple Lighting

	#include "CGL.h"
	#include "HelloLightingShader.hpp"
	
	cglVert cube_vertices[36] = {
			cglVert(cglVec4(-0.5f, -0.5f, -0.5f, 1.0f), cglVec3(0.0f,  0.0f, -1.0f), cglVec2(0.0f,  0.0f)),
			cglVert(cglVec4(0.5f, -0.5f, -0.5f, 1.0f), cglVec3(0.0f,  0.0f, -1.0f), cglVec2(1.0f,  0.0f)),
			cglVert(cglVec4(0.5f,  0.5f, -0.5f, 1.0f), cglVec3(0.0f,  0.0f, -1.0f), cglVec2(1.0f,  1.0f)),
			cglVert(cglVec4(0.5f,  0.5f, -0.5f, 1.0f), cglVec3(0.0f,  0.0f, -1.0f), cglVec2(1.0f,  1.0f)),
			cglVert(cglVec4(-0.5f,  0.5f, -0.5f, 1.0f), cglVec3(0.0f,  0.0f, -1.0f), cglVec2(0.0f,  1.0f)),
			cglVert(cglVec4(-0.5f, -0.5f, -0.5f, 1.0f), cglVec3(0.0f,  0.0f, -1.0f), cglVec2(0.0f,  0.0f)),
	
			cglVert(cglVec4(-0.5f, -0.5f,  0.5f, 1.0f), cglVec3(0.0f,  0.0f, 1.0f), cglVec2(0.0f,  0.0f)),
			cglVert(cglVec4(0.5f, -0.5f,  0.5f, 1.0f), cglVec3(0.0f,  0.0f, 1.0f), cglVec2(1.0f,  0.0f)),
			cglVert(cglVec4(0.5f,  0.5f,  0.5f, 1.0f), cglVec3(0.0f,  0.0f, 1.0f), cglVec2(1.0f,  1.0f)),
			cglVert(cglVec4(0.5f,  0.5f,  0.5f, 1.0f), cglVec3(0.0f,  0.0f, 1.0f), cglVec2(1.0f, 1.0f)),
			cglVert(cglVec4(-0.5f,  0.5f,  0.5f, 1.0f), cglVec3(0.0f,  0.0f, 1.0f), cglVec2(0.0f, 1.0f)),
			cglVert(cglVec4(-0.5f, -0.5f,  0.5f, 1.0f), cglVec3(0.0f,  0.0f, 1.0f), cglVec2(0.0f,  0.0f)),
	
			cglVert(cglVec4(-0.5f,  0.5f,  0.5f, 1.0f), cglVec3(-1.0f,  0.0f,  0.0f), cglVec2(1.0f,  0.0f)),
			cglVert(cglVec4(-0.5f,  0.5f, -0.5f, 1.0f), cglVec3(-1.0f,  0.0f,  0.0f), cglVec2(1.0f,  1.0f)),
			cglVert(cglVec4(-0.5f, -0.5f, -0.5f, 1.0f), cglVec3(-1.0f,  0.0f,  0.0f), cglVec2(0.0f,  1.0f)),
			cglVert(cglVec4(-0.5f, -0.5f, -0.5f, 1.0f), cglVec3(-1.0f,  0.0f,  0.0f), cglVec2(0.0f,  1.0f)),
			cglVert(cglVec4(-0.5f, -0.5f,  0.5f, 1.0f), cglVec3(-1.0f,  0.0f,  0.0f), cglVec2(0.0f,  0.0f)),
			cglVert(cglVec4(-0.5f,  0.5f,  0.5f, 1.0f), cglVec3(-1.0f,  0.0f,  0.0f), cglVec2(1.0f,  0.0f)),
	
			cglVert(cglVec4(0.5f,  0.5f,  0.5f, 1.0f), cglVec3(1.0f,  0.0f,  0.0f), cglVec2(1.0f,  0.0f)),
			cglVert(cglVec4(0.5f,  0.5f, -0.5f, 1.0f), cglVec3(1.0f,  0.0f,  0.0f), cglVec2(1.0f,  1.0f)),
			cglVert(cglVec4(0.5f, -0.5f, -0.5f, 1.0f), cglVec3(1.0f,  0.0f,  0.0f), cglVec2(0.0f,  1.0f)),
			cglVert(cglVec4(0.5f, -0.5f, -0.5f, 1.0f), cglVec3(1.0f,  0.0f,  0.0f), cglVec2(0.0f,  1.0f)),
			cglVert(cglVec4(0.5f, -0.5f,  0.5f, 1.0f), cglVec3(1.0f,  0.0f,  0.0f), cglVec2(0.0f,  0.0f)),
			cglVert(cglVec4(0.5f,  0.5f,  0.5f, 1.0f), cglVec3(1.0f,  0.0f,  0.0f), cglVec2(1.0f,  0.0f)),
	
			cglVert(cglVec4(-0.5f, -0.5f, -0.5f, 1.0f), cglVec3(0.0f, -1.0f,  0.0f), cglVec2(0.0f,  1.0f)),
			cglVert(cglVec4(0.5f, -0.5f, -0.5f, 1.0f), cglVec3(0.0f, -1.0f,  0.0f), cglVec2(1.0f,  1.0f)),
			cglVert(cglVec4(0.5f, -0.5f,  0.5f, 1.0f), cglVec3(0.0f, -1.0f,  0.0f), cglVec2(1.0f,  0.0f)),
			cglVert(cglVec4(0.5f, -0.5f,  0.5f, 1.0f), cglVec3(0.0f, -1.0f,  0.0f), cglVec2(1.0f,  0.0f)),
			cglVert(cglVec4(-0.5f, -0.5f,  0.5f, 1.0f), cglVec3(0.0f, -1.0f,  0.0f), cglVec2(0.0f,  0.0f)),
			cglVert(cglVec4(-0.5f, -0.5f, -0.5f, 1.0f), cglVec3(0.0f, -1.0f,  0.0f), cglVec2(0.0f,  1.0f)),
	
			cglVert(cglVec4(-0.5f,  0.5f, -0.5f, 1.0f), cglVec3(0.0f, 1.0f,  0.0f), cglVec2(0.0f,  1.0f)),
			cglVert(cglVec4(0.5f,  0.5f, -0.5f, 1.0f), cglVec3(0.0f, 1.0f,  0.0f), cglVec2(1.0f,  1.0f)),
			cglVert(cglVec4(0.5f,  0.5f,  0.5f, 1.0f), cglVec3(0.0f, 1.0f,  0.0f), cglVec2(1.0f,  0.0f)),
			cglVert(cglVec4(0.5f, 0.5f,  0.5f, 1.0f), cglVec3(0.0f, 1.0f,  0.0f), cglVec2(1.0f,  0.0f)),
			cglVert(cglVec4(-0.5f,  0.5f,  0.5f, 1.0f), cglVec3(0.0f, 1.0f,  0.0f), cglVec2(0.0f,  0.0f)),
			cglVert(cglVec4(-0.5f,  0.5f, -0.5f, 1.0f), cglVec3(0.0f, 1.0f,  0.0f), cglVec2(0.0f,  1.0f)),
	};
	
	HelloLightingShader shader;
	
	void draw_cube()
	{
		for (int i = 0; i < 36; i+= 3)
		{
			// submit primitive
			cglSubmitPrimitive(&shader, cube_vertices[i], cube_vertices[i + 1], cube_vertices[i + 2]);
		}
	
		// fence primitive tasks
		cglFencePrimitives();
	}
	
	int main()
	{
		size_t w = 1024;
		size_t h = 768;
		// initialize window
		cglInitWindow("Demo", w, h);
	
		// set viewport
		cglSetViewPort(w, h);
	
		// resize callback
		cglAddResizeEvent([](size_t w, size_t h, void* ud)
		{
			cglSetViewPort(w, h);
		}, nullptr);
	
		// setup shader properties
		cglVec3 cam_pos = cglVec3(2.0f, 2.0f, 2.0f);
		cglVec3 cube_pos = cglVec3Zero;
		float near = 0.05f;
		float far = 100.0f;
	
		float angle = 0.0f;
	
		cglMat4 model = cglTranslation(cube_pos) * cglRotation(cglVec3(0.0f, 1.0f, 0.0f), angle) * cglScale(cglVec3One);
		cglMat4 view = cglLookat(cam_pos, cube_pos, cglVec3Up);
		cglMat4 proj = cglPerspective(60.0f, (float)w / (float)h, near, far);
	
		shader.local_properties.set_mat4x4(mat_model, model);
		shader.local_properties.set_mat4x4(mat_view, view);
		shader.local_properties.set_mat4x4(mat_projection, proj);
		shader.local_properties.set_mat4x4(mat_vp, proj * view);
		shader.local_properties.set_mat4x4(mat_mvp, proj * view * model);
	
		shader.double_face = true;
		shader.ztest_func = cglCompareFunc::kLess;
	
		shader.local_properties.set_float4(albedo_prop, cglVec4(0.5f, 0.0f, 0.0f, 1.0f));
		shader.local_properties.set_float4(light_direction, cglVec4(0.0f, 1.0f, 1.5f, 1.0f));
		shader.local_properties.set_float4(light_diffuse, cglVec4(1.0f, 1.0f, 1.0f, 1.0f)); 
		shader.local_properties.set_float(light_intensity, 1.0f);
	
		// set background color
		cglSetClearColor(tinymath::kColorBlue);
	
		while (cglIsMainWindowOpen())
		{
			// clear window buffer
			cglClearMainWindow();
	
			// clear buffer
			cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);
	
			//angle += 1.0f;
			model = cglTranslation(cube_pos) * cglRotation(cglVec3(1.0f, 1.0f, 1.0f), angle) * cglScale(cglVec3One);
			shader.local_properties.set_mat4x4(mat_model, model);
			draw_cube();
	
			// fence pixel tasks
			cglFencePixels();
	
			cglSwapBuffer(true);
		}
	
		cglCloseMainWindow();
	
		return 0;
	}





Shader:

	#pragma once
	#include "Shader.hpp"
	
	using namespace CpuRasterizor;
	using namespace tinymath;
	
	class HelloLightingShader : public Shader
	{
	public:
		HelloLightingShader() : Shader("sample_shader") {}
	
		v2f vertex_shader(const a2v& input) const
		{
			v2f o;
			auto opos = vec4f(input.position.x, input.position.y, input.position.z, 1.0f);
			auto m = model();
			auto vp = vp_matrix();
			auto wpos = m * opos;
			auto v = view();
			auto p = projection();
			auto cpos = vp * wpos;
			o.position = cpos;
			o.uv = input.uv;
			tinymath::mat3x3 normal_matrix = tinymath::mat4x4_to_mat3x3(tinymath::transpose(tinymath::inverse(m)));
			o.normal = normal_matrix * input.normal;
			return o;
		}
	
		Color fragment_shader(const v2f& input) const
		{
			tinymath::vec4f albedo;
			tinymath::vec4f direction;
			tinymath::vec4f diffuse;
			float intensity;
	
			albedo = local_properties.get_float4(albedo_prop);
			direction = local_properties.get_float4(light_direction);
			diffuse = local_properties.get_float4(light_diffuse);
			intensity = local_properties.get_float(light_intensity);
	
			vec3f normal = input.normal;
			vec3f light_dir = direction.xyz;
			float ndl = dot(normal, light_dir);
	
			return albedo * diffuse * ndl * intensity;
		}
	};

Result:

<img src="/gallery/HelloLighting.png" width = "256" height = "200" alt="overview" align=bottom /> 


### Viewer

<img src="/gallery/IBL.png" width = "512" height = "400" alt="overview" align=bottom /> 

<img src="/gallery/IBL2.png" width = "512" height = "400" alt="overview" align=bottom /> 

### HDRI Tool

<img src="/gallery/ld9.png" width = "512" height = "300" alt="lookdev" align=bottom /> 

### Gallery

| Physical based rendering | Physical based rendering | Image based lighting |
| --- | --- | --- |
|  <img src="/gallery/Roughness.png" width = "180" height = "140" alt="Roughness" align=bottom/> | <img src="/gallery/Metallic.png" width = "180" height = "140" alt="Metallic" align=bottom/> | <img src="/gallery/IBL_Comparison.png" width = "180" height = "140" alt="MSAA" align=bottom/>  |


| MSAA | Pixel Derivative | Clipping | DebugView |
| ---  | --- | ---  | --- |
| <img src="/gallery/msaa_on_4x.png" width = "180" height = "180" alt="MSAA" align=bottom/> | <img src="/gallery/mipmap.png" width = "180" height = "180" alt="Mipmap" align=bottom/> | <img src="/gallery/Clipping.png" width = "180" height = "180" alt="Clipping" align=bottom />  | <img src="/gallery/DebugView.png" width = "180" height = "180" alt="DebugView" align=bottom/> |

| Stencil test | Depth test | Blending |
| --- | --- | --- |
| <img src="/gallery/stencil_test.jpg" width = "180" height = "180" alt="Per Sample Operation" align=bottom/> | <img src="/gallery/z_buffer.jpg" width = "220" height = "180" alt="Per Sample Operation" align=bottom/> | <img src="/gallery/transparent_cube.jpg" width = "180" height = "180" alt="Blending" align=bottom/> |


| Point sampler | Bilinear sampler |
| --- | --- |
| <img src="/gallery/point_filtering.jpg" width = "180" height = "180" alt="Texture Filtering" align=bottom/> | <img src="/gallery/bilinear_filtering.jpg" width = "180" height = "180" alt="Texture Filtering" align=bottom/>  |

| Mipmap |
| ---  |
| <img src="/gallery/MipmapComparison.png" width = "512" height = "180" alt="MSAA" align=bottom/> | 

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

- texture sampler

- cubemap sampler

- pixel derivative

- mipmap

- segment drawer

---
		
#### Application

- viewer

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

- simd
			
- premake --> cmake

- cross platform
		 
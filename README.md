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
		size_t w = 600;
		size_t h = 400;
		// initialize window
		cglInitWindow("Demo", w, h);

		// set viewport
		cglSetViewPort(0, 0, w, h);

		// resize callback
		cglAddResizeEvent([](size_t resized_w, size_t resized_h, void* ud)
		{
			UNUSED(ud);
			cglSetViewPort(0, 0, resized_w, resized_h);
		}, nullptr);

		HelloTriangleShader shader;
		uint32_t shader_id;
		cglCreateProgram(&shader, shader_id);

		// a triangle 
		cglVert v1(cglVec4(-0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2Zero);
		cglVert v2(cglVec4(0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2Zero);
		cglVert v3(cglVec4(0.0f, 0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2Zero);

		// setup shader properties
		cglUniformMatrix4fv(shader_id, mat_model_prop, cglMat4Identity);
		cglUniformMatrix4fv(shader_id, mat_view_prop, cglMat4Identity);
		cglUniformMatrix4fv(shader_id, mat_projection_prop, cglMat4Identity);
		cglUniformMatrix4fv(shader_id, mat_vp_prop, cglMat4Identity);
		cglUniformMatrix4fv(shader_id, mat_mvp_prop, cglMat4Identity);

		cglDisable(cglPipelineFeature::kBlending);
		cglDisable(cglPipelineFeature::kFaceCulling);
		cglDepthFunc(cglCompareFunc::kAlways);

		// set background color
		cglSetClearColor(tinymath::kColorBlue);

		while (cglIsMainWindowOpen())
		{
			// clear window buffer
			cglClearMainWindow();

			// clear buffer
			cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);

			cglUseProgram(shader_id);

			// submit primitive
			cglSubmitPrimitive(v1, v2, v3);

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
		size_t w = 600;
		size_t h = 400;
		// initialize window
		cglInitWindow("Demo", w, h);

		// set viewport
		cglSetViewPort(0, 0, w, h);

		// resize callback
		cglAddResizeEvent([](size_t resized_w, size_t resized_h, void* ud)
		{
			UNUSED(ud);
			cglSetViewPort(0, 0, resized_w, resized_h);
		}, nullptr);

		HelloTextureShader shader;
		uint32_t shader_id;
		cglCreateProgram(&shader, shader_id);

		// a triangle 
		cglVert v1(cglVec4(-0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2(0.0f, 0.0f));
		cglVert v2(cglVec4(0.5f, -0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2(1.0f, 0.0f));
		cglVert v3(cglVec4(0.0f, 0.5f, 0.0f, 1.0f), cglVec3Zero, cglVec2(0.5f, 1.0f));

		// setup shader properties
		cglUniformMatrix4fv(shader_id, mat_model_prop, cglMat4Identity);
		cglUniformMatrix4fv(shader_id, mat_view_prop, cglMat4Identity);
		cglUniformMatrix4fv(shader_id, mat_projection_prop, cglMat4Identity);
		cglUniformMatrix4fv(shader_id, mat_vp_prop, cglMat4Identity);
		cglUniformMatrix4fv(shader_id, mat_mvp_prop, cglMat4Identity);

		cglDisable(cglPipelineFeature::kBlending);
		cglDisable(cglPipelineFeature::kFaceCulling);
		cglDepthFunc(cglCompareFunc::kAlways);

		auto tex = std::make_shared<Texture>(128, 128, cglTextureFormat::kRGB);

		// generate a checkerboard texture
		for (size_t r = 0; r < 64; ++r)
		{
			for (size_t c = 0; c < 64; ++c)
			{
				auto val = ((r & 0x8) == 0) ^ ((c & 0x8) == 0);
				tex->write(r, c, { (float)val, (float)val, (float)val, 1.0f});
			}
		}

		// todo: strinify the key
		shader.local_properties.set_texture(123, tex);

		// set background color
		cglSetClearColor(tinymath::kColorBlue);

		while (cglIsMainWindowOpen())
		{
			// clear window buffer
			cglClearMainWindow();

			// clear buffer
			cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);

			cglUseProgram(shader_id);

			// submit primitive
			cglSubmitPrimitive(v1, v2, v3); 

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
	
			// visualize uv
			//return input.uv;
	
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

	Color fragment_shader(const v2f& input) const
	{
		return input.uv;
	}

Result:

<img src="/gallery/HelloTexture2.png" width = "256" height = "200" alt="overview" align=bottom /> 


##### Simple Lighting

	#include "CGL.h"
	#include "HelloLightingShader.hpp"

	HelloLightingShader shader;

	cglVec3 spherical_coord(float theta, float phi)
	{
		cglVec3 ret;
		ret.x = cglCos(theta) * cglCos(phi);
		ret.y = cglSin(theta);
		ret.z = cglCos(theta) * cglSin(phi);
		return cglNormalize(ret);
	}

	int main()
	{
		size_t w = 600;
		size_t h = 400;
		// initialize window
		cglInitWindow("Demo", w, h);

		// set viewport
		cglSetViewPort(0, 0, w, h);

		// resize callback
		cglAddResizeEvent([](size_t resized_w, size_t resized_h, void* ud)
		{
			UNUSED(ud);
			cglSetViewPort(0, 0, resized_w, resized_h);
		}, nullptr);

		// setup shader properties
		cglVec3 cam_pos = cglVec3(1.5f, 1.5f, 1.5f);
		cglVec3 cube_pos = cglVec3Zero;
		cglVec3 light_direction;	

		float l_theta = 0.0f;
		float l_phi = 0.0f;

		light_direction.x = 0.0f;
		light_direction.y = 0.5f;
		light_direction.z = 1.0f;

		float near = 0.05f;
		float far = 100.0f;

		cglMat4 model = cglTranslation(cube_pos);
		cglMat4 view = cglLookat(cam_pos, cube_pos, cglVec3Up);
		cglMat4 proj = cglPerspective(60.0f, (float)w / (float)h, near, far);

		uint32_t shader_id;
		cglCreateProgram(&shader, shader_id);

		cglUniformMatrix4fv(shader_id, mat_model_prop, model);
		cglUniformMatrix4fv(shader_id, mat_view_prop, view);
		cglUniformMatrix4fv(shader_id, mat_projection_prop, proj);
		cglUniformMatrix4fv(shader_id, mat_vp_prop, proj * view);
		cglUniformMatrix4fv(shader_id, mat_mvp_prop, proj * view * model);
		cglUniform4fv(shader_id, cam_position_prop, cam_pos);
		cglUniform4fv(shader_id, albedo_prop, cglVec4(0.5f, 0.0f, 0.0f, 1.0f));
		cglUniform4fv(shader_id, light_direction_prop, light_direction);
		cglUniform4fv(shader_id, light_color_prop, cglVec4(1.0f, 1.0f, 1.0f, 1.0f));
		cglUniform1f(shader_id, light_intensity_prop, 1.0f);

		cglDisable(cglPipelineFeature::kBlending);
		cglEnable(cglPipelineFeature::kFaceCulling);
		cglCullFace(cglFaceCulling::Back);
		cglDepthFunc(cglCompareFunc::kLess);

		std::vector<Vertex> vert;
		std::vector<size_t> ind;

		// construct a sphere
		float step = TWO_PI / 64.0f;
		for (float phi = 0.0f; phi < TWO_PI; phi += step)
		{
			for (float theta = -HALF_PI; theta < HALF_PI; theta += step)
			{
				float phi0 = phi;
				float phi1 = phi + step;

				float theta0 = theta;
				float theta1 = theta + step;

				auto pos0 = spherical_coord(theta0, phi0);

				auto pos1 = spherical_coord(theta1, phi0);

				auto pos2 = spherical_coord(theta1, phi1);

				auto pos3 = spherical_coord(theta0, phi1);

				Vertex v0, v1, v2, v3;
				v0.position = pos0;
				v0.normal = pos0;
				v1.position = pos1;
				v1.normal = pos1;
				v2.position = pos2;
				v2.normal = pos2;
				v3.position = pos3;
				v3.normal = pos3;

				ind.push_back(vert.size());
				vert.push_back(v0);
				ind.push_back(vert.size());
				vert.push_back(v1);
				ind.push_back(vert.size());
				vert.push_back(v2);
				ind.push_back(vert.size());
				vert.push_back(v0);
				ind.push_back(vert.size());
				vert.push_back(v2);
				ind.push_back(vert.size());
				vert.push_back(v3);
			}
		}

		// set background color
		cglSetClearColor(tinymath::kColorBlue);

		while (cglIsMainWindowOpen())
		{
			// clear window buffer
			cglClearMainWindow();

			// clear buffer
			cglClearBuffer(cglFrameContent::kColor | cglFrameContent::kDepth | cglFrameContent::kStencil);

			// rotate light
			l_theta += 0.01f;
			l_phi += 0.01f;
			light_direction.x = cglCos(l_theta) * cglCos(l_phi);
			light_direction.y = cglSin (l_theta);
			light_direction.z = cglCos(l_theta) * cglSin(l_phi);
			light_direction = cglNormalize(light_direction);

			shader.local_properties.set_float4(light_direction_prop, light_direction);

			cglUseProgram(shader_id);

			for (int i = 0; i < ind.size(); i += 3)
			{
				size_t index = ind[i];
				auto& v1 = vert[index];
				auto& v2 = vert[index + 1];
				auto& v3 = vert[index + 2];
				// submit primitive
				cglSubmitPrimitive(v1, v2, v3);
			}

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
			mat3x3 normal_matrix = mat4x4_to_mat3x3(transpose(inverse(m)));
			o.normal = normal_matrix * input.normal;
			return o;
		}
	
		Color fragment_shader(const v2f& input) const
		{
			vec4f albedo = local_properties.get_float4(albedo_prop);
			vec4f light_color = local_properties.get_float4(light_color_prop);
			float intensity = local_properties.get_float(light_intensity_prop);
	
			vec3f normal = input.normal;
			vec3f light_dir = local_properties.get_float4(light_direction_prop).xyz;
			float ndl = dot(normal, light_dir);
	
			return albedo * light_color * ndl * intensity;
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
		 
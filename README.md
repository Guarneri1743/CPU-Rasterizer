# Guarneri(A tile based soft rasterizer)

#### Rasterizer Feature:

- 3D-Math

- Multi-thread drawcall submit
			
- Vertex Shader/Fragment Shader

- Near Plane Clipping(Sutherland-Hodgman)

- Frustum Culling(Per triangle)

- CVV Clipping
			
- Back-Face Culling
			
- Rasterization(Tile based & Multi-thread)

- Perspective-Correct Interpolation(Texture mapping)
			
- Screen(Viewport) Clipping
			
- Early-Z
			
- Alpha-Test (discard)
			
- Stencil-Test
			
- Depth-Test/Depth-Writing
			
- Alpha-Blending

- Line-Drawer(Bresenham、DDA)

- Texture sampler

- Cubemap sampler
			
- Present framebuffer(GDI bitmap)

- Support switching LH/RH and DX/GL.

- Visualization: Early-Z optimized pixels, Back-Face Culled triangles, Z-Buffer, Application(Shadowmap, UV, Normal, AO), etc.

---
		
#### Application Feature:

- Input Manager
			
- Model-Importer

- Transform

- Roaming Camera

- Material

- Texture

- Various Renderer

- Commonly used lighting techs(Directional lighting, point lighting, Normal mapping, Shadow mapping, etc.)
			
- Cook-Torrance BRDF, IBL(naive impl)

- Metallic workflow

- Gamma/Linear Color-Space workflow

---
			
#### ThirdParty:
- assimp (Model Importer)
- stb_image (Image Importer)
- [nlohmann json](https://github.com/nlohmann/json) (Serialization)
- [ThreadPool](https://github.com/progschj/ThreadPool) (Drawcall/Rasterization acceleration)

---

#### Usage
1. Download [zip](https://github.com/Guarneri1743/SoftRasterizer/archive/main.zip) or clone [Guarneri](https://github.com/Guarneri1743/SoftRasterizer)
2. cd to the project folder
3. run 'setup.bat'
4. you'll see a new folder 'vs2019' contians the solution(feel free to modify the 'setup.bat' script if you are using vs2015/2017)

---

####TODO(Future): 
		
##### Rasterizer:

- use SIMD math lib
			
- Shader-compiler, ddx, ddy（mipmap, lodSampler）
			
- OpenGL/DX-like API (State machine)
			
- Scissor Test
			
- Early-Z condition optimization(more intelligent?)
			
- Hi-Z
		 
- MSAA(SSAA)

	 
##### Application:

- Specular workflow
			
- CPU Frustum Culling, Occlusion Culling

- Skeleton-Animation, Skinning

- Deferred/TBDR/Cluster pipeline

- IBL: irradiance convloution
			
- Hi-transform
			
- Z-Prepass
			
- HDR(ACES?)
			
- SSAO/HBAO/TAA/Bloom/ColorGrading (honestly, it's hard to implement post processing in a soft rasterizer, cuz it may eat cpu up)
					
- Instancing

---

### Screenshots

![Overview](https://github.com/Guarneri1743/SoftRasterizer/blob/main/screenshots/overview.jpg)

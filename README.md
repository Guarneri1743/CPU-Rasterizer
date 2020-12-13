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

#### TODO(Future): 
		
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

Overview

- Star War

<img src="/screenshots/helmet_ibl.jpg" width = "800" height = "800" alt="Star War" align=center />

---


- Backpack
<img src="/screenshots/overview.jpg" width = "800" height = "600" alt="Backpack" align=center />


---

- Near Plane Clipping:
<img src="/screenshots/near_plane_clipping.jpg" width = "400" height = "300" alt="Near Plane Clipping" align=center />
<img src="/screenshots/near_plane_clipping2.jpg" width = "400" height = "300" alt="Near Plane Clipping" align=center />


---


- Early-Z:
<img src="/screenshots/early_z.jpg" width = "400" height = "300" alt="Early-Z" align=center />


---

- Perspective Correct Interpolation(UV):
<img src="/screenshots/perspective_correct_uv.jpg" width = "400" height = "300" alt="Perspective Correct Interpolation" align=center />

---

- Texture Filtering:

Point
<img src="/screenshots/point_filtering.jpg" width = "400" height = "300" alt="Texture Filtering" align=center/>
Bilinear
<img src="/screenshots/bilinear_filtering.jpg" width = "400" height = "300" alt="Texture Filtering" align=center/> 

---


- Per Sample Operation:

Stencil Test

<img src="/screenshots/stencil_test.jpg" width = "400" height = "400" alt="Per Sample Operation" align=center/>

Z Test

<img src="/screenshots/z_buffer.jpg" width = "400" height = "300" alt="Per Sample Operation" align=center/>

--- 


- Blending:

<img src="/screenshots/opaque_cube.jpg" width = "400" height = "400" alt="Blending" align=center/>
<img src="/screenshots/transparent_cube.jpg" width = "400" height = "400" alt="Blending" align=center/>

--- 

- Wireframe:

<img src="/screenshots/wireframe.jpg" width = "400" height = "300" alt="Wireframe" align=center/>

---

- Tile based rasterization:

<img src="/screenshots/tile_based_rendering.jpg" width = "400" height = "300" alt="Tile based rasterization" align=center/>

tile = 64/128

---

- Shadow Mapping:

<img src="/screenshots/shadowmap.jpg" width = "400" height = "300" alt="Shadow Mapping" align=center/>

<img src="/screenshots/overview.jpg" width = "400" height = "300" alt="Shadow Mapping" align=center/>

<img src="/screenshots/shadow_off.jpg" width = "512" height = "512" alt="Shadow Mapping" align=center/>

<img src="/screenshots/shadow_on.jpg" width = "512" height = "512" alt="Shadow Mapping" align=center/>

---

- Frustum Culling:

<img src="/screenshots/frustum_culling.jpg" width = "512" height = "512" alt="Frustum Culling" align=center/>

---

- Back-Face Culling:

<img src="/screenshots/backface_culling.jpg" width = "512" height = "512" alt="Back-Face Culling" align=center/>

---

- AO:

<img src="/screenshots/ao.jpg" width = "400" height = "400" alt="AO" align=center/>

<img src="/screenshots/ao2.jpg" width = "400" height = "400" alt="AO" align=center/>

---
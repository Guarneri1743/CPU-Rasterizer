# Tile Based CPU Rasterizer

#### Feature:

- 3D-Math

- Multi-thread drawcall submit
			
- Vertex shader/fragment shader

- CVV clipping

- Frustum culling
			
- Back-face culling
			
- Multi-thread and tile based rasterization

- Perspective-correct interpolation
			
- Early-Z
			
- Alpha test (discard)
			
- Stencil test
			
- Depth test
			
- Alpha blending

- Segment-drawer

- Texture sampler

- Cubemap sampler

- Resource visualization

---
		
#### Application:

- Input manager
			
- Model importer

- Transform

- Roaming camera

- Material

- Texture

- Various renderer

- Commonly used lighting techs
			
- Cook-Torrance BRDF, IBL

- Metallic workflow

- Gamma/Linear workflow

---
			
#### ThirdParty:
- assimp (Model Importer)
- stb_image (Image Importer)
- [nlohmann json](https://github.com/nlohmann/json) (Serialization)
- [ThreadPool](https://github.com/progschj/ThreadPool) (Drawcall/Rasterization acceleration)

---

#### Install
1. Download [zip](https://github.com/Guarneri1743/SoftRasterizer/archive/main.zip) or clone [CPU-Rasterizer](https://github.com/Guarneri1743/SoftRasterizer)
2. cd to the project folder
3. run 'setup.bat'
4. you'll see a new folder 'vs2019' contians the solution(modify the 'setup.bat' script if you are using vs2015/2017)

---

#### Future Work: 

- SIMD
			
- Shader-compiler, ddx, ddy, mipmap and lodSampler
			
- OpenGL/DX-like API
			
- Scissor test
			
- Early-Z condition optimization
			
- Hi-Z
		 
- MSAA

- Specular workflow
			
- CPU frustum culling & occlusion culling

- Skeleton-animation, skinning

- TBDR

- Cluster

- IBL baker
			
- Hi-transform
			
- Z-Prepass
			
- HDR(ACES?)
			
- SSAO/HBAO/TAA/Bloom/ColorGrading 
					
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


<img src="/screenshots/near_plane_clipping.jpg" width = "800" height = "600" alt="Near Plane Clipping" align=center />

<img src="/screenshots/near_plane_clipping2.jpg" width = "800" height = "600" alt="Near Plane Clipping" align=center />


---


- Early-Z:
<img src="/screenshots/early_z.jpg" width = "800" height = "600" alt="Early-Z" align=center />


---

- Perspective Correct Interpolation(UV):
<img src="/screenshots/perspective_correct_uv.jpg" width = "800" height = "600" alt="Perspective Correct Interpolation" align=center />

---

- Texture Filtering:

Point
<img src="/screenshots/point_filtering.jpg" width = "800" height = "600" alt="Texture Filtering" align=center/>

Bilinear
<img src="/screenshots/bilinear_filtering.jpg" width = "800" height = "600" alt="Texture Filtering" align=center/> 

---


- Per Sample Operation:

Stencil Test

<img src="/screenshots/stencil_test.jpg" width = "800" height = "800" alt="Per Sample Operation" align=center/>

Z Test

<img src="/screenshots/z_buffer.jpg" width = "800" height = "600" alt="Per Sample Operation" align=center/>

--- 


- Blending:

<img src="/screenshots/opaque_cube.jpg" width = "800" height = "800" alt="Blending" align=center/>

<img src="/screenshots/transparent_cube.jpg" width = "800" height = "800" alt="Blending" align=center/>

--- 

- Wireframe:

<img src="/screenshots/wireframe.jpg" width = "800" height = "600" alt="Wireframe" align=center/>

---

- Tile based rasterization:

<img src="/screenshots/tile_based_rendering.jpg" width = "800" height = "600" alt="Tile based rasterization" align=center/>

tile = 64/128

---

- Shadow Mapping:

<img src="/screenshots/shadowmap.jpg" width = "800" height = "600" alt="Shadow Mapping" align=center/>

<img src="/screenshots/overview.jpg" width = "800" height = "600" alt="Shadow Mapping" align=center/>

<img src="/screenshots/shadow_off.jpg" width = "800" height = "800" alt="Shadow Mapping" align=center/>

<img src="/screenshots/shadow_on.jpg" width = "800" height = "800" alt="Shadow Mapping" align=center/>

---

- Frustum Culling:

<img src="/screenshots/frustum_culling.jpg" width = "800" height = "600" alt="Frustum Culling" align=center/>

---

- Back-Face Culling:

<img src="/screenshots/backface_culling.jpg" width = "800" height = "600" alt="Back-Face Culling" align=center/>

---

- AO:

<img src="/screenshots/ao.jpg" width = "800" height = "600" alt="AO" align=center/>

<img src="/screenshots/ao2.jpg" width = "800" height = "800" alt="AO" align=center/>

---
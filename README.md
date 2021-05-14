# Tile Based CPU Rasterizer

#### Feature:

- multi-thread drawcall submit
			
- vertex/fragment shader

- cvv clipping

- frustum culling
			
- back-face culling
			
- multi-thread and tile based rasterization

- msaa

- perspective correct interpolation
			
- early-z
			
- alpha test (discard)
			
- stencil test
			
- depth test
			
- alpha blending

- segment drawer

- texture sampler

- cubemap sampler

- resource visualization

---
		
#### Application:

- input manager
			
- model importer

- transform

- roaming camera

- material

- texture

- renderer

- commonly used lighting techs
			
- cook-torrance brdf, simple ibl

- metallic workflow

- gamma/linear workflow

---
			
#### ThirdParty:
- assimp 
- stb_image 
- [nlohmann json](https://github.com/nlohmann/json) 
- [ThreadPool](https://github.com/progschj/ThreadPool) 

---

#### Install
1. download [zip](https://github.com/Guarneri1743/CPU-Rasterizer/archive/main.zip) or clone [CPU-Rasterizer](https://github.com/Guarneri1743/CPU-Rasterizer)
2. run 'setup.bat' or type 'premake vs2017/vs2019' in command-line

---

#### Future Work: 

- low-level:
	
	- simd
				
	- ddx, ddy, mipmap and lod sampler
				
	- gl/dx like api
				
	- scissor test
				
	- hsr (base on tbdr)
		 
- features:

	- specular workflow
			
	- occlusion culling
	
	- skeleton-animation
	
	- skinning
					
	- hdr

---

### Gallery

overview

<img src="/gallery/helmet_ibl.jpg" width = "256" height = "256" alt="Star War" align=bottom />
<img src="/gallery/overview.jpg" width = "300" height = "256" alt="Backpack" align=bottom />
<img src="/gallery/wireframe.jpg" width = "300" height = "256" alt="Wireframe" align=center/>

- tile based rasterization:

<img src="/gallery/tile_based_rendering.jpg" width = "300" height = "256" alt="Tile based rasterization" align=center/>

- near plane clipping:

<img src="/gallery/near_plane_clipping2.jpg" width = "256" height = "256" alt="Near Plane Clipping" align=center />
<img src="/gallery/near_plane_clipping.jpg" width = "256" height = "256" alt="Near Plane Clipping" align=center />


- MSAA:

no MSAA vs 16x MSAA
<img src="/gallery/no_msaa.jpg" width = "256" height = "256" alt="MSAA" align=center/>
<img src="/gallery/16xMSAA.jpg" width = "256" height = "256" alt="MSAA" align=center/> 


- perspective correct interpolation:
<img src="/gallery/perspective_correct_uv.jpg" width = "256" height = "256" alt="Perspective Correct Interpolation" align=center />


- texture filtering:

point/bilinear
<img src="/gallery/point_filtering.jpg" width = "280" height = "256" alt="Texture Filtering" align=center/>
<img src="/gallery/bilinear_filtering.jpg" width = "280" height = "256" alt="Texture Filtering" align=center/> 



- per sample operation:

stencil-test, z-test and blending

<img src="/gallery/stencil_test.jpg" width = "256" height = "256" alt="Per Sample Operation" align=center/>
<img src="/gallery/z_buffer.jpg" width = "280" height = "256" alt="Per Sample Operation" align=center/>
<img src="/gallery/transparent_cube.jpg" width = "256" height = "256" alt="Blending" align=center/>


- shadow mapping:

<img src="/gallery/shadowmap.jpg" width = "256" height = "256" alt="Shadow Mapping" align=top/>

<img src="/gallery/overview.jpg" width = "256" height = "256" alt="Shadow Mapping" align=top/>

shadow off/on

<img src="/gallery/shadow_off.jpg" width = "256" height = "256" alt="Shadow Mapping" align=bottom/>

<img src="/gallery/shadow_on.jpg" width = "256" height = "256" alt="Shadow Mapping" align=bottom/>


- AO:

<img src="/gallery/ao.jpg" width = "256" height = "256" alt="AO" align=center/>

<img src="/gallery/ao2.jpg" width = "256" height = "256" alt="AO" align=center/>

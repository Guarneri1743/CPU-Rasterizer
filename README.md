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

overview:

<table>
<tr>
<img src="/gallery/helmet_ibl.jpg" width = "180" height = "180" alt="Star War" align=bottom />
<img src="/gallery/overview.jpg" width = "220" height = "180" alt="Backpack" align=bottom />
<img src="/gallery/wireframe.jpg" width = "220" height = "180" alt="Wireframe" align=bottom/>
</tr>
</table>

tile based rasterization:

<img src="/gallery/tile_based_rendering.jpg" width = "220" height = "180" alt="Tile based rasterization" align=bottom/>

near plane clipping:

<table>
<tr>
<img src="/gallery/near_plane_clipping2.jpg" width = "180" height = "180" alt="Near Plane Clipping" align=bottom />
<img src="/gallery/near_plane_clipping.jpg" width = "180" height = "180" alt="Near Plane Clipping" align=bottom />
</tr>
</table>

MSAA:

<table>
<tr>
<img src="/gallery/no_msaa.jpg" width = "180" height = "180" alt="MSAA" align=bottom/>
<img src="/gallery/16xMSAA.jpg" width = "180" height = "180" alt="MSAA" align=bottom/> 
</tr>
</table>

perspective correct interpolation:

<img src="/gallery/perspective_correct_uv.jpg" width = "180" height = "180" alt="Perspective Correct Interpolation" align=bottom />

point/bilinear texture filtering:

<table>
<tr>
<img src="/gallery/point_filtering.jpg" width = "180" height = "180" alt="Texture Filtering" align=bottom/>
<img src="/gallery/bilinear_filtering.jpg" width = "180" height = "180" alt="Texture Filtering" align=bottom/> 
</tr>
</table>

stencil-test, z-test and blending

<table>
<tr>
<img src="/gallery/stencil_test.jpg" width = "180" height = "180" alt="Per Sample Operation" align=bottom/>
<img src="/gallery/z_buffer.jpg" width = "220" height = "180" alt="Per Sample Operation" align=bottom/>
<img src="/gallery/transparent_cube.jpg" width = "180" height = "180" alt="Blending" align=bottom/>
</tr>
</table>

shadow mapping:

<table>
<tr>
<img src="/gallery/shadowmap.jpg" width = "180" height = "180" alt="Shadow Mapping" align=bottom/>
<img src="/gallery/overview.jpg" width = "180" height = "180" alt="Shadow Mapping" align=bottom/>
<img src="/gallery/shadow_off.jpg" width = "180" height = "180" alt="Shadow Mapping" align=bottom/>
<img src="/gallery/shadow_on.jpg" width = "180" height = "180" alt="Shadow Mapping" align=bottom/>
</tr>
</table>

AO:

<table>
<tr>
<img src="/gallery/ao.jpg" width = "180" height = "180" alt="AO" align=bottom/>
<img src="/gallery/ao2.jpg" width = "180" height = "180" alt="AO" align=bottom/>
</tr>
</table>
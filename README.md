# Tile Based CPU Rasterizer

### Gallery

overview

<img src="/gallery/IBL.png" width = "800" height = "600" alt="overview" align=bottom /> 

<img src="/gallery/IBL2.png" width = "800" height = "600" alt="overview" align=bottom /> 

<img src="/gallery/IBL3.png" width = "800" height = "600" alt="overview" align=bottom /> 

| tile based rasterization | near plane clipping |  near plane clipping | perspective correct interpolation |
| --- | --- | --- | --- |
| <img src="/gallery/tile_based_rendering.jpg" width = "220" height = "180" alt="Tile based rasterization" align=bottom/> | <img src="/gallery/near_plane_clipping2.jpg" width = "180" height = "180" alt="Near Plane Clipping" align=bottom /> | <img src="/gallery/near_plane_clipping.jpg" width = "180" height = "180" alt="Near Plane Clipping" align=bottom /> | <img src="/gallery/perspective_correct_uv.jpg" width = "180" height = "180" alt="Perspective Correct Interpolation" align=bottom /> |

| no msaa | 16x msaa |
| --- | --- |
| <img src="/gallery/no_msaa.jpg" width = "180" height = "180" alt="MSAA" align=bottom/> | <img src="/gallery/16xMSAA.jpg" width = "180" height = "180" alt="MSAA" align=bottom/> |

| point | bilinear |
| --- | --- |
| <img src="/gallery/point_filtering.jpg" width = "180" height = "180" alt="Texture Filtering" align=bottom/> | <img src="/gallery/bilinear_filtering.jpg" width = "180" height = "180" alt="Texture Filtering" align=bottom/>  |

| stencil-test | z-test | blending |
| --- | --- | --- |
| <img src="/gallery/stencil_test.jpg" width = "180" height = "180" alt="Per Sample Operation" align=bottom/> | <img src="/gallery/z_buffer.jpg" width = "220" height = "180" alt="Per Sample Operation" align=bottom/> | <img src="/gallery/transparent_cube.jpg" width = "180" height = "180" alt="Blending" align=bottom/> |


| shadow mapping |
| --- |
| <img src="/gallery/shadowmap.jpg" width = "180" height = "180" alt="Shadow Mapping" align=bottom/> <img src="/gallery/shadow_off.jpg" width = "180" height = "180" alt="Shadow Mapping" align=bottom/> <img src="/gallery/shadow_on.jpg" width = "180" height = "180" alt="Shadow Mapping" align=bottom/> |

| ao |
| --- | 
| <img src="/gallery/ao.jpg" width = "200" height = "180" alt="AO" align=bottom/> <img src="/gallery/ao2.jpg" width = "180" height = "180" alt="AO" align=bottom/> |

--- 

#### Features

- fully parallel of triangle/pixel processing
			
- vertex/fragment shader

- cvv clipping

- frustum culling
			
- back-face culling

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

- ibl baking

- ibl

- metallic workflow

- gamma/linear workflow

---

#### Install
1. download [zip](https://github.com/Guarneri1743/CPU-Rasterizer/archive/main.zip) or clone [CPU-Rasterizer](https://github.com/Guarneri1743/CPU-Rasterizer)
2. run 'setup.bat' or type 'premake vs2017/vs2019' in command-line

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
				
	- ddx, ddy
	
	- vector swizzling
				
	- gl/dx like api
		 
- features:

	- specular workflow
	
	- skeleton-animation
	
	- skinning
					
	- hdr
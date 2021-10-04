# Tile Based CPU Rasterizer

### Gallery

overview

<img src="/gallery/IBL.png" width = "800" height = "600" alt="overview" align=bottom /> 

<img src="/gallery/IBL2.png" width = "800" height = "600" alt="overview" align=bottom /> 

<img src="/gallery/IBL3.png" width = "800" height = "600" alt="overview" align=bottom /> 

| DebugView |
| --- |
|  <img src="/gallery/DebugView.png" width = "600" height = "400" alt="DebugView" align=bottom/> |


| Physical based rendering |
| ---  |
|  <img src="/gallery/Roughness.png" width = "600" height = "400" alt="Roughness" align=bottom/> |

| Physical based rendering |
| --- |
| <img src="/gallery/Metallic.png" width = "600" height = "400" alt="Metallic" align=bottom/> |


| Image based lighting |
| --- |
| <img src="/gallery/IBL_Comparison.png" width = "600" height = "400" alt="MSAA" align=bottom/>  |

| Clipping  |
| ---  |
| <img src="/gallery/Clipping.png" width = "600" height = "400" alt="Clipping" align=bottom />  |

| Stencil test | Depth test | Blending |
| --- | --- | --- |
| <img src="/gallery/stencil_test.jpg" width = "180" height = "180" alt="Per Sample Operation" align=bottom/> | <img src="/gallery/z_buffer.jpg" width = "220" height = "180" alt="Per Sample Operation" align=bottom/> | <img src="/gallery/transparent_cube.jpg" width = "180" height = "180" alt="Blending" align=bottom/> |


| Point sampler | Bilinear sampler |
| --- | --- |
| <img src="/gallery/point_filtering.jpg" width = "180" height = "180" alt="Texture Filtering" align=bottom/> | <img src="/gallery/bilinear_filtering.jpg" width = "180" height = "180" alt="Texture Filtering" align=bottom/>  |


| MSAA |
| ---  |
| <img src="/gallery/msaa_on_4x.png" width = "512" height = "512" alt="MSAA" align=bottom/> |


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
				
	- gl/dx like api
		 
- features:

	- specular workflow
	
	- skeleton-animation
	
	- skinning
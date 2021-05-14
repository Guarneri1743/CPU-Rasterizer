#ifndef _GUARNERI_
#define _GUARNERI_

#include <stdint.h>
#include "InputManager.hpp"
#include "Time.hpp"
#include "Scene.hpp"
#include "GraphicsDevice.hpp"

namespace Guarneri
{
	class CPURasterizer
	{
	public:
		static void prepare(const uint32_t w, const uint32_t h, LPCSTR title)
		{
			INST(GDIWindow).initialize(w, h, title, INST(InputManager).event_callback);
			INST(InputManager).add_on_key_down_evt([](KeyCode code, void* data)
			{
				UNUSED(data);
				if (code == KeyCode::ESC) INST(GDIWindow).dispose();
			}, nullptr);
			INST(GraphicsDevice).initialize(INST(GDIWindow).framebuffer, INST(GDIWindow).width, INST(GDIWindow).height);
			Time::start();
		}

		static void kick_off(Scene& scene)
		{
			while (INST(GDIWindow).is_valid())
			{
				Time::frame_start();
				INST(InputManager).update();
				scene.update();
				scene.render();
				const int w = 800;
				const int h = 20;
				{
					std::stringstream ss;
					ss << "Shortcut: " << "F1:ShadowMap(P: ShadowOn/Off), F2:Depth, F3:WireFrame, B: Bilinear On/Off";
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "Shortcut: " << "F4: FrameTiles, F5:UV, F6:VertexColor, F7:Normal, F8:Specular, F9:Stencil";
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "CullingClip: " << INST(MiscParameter).culling_clipping_flag;
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "RenderFlag: " << INST(MiscParameter).render_flag;
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "EnableShadow: " << (INST(MiscParameter).enable_shadow ? "SHADOW_ON" : "SHADOW_OFF");
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "PCF: " << (INST(MiscParameter).pcf_on ? "PCF_ON" : "PCF_OFF");
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "MSAA: " << (INST(MiscParameter).enable_msaa ? "MSAA_ON" : "MSAA_OFF");
					if (INST(MiscParameter).enable_msaa)
					{
						ss << std::to_string(INST(GraphicsDevice).msaa_subsample_count) << "X";
					}
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "ShadowBias: " << INST(MiscParameter).shadow_bias;
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "FPS: " << (int)Time::fps;
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "Triangles: " << INST(GraphicsDevice).statistics.triangle_count;
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "FrustumCulled: " << INST(GraphicsDevice).statistics.culled_triangle_count;
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "BackFaceCulled: " << INST(GraphicsDevice).statistics.culled_backface_triangle_count;
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "EarlyZ_Optimized_Pixels: " << INST(GraphicsDevice).statistics.earlyz_optimized;
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "CameraPos: " << INST(MiscParameter).camera_pos;
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				{
					std::stringstream ss;
					ss << "LightDir: " << INST(MiscParameter).main_light.forward;
					INST(GDIWindow).draw_text(w, h, ss.str().c_str());
				}
				if (INST(GraphicsDevice).multi_thread)
				{
					{
						std::stringstream ss;
						ss << "Threads: " << std::thread::hardware_concurrency();
						INST(GDIWindow).draw_text(w, h, ss.str().c_str());
					}
				}
				if (INST(GraphicsDevice).tile_based)
				{
					auto tinfo = INST(GraphicsDevice).get_tile_info();
					{
						std::stringstream ss;
						ss << "TileSize: " << tinfo.tile_size;
						INST(GDIWindow).draw_text(w, h, ss.str().c_str());
					}
					{
						std::stringstream ss;
						ss << "TileRowCount: " << tinfo.row_tile_count;
						INST(GDIWindow).draw_text(w, h, ss.str().c_str());
					}
					{
						std::stringstream ss;
						ss << "TileColCount: " << tinfo.col_tile_count;
						INST(GDIWindow).draw_text(w, h, ss.str().c_str());
					}
					{
						std::stringstream ss;
						ss << "TileTaskSize: " << tinfo.tile_task_size;
						INST(GDIWindow).draw_text(w, h, ss.str().c_str());
					}
				}
				INST(GDIWindow).flush();
				Time::frame_end();
				Sleep(1);
			}
		}
	};
}
#endif
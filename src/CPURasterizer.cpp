#include "CPURasterizer.hpp"
#include "GDIWindow.hpp"
#include "GraphicsDevice.hpp"
#include "ShaderLab.hpp"

namespace Guarneri
{
	static std::ostream& operator << (std::ostream& stream, const RenderFlag& flag)
	{
		int count = 0;
		if ((flag & RenderFlag::WIREFRAME) != RenderFlag::DISABLE)
		{
			stream << "WIREFRAME";
			count++;
		}
		if ((flag & RenderFlag::DEPTH) != RenderFlag::DISABLE)
		{
			stream << (count > 0 ? " | DEPTH" : "DEPTH");
			count++;
		}
		if ((flag & RenderFlag::UV) != RenderFlag::DISABLE)
		{
			stream << (count > 0 ? " | UV" : "UV");
			count++;
		}
		if ((flag & RenderFlag::VERTEX_COLOR) != RenderFlag::DISABLE)
		{
			stream << (count > 0 ? " | VERTEX_COLOR" : "VERTEX_COLOR");
			count++;
		}
		if ((flag & RenderFlag::NORMAL) != RenderFlag::DISABLE)
		{
			stream << (count > 0 ? " | NORMAL" : "NORMAL");
			count++;
		}
		if ((flag & RenderFlag::CULLED_BACK_FACE) != RenderFlag::DISABLE)
		{
			stream << (count > 0 ? " | CULLED_BACK_FACE" : "CULLED_BACK_FACE");
			count++;
		}
		if ((flag & RenderFlag::EARLY_Z_DEBUG) != RenderFlag::DISABLE)
		{
			stream << (count > 0 ? " | EARLY_Z_DEBUG" : "EARLY_Z_DEBUG");
			count++;
		}
		if ((flag & RenderFlag::MIPMAP) != RenderFlag::DISABLE)
		{
			stream << (count > 0 ? " | MIPMAP" : "MIPMAP");
			count++;
		}
		if ((flag & RenderFlag::STENCIL) != RenderFlag::DISABLE)
		{
			stream << (count > 0 ? " | STENCIL" : "STENCIL");
			count++;
		}
		if ((flag & RenderFlag::SCANLINE) != RenderFlag::DISABLE)
		{
			stream << (count > 0 ? " | SCANLINE" : "SCANLINE");
			count++;
		}
		if ((flag & RenderFlag::FRAME_TILE) != RenderFlag::DISABLE)
		{
			stream << (count > 0 ? " | FRAME_TILE" : "FRAME_TILE");
			count++;
		}
		if ((flag & RenderFlag::SHADOWMAP) != RenderFlag::DISABLE)
		{
			stream << (count > 0 ? " | SHADOWMAP" : "SHADOWMAP");
			count++;
		}
		if ((flag & RenderFlag::SPECULAR) != RenderFlag::DISABLE)
		{
			stream << (count > 0 ? " | SPECULAR" : "SPECULAR");
			count++;
		}
		return stream;
	}

	static std::ostream& operator << (std::ostream& stream, const CullingAndClippingFlag& flag)
	{
		int count = 0;
		if ((flag & CullingAndClippingFlag::APP_FRUSTUM_CULLING) != CullingAndClippingFlag::DISABLE)
		{
			stream << "APP_FRUSTUM_CULLING";
			count++;
		}
		if ((flag & CullingAndClippingFlag::BACK_FACE_CULLING) != CullingAndClippingFlag::DISABLE)
		{
			stream << (count > 0 ? " | BACK_FACE_CULLING" : "BACK_FACE_CULLING");
			count++;
		}
		if ((flag & CullingAndClippingFlag::NEAR_PLANE_CLIPPING) != CullingAndClippingFlag::DISABLE)
		{
			stream << (count > 0 ? " | NEAR_PLANE_CLIPPING" : "NEAR_PLANE_CLIPPING");
			count++;
		}
		if ((flag & CullingAndClippingFlag::SCREEN_CLIPPING) != CullingAndClippingFlag::DISABLE)
		{
			stream << (count > 0 ? " | SCREEN_CLIPPING" : "SCREEN_CLIPPING");
			count++;
		}
		return stream;
	}

	void CPURasterizer::prepare(const uint32_t w, const uint32_t h, const char* title)
	{
		ShaderLab::initialize();
		INST(GDIWindow).initialize(w, h, static_cast<LPCSTR>(title), INST(InputManager).event_callback);
		INST(InputManager).add_on_key_down_evt([](KeyCode code, void* data)
		{
			UNUSED(data);
			if (code == KeyCode::ESC) INST(GDIWindow).dispose();
		}, nullptr);
		INST(GraphicsDevice).initialize(INST(GDIWindow).framebuffer, INST(GDIWindow).width, INST(GDIWindow).height);
		Time::start();
	}

	void CPURasterizer::kick_off(Scene& scene)
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
					ss << std::to_string(INST(GraphicsDevice).get_subsample_count()) << "X";
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
				ss << "CameraPos: " << INST(MiscParameter).camera_pos.str();
				INST(GDIWindow).draw_text(w, h, ss.str().c_str());
			}
			{
				std::stringstream ss;
				ss << "LightDir: " << INST(MiscParameter).main_light.forward.str();
				INST(GDIWindow).draw_text(w, h, ss.str().c_str());
			}
			{
				std::stringstream ss;
				ss << "Threads: " << std::thread::hardware_concurrency();
				INST(GDIWindow).draw_text(w, h, ss.str().c_str());
			}
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
			INST(GDIWindow).flush();
			Time::frame_end();
			Sleep(1);
		}
	}
}
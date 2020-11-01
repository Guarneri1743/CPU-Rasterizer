#pragma once
#include <guarneri.hpp>

namespace guarneri{
	class scene : singleton<scene>{
	public:
		void initialize() {
			auto tex_path = res_path() + "/textures/pavingstones_decorative2_2k_h_1.jpg";

			texture plane_tex(tex_path.c_str(), "MainTex");

			texture noise("MainTex", 512, 512, texture_format::rgba);

			noise::generate_fractal_image(noise, 512, 512);

			material plane_material;
			plane_material.transparent = false;

			material box_material;
			box_material.transparent = true;
			box_material.blend_op = blend_operator::add;
			box_material.src_factor = blend_factor::src_alpha;
			box_material.dst_factor = blend_factor::one_minus_src_alpha;
			box_material.zwrite_mode = zwrite::off;

			plane_material.set_texture("MainTex", &plane_tex);

			model bunny();

			float angle = 1;

			uint32_t w = 800;
			uint32_t h = 600;

			gdi_window gdiwin(w, h, _T("SoftRasterizerTitle"), _T("SoftRasterizer"));

			auto& device = singleton<render_device>::get();
			device.initialize(gdiwin.framebuffer, w, h);

			float aspect = (float)w / (float)h;

			float3 cam_pos = float3(5.0f, 5.0f, 5.0f);
			float3 box_pos = float3(0.0f, 0.0f, 0.0f);

			singleton<camera>::get().initialize(cam_pos, aspect, 45.0f, 0.5f, 500.0f, camera::projection::perspective);

			model model;
			model.load_from_file(res_path() + "/backpack/backpack.obj");

			renderer model_renderer(model);
		}

		void finalize() {

		}
	};
}
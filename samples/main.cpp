#include <iostream>
#include <vertex.hpp>
#include <render_device.hpp>
#include <material.hpp>
#include <camera.hpp>
#include <gdi_window.hpp>
#include <texture.hpp>
#include <noise.hpp>

using namespace guarneri;
using namespace std;

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

vertex mesh[8] = {
	 vertex(float4(-1, -1,  1, 1), float4(1.0f, 0.2f, 0.2f, 0.3f), float3::ONE, float2(0.0f, 0.0f), float3()),
	 vertex(float4(1, -1,  1, 1), float4(0.2f, 0.2f, 1.0f, 0.3f),float3::ONE,  float2(0.0f, 1.0f), float3()),
	 vertex(float4(1, 1,  1, 1), float4(0.2f, 0.2f, 1.0f, 0.3f), float3::ONE,  float2(1.0f, 1.0f), float3()),
	 vertex(float4(-1,  1,  1, 1), float4(1.0f, 0.2f, 1.0f, 0.3f),  float3::ONE, float2(1.0f, 0.0f), float3()),
	 vertex(float4(-1, -1, -1, 1), float4(1.0f, 1.0f, 0.2f, 0.3f), float3::ONE, float2(0.0f, 0.0f), float3()),
	 vertex(float4(1, -1, -1, 1),float4(0.2f, 1.0f, 1.0f, 0.3f),  float3::ONE, float2(0.0f, 1.0f), float3()),
	 vertex(float4(1,  1, -1, 1), float4(1.0f, 0.3f, 0.3f, 0.3f), float3::ONE,  float2(1.0f, 1.0f), float3()),
	 vertex(float4(-1,  1, -1, 1),  float4(0.2f, 1.0f, 0.3f, 0.3f), float3::ONE,float2(1.0f, 0.0f), float3()),
};

void draw_plane(render_device& device, material& mat, int a, int b, int c, int d, const mat4& m, const mat4& v, const mat4& p) {
	vertex p1 = mesh[a], p2 = mesh[b], p3 = mesh[c], p4 = mesh[d];
	p1.uv.x = 0, p1.uv.y = 0, p2.uv.x = 0, p2.uv.y = 1;
	p3.uv.x = 1, p3.uv.y = 1, p4.uv.x = 1, p4.uv.y = 0;
	device.draw_primitive(mat, p1, p2, p3, m, v, p);
	device.draw_primitive(mat, p3, p4, p1, m, v, p);
}

void draw_box(render_device& device, material& mat, const mat4& m, const mat4& v, const mat4& p) {
	draw_plane(device, mat, 0, 1, 2, 3, m, v, p);
	draw_plane(device, mat, 7, 6, 5, 4, m, v, p);
	draw_plane(device, mat, 0, 4, 5, 1, m, v, p);
	draw_plane(device, mat, 1, 5, 6, 2, m, v, p);
	draw_plane(device, mat, 2, 6, 7, 3, m, v, p);
	draw_plane(device, mat, 3, 7, 4, 0, m, v, p);
}

int main(void)
{
	shader s("default");

	auto dir = guarneri::res_path();
	auto tex_path = dir + "/pavingstones_decorative2_2k_h_1.jpg";
	
	texture plane_tex(tex_path.c_str(), "MainTex");

	texture noise("MainTex", 512, 512, texture_format::rgba);

	noise::generate_fractal_image(noise, 512, 512);

	material plane_material(&s);
	plane_material.transparent = false;

	material box_material(&s);
	box_material.transparent = true;
	box_material.blend_op = blend_operator::add;
	box_material.src_factor = blend_factor::src_alpha;
	box_material.dst_factor = blend_factor::one_minus_src_alpha;
	box_material.zwrite_mode = zwrite::off;

	plane_material.set_texture("MainTex", &noise);

	float angle = 1;

	unsigned int w = 800;
	unsigned int h = 600;

	gdi_window gdiwin(w, h, _T("SoftRasterizerTitle"), _T("SoftRasterizer"));

	render_device device(gdiwin.framebuffer, w, h);

	float aspect = (float)w / (float)h;

	float3 cam_pos = float3(5.0f, 5.0f, 5.0f);
	float3 box_pos = float3(0.0f, 0.0f, 0.0f);

	camera cam(cam_pos, aspect, 45.0f, 0.5f, 500.0f, camera::projection::perspective);


	while (gdiwin.is_valid()) {
		if (IS_ON(VK_F3)) device.r_flag = render_flag::wire_frame;
		if (IS_ON(VK_F2)) device.r_flag = render_flag::scanline;
		if (IS_ON(VK_F1)) device.r_flag = render_flag::shaded;
		if (IS_ON(VK_F4)) device.r_flag = render_flag::depth;
		if (IS_ON(VK_F5)) device.r_flag = (render_flag)((int)render_flag::shaded | (int)render_flag::uv);
		if (IS_ON(VK_F6)) device.r_flag = (render_flag)((int)render_flag::shaded | (int)render_flag::vertex_color);
		if (IS_ON(VK_UP)) box_pos.z -= 0.1f;
		if (IS_ON(VK_DOWN)) box_pos.z += 0.1f;
		if (IS_ON(VK_LEFT)) box_pos.x += 0.1f;
		if (IS_ON(VK_RIGHT)) box_pos.x -= 0.1f;
		if (IS_ON('W')) box_pos.y += 0.1f;
		if (IS_ON('S')) box_pos.y -= 0.1f;
		if (IS_ON('R')) angle += 1.0f;

		cam.set_position(cam_pos);
		cam.set_target(float3(0.0f, 0.0f, 0.0f));
		update_misc_params(w, h, cam.near, cam.far, cam.fov);

		device.clear_buffer();

		mat4 t = mat4::translation(box_pos);
		mat4 r = mat4::rotation(float3(0, -0.5, 0), angle);
		mat4 m = t * r;
		mat4 pm = mat4::translation(float3(0.0f, -1.0f, 0.0f));
		mat4 scale = mat4::scale(float3(3.0f, 1.0f, 3.0f));

		draw_plane(device, plane_material, 2, 6, 7, 3, pm * scale, cam.view_matrix(), cam.get_projection_matrix());

		draw_box(device, box_material, m, cam.view_matrix(), cam.get_projection_matrix());

		gdiwin.flush();

		Sleep(0);
	}

	return 0;
}
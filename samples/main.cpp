#include <iostream>
#include <vertex.hpp>
#include <render_device.hpp>
#include <material.hpp>
#include <camera.hpp>
#include <screen.hpp>
#include <texture.hpp>
#include <noise.hpp>

#undef near;
#undef far;

using namespace guarneri;
using namespace std;

vertex mesh[8] = {
	 vertex(float4(-1, -1,  1, 1), float4(1.0f, 0.2f, 0.2f), float3::ONE, float2(0.0f, 0.0f), float3()),
	 vertex(float4(1, -1,  1, 1), float4(0.2f, 0.2f, 1.0f),float3::ONE,  float2(0.0f, 1.0f), float3()),
	 vertex(float4(1, 1,  1, 1), float4(0.2f, 0.2f, 1.0f), float3::ONE,  float2(1.0f, 1.0f), float3()),
	 vertex(float4(-1,  1,  1, 1), float4(1.0f, 0.2f, 1.0f),  float3::ONE, float2(1.0f, 0.0f), float3()),
	 vertex(float4(-1, -1, -1, 1), float4(1.0f, 1.0f, 0.2f), float3::ONE, float2(0.0f, 0.0f), float3()),
	 vertex(float4(1, -1, -1, 1),float4(0.2f, 1.0f, 1.0f),  float3::ONE, float2(0.0f, 1.0f), float3()),
	 vertex(float4(1,  1, -1, 1), float4(1.0f, 0.3f, 0.3f), float3::ONE,  float2(1.0f, 1.0f), float3()),
	 vertex(float4(-1,  1, -1, 1),  float4(0.2f, 1.0f, 0.3f), float3::ONE,float2(1.0f, 0.0f), float3()),
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

	texture<float> noise("noise", 512, 512);

	noise::generate_fractal_image(noise, 512, 512);

	material plane_material(&s);
	plane_material.transparent = false;

	material box_material(&s);
	box_material.transparent = true;
	box_material.blend_op = blend_operator::add;
	box_material.src_factor = blend_factor::alpha;
	box_material.dst_factor = blend_factor::one_minus_alpha;

	plane_material.set_texture("noise", &noise);

	float alpha = 1;

	int w = 800;
	int h = 600;

	if (screen_init(w, h, _T("SoftRasterizer")))
		return -1;

	render_device device(screen_fb, 800, 600);
	float aspect = (float)w / (float)h;

	float3 cam_pos = float3(5.0f, 5.0f, 5.0f);
	float3 box_pos = float3(0.0f, 0.0f, 0.0f);

	camera cam(cam_pos, aspect, 45.0f, 0.5f, 500.0f, camera::projection::perspective);

	while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0) {
		screen_dispatch();
		if (screen_keys[VK_UP]) box_pos.z -= 0.2f;
		if (screen_keys[VK_DOWN]) box_pos.z += 0.2f;
		if (screen_keys[VK_LEFT]) box_pos.x += 0.2f;
		if (screen_keys[VK_RIGHT]) box_pos.x -= 0.2f;
		if (screen_keys[VK_SPACE]) alpha += 1.0f;
		if (screen_keys['W']) device.r_flag = render_flag::wire_frame;
		if (screen_keys['L']) device.r_flag = render_flag::scanline;
		if (screen_keys['S']) device.r_flag = render_flag::shaded;
		if (screen_keys['D']) device.r_flag = render_flag::depth;
		if (screen_keys['U']) device.r_flag = (render_flag)((int)render_flag::shaded | (int)render_flag::uv);
		if (screen_keys['V']) device.r_flag = (render_flag)((int)render_flag::shaded | (int)render_flag::vertex_color);


		update_misc_params(w, h, cam.near, cam.far, cam.fov);
		device.flush();
		cam.set_position(cam_pos);
		cam.set_target(float3(0.0f, 0.0f, 0.0f));
		mat4 t = mat4::translation(box_pos);
		mat4 r = mat4::rotation(float3(-1, -0.5, -1), alpha);
		mat4 m = t * r;
		draw_box(device, box_material, m, cam.view_matrix(), cam.get_projection_matrix());

		mat4 pm = mat4::translation(float3(0.0f, -1.0f, 0.0f));
		mat4 scale = mat4::scale(float3(3.0f, 1.0f, 3.0f));
		draw_plane(device, plane_material, 2, 6, 7, 3, pm * scale, cam.view_matrix(), cam.get_projection_matrix());

		screen_update();
		Sleep(1);
	}
	return 0;
}
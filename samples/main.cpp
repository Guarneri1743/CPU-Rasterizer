#include <iostream>
#include <vertex.hpp>
#include <render_device.hpp>
#include <material.hpp>
#include <camera.hpp>
#include <screen.hpp>

using namespace guarneri;
using namespace std;


vertex mesh[8] = {
	 vertex(float4(-1, -1,  1, 1), float4::ONE, float3(1.0f, 0.2f, 0.2f), float2(0.0f, 0.0f), float3()),
	 vertex(float4(1, -1,  1, 1), float4::ONE, float3(0.2f, 0.2f, 1.0f), float2(0.0f, 1.0f), float3()),
	 vertex(float4(1, 1,  1, 1), float4::ONE, float3(0.2f, 0.2f, 1.0f), float2(1.0f, 1.0f), float3()),
	 vertex(float4(-1,  1,  1, 1), float4::ONE, float3(1.0f, 0.2f, 1.0f), float2(1.0f, 0.0f), float3()),
	 vertex(float4(-1, -1, -1, 1), float4::ONE, float3(1.0f, 1.0f, 0.2f), float2(0.0f, 0.0f), float3()),
	 vertex(float4(1, -1, -1, 1), float4::ONE, float3(0.2f, 1.0f, 1.0f), float2(0.0f, 1.0f), float3()),
	 vertex(float4(1,  1, -1, 1), float4::ONE, float3(1.0f, 0.3f, 0.3f), float2(1.0f, 1.0f), float3()),
	 vertex(float4(-1,  1, -1, 1), float4::ONE, float3(0.2f, 1.0f, 0.3f), float2(1.0f, 0.0f), float3()),
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
	shader* s = new shader();
	id_t shader_id = 0;
	if (!shader_manager::singleton()->add_shader(s, shader_id)) {
		return 0;
	}
	
	material material(0);

	float alpha = 1;

	TCHAR* title = _T("Mini3d (software render tutorial) - ")
		_T("Left/Right: rotation, Up/Down: forward/backward, Space: swiuvh state");

	if (screen_init(800, 600, title))
		return -1;

	render_device device(screen_fb, 800, 600);

	float3 pos = float3(4.0f, 4.0f, 4.0f);
	camera cam(pos, (float)8/(float)6, 60.0f, 0.5f, 500.0f, camera::projection::perspective);
	cout << cam.get_projection_matrix() << endl << endl;
	cout << cam.view_matrix() << endl << endl;
	cout << cam.get_projection_matrix() * cam.view_matrix() << endl;
	cout << mat4::IDENTITY << endl <<endl;
	cout << cam.get_projection_matrix() * cam.view_matrix() * mat4::IDENTITY << endl;
	float4 pt(0, 0, 0, 1);
	cout << cam.view_matrix() * pt << endl;
	cout << cam.get_projection_matrix() * cam.view_matrix() * pt << endl;
	while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0) {
		screen_dispatch();
		if (screen_keys[VK_UP]) pos.y -= 0.01f;
		if (screen_keys[VK_DOWN]) pos.y += 0.01f;
		if (screen_keys[VK_LEFT]) alpha += 0.01f;
		if (screen_keys[VK_RIGHT]) alpha -= 0.01f;

		device.clear_color_buffer();
		cam.set_position(pos);
		cam.set_target(float3(0, 0, 0));

		/*mat4 r = mat4::rotation(float3(-1, -0.5, -1), alpha);
		mat4 s = mat4::scale(float3(0.2f, 0.2f, 0.2f));
		mat4 m = s * r;*/
		mat4 m = mat4::IDENTITY;
		draw_box(device, material, m, cam.view_matrix(), cam.get_projection_matrix());
		screen_update();
		Sleep(1);
	}
	return 0;
}
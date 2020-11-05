#include <iostream>
#include <guarneri.hpp>


using namespace guarneri;
using namespace std;

int main()
{
	mat4 m = mat4::lookat_matrix(float3(5, 5, 5), float3::ZERO, float3::UP);
	cout << m << endl;
	//void* buffer = malloc(400);
	//framebuffer fb(buffer, 3, 3);
	//for (int i = 0; i < 3; i++) {
	//	for (int j = 0; j < 3; j++) {
	//		unsigned char c = (unsigned char)(i * 100);
	//		fb.write(i, j, { c, c, c, 1});
	//	}
	//}

	//for (int i = 0; i < 3; i++) {
	//	for (int j = 0; j < 3; j++) {
	//		bitmap_color_t color;
	//		fb.read(i, j, color);
	//		cout << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << ", " << (int)color.a << endl;
	//	}
	//}
	//free(buffer);
	/*mat4 m(float4(0, 0, -1, 0), float4(0, 2, 0, 0), float4(3, 0, 0, 0), float4(50, 60, 70, 1));
	mat4 t(float4(0, 0, 3, 50), float4(0, 2, 0, 60), float4(-1, 0, 0, 70), float4(0, 0, 0, 1));
	mat4 i(float4(0, 0, 0.33333f, 0), float4(0, 0.5f, 0, 0), float4(-1, 0, 0, 0), float4(70, -30, -16.66666f, 1));

	assert(m.transpose() == t);
	assert(m.inverse() == i);

	cout << "LocalToWorldMatrix:" << endl;
	cout << m << endl;
	cout << "Transpose:" << endl;
	cout << m.transpose() << endl;
	cout << "Inverse:" << endl;
	cout << m.inverse() << endl;

	auto local2world = m;
	float3 local_pt(125, -70, 30);
	auto world_pt = local2world.transform_point(local_pt);
	assert(world_pt == float3(140, -80, -55));
	cout << "world_point: " << world_pt << endl;

	float3 dir(30, 270, -190);
	auto world_dir = local2world.transform_direction(dir);
	assert(world_dir == float3(-570, 540, -30));
	cout << "world_direction: " << world_dir << endl;

	mat4 rot = mat4::rotation(float3(1, 1, 1), 70.0f);
	cout << rot << endl; */

	/*0.56135  -0.32321  0.76186	0.00000
	0.76186	  0.56135  -0.32321	0.00000
	-0.32321  0.76186	0.56135	0.00000
	0.00000	  0.00000	0.00000	1.00000*/

	// local to world matrix
	//0.00000	0.00000	3.00000	50.00000
	//0.00000	2.00000	0.00000	60.00000
	//- 1.00000	0.00000	0.00000	70.00000
	//0.00000	0.00000	0.00000	1.00000

	// inverse
	//0.00000	0.00000 - 1.00000	70.00000
	//0.00000	0.50000	0.00000 - 30.00000
	//0.33333	0.00000	0.00000 - 16.66666
	//0.00000	0.00000	0.00000	1.00000

	// transpose
	//0.00000	0.00000 - 1.00000	0.00000
	//0.00000	2.00000	0.00000	0.00000
	//3.00000	0.00000	0.00000	0.00000
	//50.00000	60.00000	70.00000	1.00000
}

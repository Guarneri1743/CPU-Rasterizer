#include <iostream>
#include "CPURasterizer.hpp"

using namespace Guarneri;
using namespace std;

int main()
{
	Matrix4x4 m(Vector4(0, 0, -1, 0), Vector4(0, 2, 0, 0), Vector4(3, 0, 0, 0), Vector4(50, 60, 70, 1));
	Matrix4x4 t(Vector4(0, 0, 3, 50), Vector4(0, 2, 0, 60), Vector4(-1, 0, 0, 70), Vector4(0, 0, 0, 1));
	Matrix4x4 i(Vector4(0, 0, 0.33333f, 0), Vector4(0, 0.5f, 0, 0), Vector4(-1, 0, 0, 0), Vector4(70, -30, -16.66666f, 1));

	assert(m.transpose() == t);
	assert(m.inverse() == i);

	cout << "LocalToWorldMatrix:" << endl;
	cout << m << endl;
	cout << "Transpose:" << endl;
	cout << m.transpose() << endl;
	cout << "Inverse:" << endl;
	cout << m.inverse() << endl;

	auto local2world = m;
	Vector3 local_pt(125, -70, 30);
	auto world_pt = local2world.transform_point(local_pt);
	assert(world_pt == Vector3(140, -80, -55));
	cout << "world_point: " << world_pt << endl;

	Vector3 dir(30, 270, -190);
	auto world_dir = local2world.transform_direction(dir);
	assert(world_dir == Vector3(-570, 540, -30));
	cout << "world_direction: " << world_dir << endl;

	Matrix4x4 rot = Matrix4x4::rotation(Vector3(1, 1, 1), 70.0f);
	cout << rot << endl; 

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

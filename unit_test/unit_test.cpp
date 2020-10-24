#include <iostream>
#include "mat4.hpp"
using namespace guarneri;
using namespace std;

int main()
{
	mat4 m(vec4(0, 0, -1, 0), vec4(0, 2, 0, 0), vec4(3, 0, 0, 0), vec4(50, 60, 70, 1));
	mat4 t(vec4(0, 0, 3, 50), vec4(0, 2, 0, 60), vec4(-1, 0, 0, 70), vec4(0, 0, 0, 1));
	mat4 i(vec4(0, 0, 0.33333f, 0), vec4(0, 0.5f, 0, 0), vec4(-1, 0, 0, 0), vec4(70, -30, -16.66666f, 1));

	assert(m.transpose() == t);
	assert(m.inverse() == i);

	cout << "LocalToWorldMatrix:" << endl;
	cout << m << endl;
	cout << "Transpose:" << endl;
	cout << m.transpose() << endl;
	cout << "Inverse:" << endl;
	cout << m.inverse() << endl;

	auto local2world = m;
	vec3 local_pt(125, -70, 30);
	auto world_pt = local2world.transform_point(local_pt);
	assert(world_pt == vec3(140, -80, -55));
	cout << "world_point: " << world_pt << endl;

	vec3 dir(30, 270, -190);
	auto world_dir = local2world.transform_direction(dir);
	assert(world_dir == vec3(-570, 540, -30));
	cout << "world_direction: " << world_dir << endl;

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

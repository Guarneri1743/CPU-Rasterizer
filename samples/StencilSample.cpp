#include "CPURasterizer.hpp"

using namespace Guarneri;
using namespace std;

int main()
{
	CPURasterizer::prepare(800, 600, "CPU-Rasterizer");
	auto scene = Scene::deserialize("/scenes/stencil_sample.scene");
	CPURasterizer::kick_off(*scene);
	return 0;
}
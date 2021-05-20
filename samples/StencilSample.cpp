#include "CPURasterizer.hpp"

int main()
{
	Guarneri::CPURasterizer::prepare(800, 600, "CPU-Rasterizer");
	auto scene = Guarneri::Scene::deserialize("/scenes/stencil_sample.scene");
	Guarneri::CPURasterizer::kick_off(*scene);
	return 0;
}
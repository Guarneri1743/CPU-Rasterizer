#include "Application.hpp"

int main()
{
	Guarneri::Application app("CPU-Rasterizer", 800, 600);
	auto scene = Guarneri::Scene::deserialize("/scenes/lighting_sample.scene");
	app.kickoff(*scene);
	return 0;
}
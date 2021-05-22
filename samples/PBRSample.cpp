#include "Application.hpp"

int main()
{
	Guarneri::Application app("CPU-Rasterizer(lighting_demo)");
	auto scene = Guarneri::Scene::deserialize("/scenes/lighting_sample.scene");
	Guarneri::Scene::set_current(scene.get());
	app.kickoff(*scene);
	return 0;
}
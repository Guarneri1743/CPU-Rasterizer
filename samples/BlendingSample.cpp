#include "Application.hpp"

int main()
{
	Guarneri::Application app("CPU-Rasterizer(filtering_demo)");
	auto scene = Guarneri::Scene::load_asset("/scenes/blending_sample.scene");
	Guarneri::Scene::set_current(scene.get());
	app.kickoff(*scene);
	return 0;
}
#include "Application.hpp"

int main()
{
	Guarneri::Scene::open_scene("/scenes/lighting_sample.scene");
	Guarneri::Application::initialize();
	Guarneri::Application::loop();
	return 0;
}

#include "Application.hpp"

int main()
{
	Guarneri::Application::initialize();
	Guarneri::Scene::open_scene("/scenes/filtering_sample.scene");
	Guarneri::Application::loop();
	return 0;
}

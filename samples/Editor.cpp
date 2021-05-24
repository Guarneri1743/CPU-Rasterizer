#include "Application.hpp"

int main()
{
	Guarneri::Application::load_scene("/scenes/lighting_sample.scene");
	Guarneri::Application::play();
	return 0;
}

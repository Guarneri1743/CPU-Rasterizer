#include "Application.hpp"

int main()
{
	Guarneri::Application::load_scene("/scenes/filtering_sample.scene");
	Guarneri::Application::play();
	return 0;
}

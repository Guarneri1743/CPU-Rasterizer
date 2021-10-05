#include "Application.hpp"

int main()
{
	CpuRasterizor::Application::initialize();
	CpuRasterizor::Scene::open_scene("/scenes/filtering_sample.scene");
	CpuRasterizor::Application::loop();
	return 0;
}

#include "CPURasterizer.hpp"

using namespace Guarneri;
using namespace std;

bool auto_rotation = false;

int main()
{
	CPURasterizer::prepare(800, 600, "CPU-Rasterizer");
	auto scene = Scene::deserialize("/scenes/lighting_sample.scene");
	CPURasterizer::kick_off(*scene);

	/*INST(InputManager).add_on_update_evt([](void* user_data) {
		auto& bp = *reinterpret_cast<std::shared_ptr<Model>*>(user_data);
		if (INST(InputManager).is_key_down(KeyCode::LEFT)) {
			bp->transform->rotate(5.0f, 0.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::RIGHT)) {
			bp->transform->rotate(-5.0f, 0.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::G)) {
			bp->material->lighting_param.glossiness *= 2.0f;
		}
		if (INST(InputManager).is_key_down(KeyCode::H)) {
			bp->material->lighting_param.glossiness /= 2.0f;
		}
		}, & backpack);


	INST(InputManager).add_on_update_evt([](void* user_data) {
		if (INST(InputManager).is_key_down(KeyCode::SPACE))
		{
			auto_rotation = !auto_rotation;
		}
		
		Scene* s = reinterpret_cast<Scene*>(user_data);
		if (auto_rotation)
		{
			s->main_light.rotate(10.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::R)) {
			s->main_light.rotate(10.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::T)) {
			s->main_light.rotate(0.0f, 10.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::Y)) {
			s->main_light.rotate(-10.0f, 0.0f);
		}
		if (INST(InputManager).is_key_down(KeyCode::U)) {
			s->main_light.rotate(0.0f, -10.0f);
		}
		}, &demo_scene);*/

	return 0;
}
#include "Application.hpp"

int main()
{
	Guarneri::Application app("CPU-Rasterizer(filtering_demo)");
	auto scene = Guarneri::Scene::load_asset("/scenes/filtering_sample.scene");
	Guarneri::Scene::set_current(scene.get());
	app.kickoff(*scene);

	//INST(InputManager).add_on_update_evt([](void* user_data) {
	//	if (INST(InputManager).is_key_down(KeyCode::B)) {
	//		auto t = *reinterpret_cast<std::shared_ptr<Texture>*>(user_data);
	//		t->filtering = t->filtering == Filtering::BILINEAR ? Filtering::POINT : Filtering::BILINEAR;
	//	}
	//	}, & plane_albedo);

	//INST(InputManager).add_on_update_evt([](void* user_data) {
	//	if (INST(InputManager).is_key_down(KeyCode::B)) {
	//		auto t = *reinterpret_cast<std::shared_ptr<Texture>*>(user_data);
	//		t->filtering = t->filtering == Filtering::BILINEAR ? Filtering::POINT : Filtering::BILINEAR;
	//	}
	//	}, & plane_normal);

	//INST(InputManager).add_on_update_evt([](void* user_data) {
	//	if (INST(InputManager).is_key_down(KeyCode::B)) {
	//		auto t = *reinterpret_cast<std::shared_ptr<Texture>*>(user_data);
	//		t->filtering = t->filtering == Filtering::BILINEAR ? Filtering::POINT : Filtering::BILINEAR;
	//	}
	//	}, & plane_s);

	//INST(InputManager).add_on_update_evt([](void* user_data) {
	//	if (INST(InputManager).is_key_down(KeyCode::B)) {
	//		auto t = *reinterpret_cast<std::shared_ptr<Texture>*>(user_data);
	//		t->filtering = t->filtering == Filtering::BILINEAR ? Filtering::POINT : Filtering::BILINEAR;
	//	}
	//	}, & plane_ao);

	return 0;
}
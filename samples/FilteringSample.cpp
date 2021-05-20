#include "CPURasterizer.hpp"

int main()
{
	Guarneri::CPURasterizer::prepare(800, 600, "CPU-Rasterizer");
	auto scene = Guarneri::Scene::deserialize("/scenes/filtering_sample.scene");
	Guarneri::CPURasterizer::kick_off(*scene);

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
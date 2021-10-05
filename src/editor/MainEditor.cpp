#include "MainEditor.hpp"
#include <filesystem>
#include "imgui/imgui.h"
#include "Marcos.h"
#include "InputManager.hpp"
#include "Window.hpp"
#include "Singleton.hpp"
#include "GraphicsDevice.hpp"
#include "Scene.hpp"
#include "Utility.hpp"
#include "Serialization.hpp"

namespace Guarneri
{
	MainEditor::MainEditor(int x, int y, int w, int h) : BaseEditor(x, y, w, h)
	{
		no_titlebar = true;
		no_collapse = true;
		no_resize = true;
		no_close = true;
		no_move = true;
		no_bring_to_front = true;
		title = "Main";
	}

	//todo
	void MainEditor::draw_file_menu()
	{
		if (ImGui::MenuItem("NewScene", "Ctrl+N"))
		{
			// todo: support create default scene
			
			//Scene* scene = new Scene();
			//scene->initialize();
			//DirectionalLight main_light;
			//main_light.position = tinymath::vec3f(0.0f, 8.0f, 0.0f);
			//main_light.ambient = tinymath::Color(0.05f, 0.05f, 0.05f, 1.0f);
			//main_light.diffuse = tinymath::Color(0.9f, 0.9f, 1.0f, 1.0f);
			//main_light.specular = tinymath::Color(1.0f, 1.0f, 1.0f, 1.0f);
			//main_light.intensity = 1.3f;
			//scene->set_main_light(main_light);

			//// setup Camera
			//scene->main_cam->transform->set_world_position(tinymath::vec3f(3.4f, 21.8f, -14.0f));
			//scene->main_cam->set_near(0.5f);
			//scene->main_cam->transform->set_world_angle(-311.0f, -15.0f, 0.0f);

			//// Plane
			//Model* plane = new Model();
			//Serializer::deserialize("/models/plane.model", *plane);
			//scene->add(std::shared_ptr<Model>(plane));
			//scene->shadow_bias = 0.00125f;
			//scene->color_space = ColorSpace::Linear;
			//scene->work_flow = PBRWorkFlow::kMetallic;
			//Scene::open_scene(scene);
		}

		if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
		{
			Serializer::serialize(*Scene::current(), Scene::current()->get_asset_path());
		}

		if (ImGui::MenuItem("Save As.."))
		{
			// todo: suppor open file browser
		}
	}

	void MainEditor::on_gui()
	{
		this->rect = tinymath::Rect(0, 0, Window::main()->get_width(), Window::main()->get_height());
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				draw_file_menu();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}
}
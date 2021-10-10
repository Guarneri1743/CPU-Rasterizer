#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Define.hpp"

namespace CpuRasterizor
{
	struct DirectionalLight;
	struct PointLight;
	class Camera;
	class Renderer;
	class SkyboxRenderer;
	class Transform;
	class Model;
	class CubeMap;

	class Scene
	{
	public:
		std::string name;

		// editor
		Transform* selection;

		// lights
		std::unique_ptr<DirectionalLight> main_light;
		std::vector<PointLight> point_lights;

		// objects
		std::vector<std::shared_ptr<Renderer>> objects;
		std::vector<std::shared_ptr<Renderer>> transparent_objects;
		std::unique_ptr<SkyboxRenderer> skybox;
		std::shared_ptr<CubeMap> cubemap;

		// cam
		std::unique_ptr<Camera> main_cam;

		// params
		bool enable_skybox;
		bool enable_shadow;
		bool pcf_on;
		float shadow_bias;
		ColorSpace color_space;
		PBRWorkFlow work_flow;

		std::string asset_path;

	public:
		Scene();
		Scene(std::string name);
		~Scene();
		void initialize();
		void add(std::shared_ptr<Model> model);
		void update();
		void set_main_light(const DirectionalLight& light);
		void add_point_light(const PointLight& light);
		void render();
		void render_shadow();
		void render_objects();
		void draw_gizmos();
		void debug_scene();
		void resize_shadowmap(size_t w, size_t h);
		void get_shadowmap_size(size_t& w, size_t& h);
		void set_cubemap(std::string path);
		std::string get_asset_path() { return asset_path; }
		static Scene* current() { return current_scene; }
		static void open_scene(const char* path);

	private:
		uint32_t shadowmap_id;
		static Scene* current_scene;
	};
}
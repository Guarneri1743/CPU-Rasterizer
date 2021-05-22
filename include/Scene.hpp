#ifndef _SCENE_
#define _SCENE_
#include <vector>
#include <memory>
#include "Define.hpp"
#include "Light.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "SkyboxRenderer.hpp"

namespace Guarneri
{
	class Scene
	{
	public:
		std::string name;

		// lights
		DirectionalLight main_light;
		std::vector<PointLight> point_lights;

		// objects
		std::vector<std::shared_ptr<Renderer>> objects;
		std::vector<std::shared_ptr<Renderer>> transparent_objects;
		std::unique_ptr<SkyboxRenderer> skybox;

		// cam
		std::unique_ptr<Camera> main_cam;

		// params
		bool enable_skybox;
		bool enable_shadow;
		bool pcf_on;
		float shadow_bias;
		ColorSpace color_space;
		PBRWorkFlow work_flow;

		// debug cam
		float debug_cam_distance;
		float debug_world_cam_distance;
		std::unique_ptr<Camera> debug_cam;
		std::unique_ptr<Camera> world_debug_cam;

	public:
		~Scene();
		void initialize();
		void add(std::shared_ptr<Model> model);
		void update();
		void draw_camera_coords();
		void draw_world_coords();
		void set_main_light(const DirectionalLight& light);
		void add_point_light(const PointLight& light);
		void render();
		void render_shadow();
		void render_objects();
		void draw_gizmos();

		static std::unique_ptr<Scene> create(const std::string& name);
		static void serialize(const Scene& scene, const std::string& path);
		static std::unique_ptr<Scene> deserialize(const std::string& path);

	private:
		Scene(std::string name);
	};
}
#endif
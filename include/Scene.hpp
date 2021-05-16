#ifndef _SCENE_
#define _SCENE_
#include <vector>
#include <memory>
#include "Light.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "SkyboxRenderer.hpp"

namespace Guarneri
{
	class Scene
	{
	public:
		DirectionalLight main_light;
		std::vector<PointLight> point_lights;
		std::vector<std::shared_ptr<Renderer>> objects;
		std::vector<std::shared_ptr<Renderer>> transparent_objects;
		std::unique_ptr<SkyboxRenderer> skybox;
		bool enable_skybox;
		std::unique_ptr<Camera> main_cam;
		std::unique_ptr<Camera> debug_cam;
		std::unique_ptr<Camera> world_debug_cam;
		float debug_cam_distance;
		float debug_world_cam_distance;

	public:
		Scene();
		~Scene();
		void initialize();
		void add(std::shared_ptr<Renderer> rdr);
		void update();
		void draw_camera_coords();
		void draw_world_coords();
		void set_main_light(const DirectionalLight& light);
		void add_point_light(const PointLight& light);
		void render();
		void render_shadow();
		void render_objects();
		void draw_gizmos();
	};
}
#endif
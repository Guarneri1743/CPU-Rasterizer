#pragma once
#include <guarneri.hpp>

namespace guarneri{
	class scene{
	public:
		scene() {
			deserialize();
		}

		~scene() {

		}

	public:
		std::vector<std::shared_ptr<renderer>> objects;
		std::vector<std::shared_ptr<renderer>> transparent_objects;

		// todo: serialzie & deserialize scene data
		void deserialize() {
			// backpack
			auto backpack = std::shared_ptr<model>();
			backpack->load_from_file(res_path() + "/backpack/backpack.obj");
			objects.push_back(std::make_shared<renderer>(backpack));

			// cube
			auto box_material = std::make_shared<material>();
			box_material->transparent = true;
			box_material->blend_op = blend_operator::add;
			box_material->src_factor = blend_factor::src_alpha;
			box_material->dst_factor = blend_factor::one_minus_src_alpha;
			box_material->zwrite_mode = zwrite::off;
			transparent_objects.push_back(std::make_shared<renderer>(model_generator::cube(box_material)));

			// plane
			auto tex_path = res_path() + "/textures/pavingstones_decorative2_2k_h_1.jpg";
			auto plane_tex = std::make_shared<texture>(tex_path.c_str(), albedo_prop);
			auto noise = std::make_shared<texture>(albedo_prop, 512, 512, texture_format::rgba);
			noise::generate_fractal_image(noise, 512, 512);
			auto plane_material = std::make_shared<material>();
			plane_material->transparent = false;
			plane_material->set_texture(albedo_prop, plane_tex);
			auto plane = model_generator::plane(plane_material);
			plane->transform.translate(float3::DOWN);
			plane->transform.scale(float3(3.0f, 1.0f, 3.0f));
			objects.push_back(std::make_shared<renderer>(plane));
		}

		void render() {
			for (auto& obj : objects) {
				obj->render();
			}

			for (auto& obj : transparent_objects) {
				obj->render();
			}
		}
	};
}